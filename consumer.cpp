#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/shm.h>
#include <cstring>
#include <iostream>
#include <stack>
using namespace std;

/*
This program provides a possible solution for producer-consumer problem using mutex and semaphore.
I have used 5 producers and 5 consumers to demonstrate the solution. You can always play with these values.
*/

// #define MaxItems 5   // Maximum items a producer can produce or a consumer can consume

struct arg_struct
{
    int name;
    double mean;
    double deviation;
    int sleep;
};
struct sharedMemory
{
    double array[11][6];
    sem_t empty;
    sem_t full;
    pthread_mutex_t mutex;
};

string getCommName(int comm)
{
    switch (comm)
    {
    case 0:
        return "GOLD";
        break;
    case 1:
        return "SILVER";
        break;
    case 2:
        return "CRUDEOIL";
        break;
    case 3:
        return "NATURALGAS";
        break;
    case 4:
        return "ALUMINIUM";
        break;
    case 5:
        return "COPPER";
        break;
    case 6:
        return "NICKEL";
        break;
    case 7:
        return "LEAD";
        break;
    case 8:
        return "ZINC";
        break;
    case 9:
        return "MENTHAOIL";
        break;
    case 10:
        return "COTTON";
        break;
    default:
        return "ALUMINIUM";
    }
    return "ALUMINIUM";
}
int digits(int number)
{
    int digits = 0;
    if (number == 0)
    {
        return 1;
    }
    while (number > 0)
    {
        number /= 10;
        digits++;
    }
    return digits;
}

void printUpdates(double *currentPrice, double *currentAvg, double *prevPrice, double *prevAvg)
{

    printf("currentPrice: %lf\n", currentPrice[3]);
    printf("prevPrice: %lf\n", prevPrice[3]);
    printf("currentAvg: %lf\n", currentAvg[3]);
    printf("prevAvg: %lf\n", prevAvg[3]);
    printf("+-------------------------------------+\n");
    printf("| Currency\t|  Price   | AvgPrice |\n");
    printf("+-------------------------------------+\n");
    for (int i = 0; i < 11; i++)
    {
        int priceDigits = digits(currentPrice[i]);
        int avgDigits = digits(currentAvg[i]);
        printf("| ");
        // char currentComm[] = getCommName(i + 1);
        if (getCommName(i).length() > 5)
        {
            cout << getCommName(i) << "\t"
                 << "|";
        }
        else
        {
            cout << getCommName(i) << "\t\t"
                 << "|";
        }
        // for (int x = 0; x < 3; x++)
        // {
        //     printf(" ");
        // }
        if (currentPrice[i] == 0)
        {

            printf("\033[0;34m");
            printf("%7.2lf  ", currentPrice[i]);
            printf("\033[0m");
            printf("| ");
        }
        else if (currentPrice[i] > prevPrice[i])
        {
            printf("\033[0;32m");
            printf("%7.2lf↑ ", currentPrice[i]);
            printf("\033[0m");
            printf("| ");
        }
        else if (currentPrice[i] < prevPrice[i])
        {
            printf("\033[0;31m");
            printf("%7.2lf↓ ", currentPrice[i]);
            printf("\033[0m");
            printf("| ");
        }
        else
        {
            printf("\033[0;33m");
            printf("%7.2lf  ", currentPrice[i]);
            printf("\033[0m");
            printf("| ");
        }
        // for (int x = 0; x < (5 - avgDigits); x++)
        // {
        //     printf(" ");
        // }
        if (currentAvg[i] == 0)
        {

            printf("\033[0;34m");
            printf("%7.2lf  ", currentAvg[i]);
            printf("\033[0m");
            printf("| ");
        }
        else if (currentAvg[i] > prevAvg[i])
        {
            printf("\033[0;32m");
            printf("%7.2lf↑ ", currentAvg[i]);
            printf("\033[0m");
            printf("| ");
        }
        else if (currentAvg[i] < prevAvg[i])
        {
            printf("\033[0;31m");
            printf("%7.2lf↓ ", currentAvg[i]);
            printf("\033[0m");
            printf("| ");
        }
        else
        {
            printf("\033[0;33m");
            printf("%7.2lf  ", currentAvg[i]);
            printf("\033[0m");
            printf("| ");
        }
        printf("\n");
    }
    printf("+-------------------------------------+\n");
}

int shmid;
struct sharedMemory *shmemory;
void consumer(int bufferSize)
{
    while (true)
    {
        sem_wait(&shmemory->full);
        pthread_mutex_lock(&shmemory->mutex);
        shmemory = (sharedMemory *)shmat(shmid, NULL, 0);
        // shmid = shmget((key_t)1235, MaxItems * sizeof(int), 0666);
        double currentPrice[11];
        double prevPrice[11];
        double currentAvg[11];
        double prevAvg[11];
        // for (int x = 0; x < 11; x++)
        // {
        //     currentPrice[x] = shmemory->price[x].top();
        //     double temp = shmemory->price[x].top();
        //     shmemory->price[x].pop();
        //     prevPrice[x] = shmemory->price[x].top();
        //     shmemory->price[x].push(temp);
        // }
        for (int x = 0; x < 11; x++)
        {
            currentPrice[x] = shmemory->array[x][0];
            prevPrice[x] = shmemory->array[x][1];
        }
        for (int x = 0; x < 11; x++)
        {
            double sum = 0.0;
            double prevSum = 0.0;
            for (int i = 0; i < 5; i++)
            {
                sum = sum + shmemory->array[x][i];
                prevSum = prevSum + shmemory->array[x][i + 1];
            }
            currentAvg[x] = (sum / 5.0);
            prevAvg[x] = (prevSum / 5.0);
        }
        printf("\e[1;1H\e[2J");
        printUpdates(currentPrice, prevPrice, currentAvg, prevAvg);
        // // detach from shared memory
        // shmdt(shmemory);
        // // destroy the shared memory
        // shmctl(shmid, IPC_RMID, NULL);

        pthread_mutex_unlock(&shmemory->mutex);
        sem_post(&shmemory->empty);
    }
}

int main(int argc, char **argv)
{
    int bufferSize;
    sem_t empty;
    sem_t full;
    pthread_mutex_t mutex;
    cout << "You have entered " << argc
         << " arguments:"
         << "\n";

    for (int i = 0; i < argc; ++i)
        cout << argv[i] << "\n";
    if (argc == 2)
    {
        for (int i = 0; i < argc; ++i)
            cout << argv[i] << "\n";
        shmid = shmget((key_t)105, 4096, 0666 | IPC_CREAT);
        shmemory = (sharedMemory *)shmat(shmid, NULL, 0);
        sscanf(argv[1], "%d", &bufferSize);
        pthread_mutex_init(&mutex, NULL);
        sem_init(&empty, 0, bufferSize);
        sem_init(&full, 0, 0);
        shmemory->mutex = mutex;
        shmemory->empty = empty;
        shmemory->full = full;
        consumer(bufferSize);
        // pthread_mutex_destroy(&shmemory->mutex);
        // sem_destroy(&shmemory->empty);
        // sem_destroy(&shmemory->full);
    }
    else
    {
        std::cerr << "missing arguments";
    }
    return 0;
}
