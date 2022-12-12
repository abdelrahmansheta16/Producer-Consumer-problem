#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/shm.h>
#include <string.h>

/*
This program provides a possible solution for producer-consumer problem using mutex and semaphore.
I have used 5 producers and 5 consumers to demonstrate the solution. You can always play with these values.
*/

// #define MaxItems 5   // Maximum items a producer can produce or a consumer can consume
#define BufferSize 5 // Size of the buffer

struct arg_struct
{
    int name;
    double mean;
    double deviation;
    int sleep;
};

// struct price
// {
//     struct Queue *prevValues[11];
//     struct Queue *prevAvg[11];
// };

double
randn(double mu, double sigma)
{
    double U1, U2, W, mult;
    static double X1, X2;
    static int call = 0;

    if (call == 1)
    {
        call = !call;
        return (mu + sigma * (double)X2);
    }

    do
    {
        U1 = -1 + ((double)rand() / RAND_MAX) * 2;
        U2 = -1 + ((double)rand() / RAND_MAX) * 2;
        W = pow(U1, 2) + pow(U2, 2);
    } while (W >= 1 || W == 0);

    mult = sqrt((-2 * log(W)) / W);
    X1 = U1 * mult;
    X2 = U2 * mult;

    call = !call;

    return (mu + sigma * (double)X1) > 0 ? (mu + sigma * (double)X1) : randn(mu, sigma);
}

char *getCommName(int comm)
{
    switch (comm)
    {
    case 1:
        return "GOLD";
        break;
    case 2:
        return "SILVER";
        break;
    case 3:
        return "CRUDEOIL";
        break;
    case 4:
        return "NATURALGAS";
        break;
    case 5:
        return "ALUMINIUM";
        break;
    case 6:
        return "COPPER";
        break;
    case 7:
        return "NICKEL";
        break;
    case 8:
        return "LEAD";
        break;
    case 9:
        return "ZINC";
        break;
    case 10:
        return "MENTHAOIL";
        break;
    case 11:
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

    printf("currentPrice: %lf\n", currentPrice[1]);
    printf("prevPrice: %lf\n", prevPrice[1]);
    printf("currentAvg: %lf\n", currentAvg[1]);
    printf("prevAvg: %lf\n", prevAvg[1]);
    printf("+-------------------------------------+\n");
    printf("| Currency\t|  Price   | AvgPrice |\n");
    printf("+-------------------------------------+\n");
    for (int i = 0; i < 11; i++)
    {
        int priceDigits = digits(currentPrice[i]);
        int avgDigits = digits(currentAvg[i]);
        printf("| ");
        // char currentComm[] = getCommName(i + 1);
        strlen(getCommName(i + 1)) > 5 ? printf("%s\t| ", getCommName(i + 1)) : printf("%s  \t| ", getCommName(i + 1));
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

double (*array)[6];
char buff[100];
int shmid;
sem_t empty;
sem_t full;
int in = 0;
int out = 0;
int buffer[BufferSize];
pthread_mutex_t mutex;

void *producer(void *arguments)
{
    double item;
    int i = 0;
    struct arg_struct *args = (struct arg_struct *)arguments;
    int comm = args->name;
    while (true)
    {
        // printf("mean: %lf\n",args->mean);
        // printf("deviation: %lf\n",args->mean);
        item = randn(args->mean, args->deviation); // Produce an random item
        // printf("item: %lf\n",args->mean);
        sem_wait(&empty);
        pthread_mutex_lock(&mutex);
        array = shmat(shmid, NULL, 0);
        for (int x = 5; x > 0; x--)
        {
            array[comm][x] = array[comm][x - 1];
        }
        array[comm][0] = item;
        buffer[in] = item;
        // printf("Producer %d: Insert Item %lf at %d\n", args->name, array[comm][i % 5], i % 5);
        in = (in + 1) % BufferSize;
        i++;
        usleep(args->sleep * 1000);
        pthread_mutex_unlock(&mutex);
        sem_post(&full);
    }
}
void *consumer()
{
    int i = 0;
    while (true)
    {
        sem_wait(&full);
        pthread_mutex_lock(&mutex);
        // shmid = shmget((key_t)1235, MaxItems * sizeof(int), 0666);
        int item = buffer[out];
        array = shmat(shmid, NULL, 0);
        double currentPrice[11];
        double prevPrice[11];
        double currentAvg[11];
        double prevAvg[11];
        for (int x = 0; x < 11; x++)
        {
            currentPrice[x] = array[x][0];
            prevPrice[x] = array[x][1];
        }
        for (int x = 0; x < 11; x++)
        {
            double sum = 0.0;
            double prevSum = 0.0;
            for (int i = 0; i < 5; i++)
            {
                sum = sum + array[x][i];
                prevSum = prevSum + array[x][i + 1];
            }
            currentAvg[x] = (sum / 5.0);
            prevAvg[x] = (prevSum / 5.0);
        }
        printf("\e[1;1H\e[2J");
        printUpdates(currentPrice, prevPrice, currentAvg, prevAvg);
        // detach from shared memory
        shmdt(array);
        // destroy the shared memory
        shmctl(shmid, IPC_RMID, NULL);
        out = (out + 1) % BufferSize;
        i++;
        pthread_mutex_unlock(&mutex);
        sem_post(&empty);
    }
}

int main()
{
    // ftok to generate unique key
    key_t key = ftok("shmfile", rand()%50);
    shmid = shmget(key, 10 * sizeof(int), 0666 | IPC_CREAT);
    pthread_t pro[5], con;
    pthread_mutex_init(&mutex, NULL);
    sem_init(&empty, 0, BufferSize);
    sem_init(&full, 0, 0);

    int num; // this makes a vairable for you to store the value in
    struct arg_struct args[num];
    printf("enter number of processes: "); // this prints a prompt for the user
    scanf("%d", &num);

    for (int i = 0; i < num; i++)
    {
        // this prints a prompt for the user
        printf("0-GOLD \t\t 1-SILVER\n2-CRUDE OIL \t 3-NATURAL GAS\n4-ALUMINIUM \t 5-COPPER\n6-NICKEL \t 7-LEAD\n8-ZINC \t\t 9-MENTHA OIL\n10-COTTON\n");
        printf("Enter Commodity name of process %d:\n", i);
        scanf("%d", &args[i].name);
        printf("Enter Commodity Price Mean of process %d: ", i); // this prints a prompt for the user
        scanf("%lf", &args[i].mean);
        printf("Enter Commodity Price Standard Deviation of process %d: ", i); // this prints a prompt for the user
        scanf("%lf", &args[i].deviation);
        printf("Enter Length of the sleep interval in milliseconds of process %d: ", i); // this prints a prompt for the user
        scanf("%d", &args[i].sleep);
    };
    for (int i = 0; i < num; i++)
    {
        pthread_create(&pro[i], NULL, (void *)producer, (void *)&args[i]);
    }
    pthread_create(&con, NULL, (void *)consumer, NULL);

    for (int i = 0; i < num; i++)
    {
        pthread_join(pro[i], NULL);
    }
    pthread_join(con, NULL);

    pthread_mutex_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);

    return 0;
}
