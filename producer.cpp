#include <iostream>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/shm.h>
#include <unistd.h>
#include <random>
#include <stack>
#include <string.h>

using namespace std;

struct arg_struct
{
    char name[100];
    double mean;
    double deviation;
    int sleep;
    int bufferSize;
    int shmid;
};

struct sharedMemory
{
    double array[11][6];
    sem_t empty;
    sem_t full;
    pthread_mutex_t mutex;
};

int converToInt(string comm)
{
    if (comm.compare("GOLD") == 0)
    {
        return 0;
    }
    else if (comm.compare("SILVER") == 0)
    {
        return 1;
    }
    else if (comm.compare("CRUDEOIL") == 0)
    {
        return 2;
    }
    else if (comm.compare("NATURALGAS") == 0)
    {
        return 3;
    }
    else if (comm.compare("ALUMINIUM") == 0)
    {
        return 4;
    }
    else if (comm.compare("COPPER") == 0)
    {
        return 5;
    }
    else if (comm.compare("NICKEL") == 0)
    {
        return 6;
    }
    else if (comm.compare("LEAD") == 0)
    {
        return 7;
    }
    else if (comm.compare("ZINC") == 0)
    {
        return 8;
    }
    else if (comm.compare("MENTHAOIL") == 0)
    {
        return 9;
    }
    else if (comm.compare("COTTON") == 0)
    {
        return 10;
    }
    return 0;
}
int shmid;
struct sharedMemory *shmemory;

void producer(arg_struct args)
{
    // cout << "hello";
    // cout << args.name;
    default_random_engine generator;
    normal_distribution<double> distribution(args.mean, args.deviation);
    double item;
    string comm = args.name;
    int commInt = converToInt(comm);
    sharedMemory *shmemory;
    while (true)
    {
        // cout << args.name;
        // cout << args.mean;
        // cout << args.deviation;
        // cout << args.bufferSize;
        item = distribution(generator); // Produce an random item
        sem_wait(&shmemory->empty);
        pthread_mutex_lock(&shmemory->mutex);
        shmemory = (sharedMemory *)shmat(shmid, NULL, 0);
        for (int x = 5; x > 0; x--)
        {
            shmemory->array[commInt][x] = shmemory->array[commInt][x - 1];
        }
        shmemory->array[commInt][0] = item;
        usleep(20 * 1000);
        pthread_mutex_unlock(&shmemory->mutex);
        sem_post(&shmemory->full);
    }
}
int main(int argc, char **argv)
{
    sem_t empty;
    sem_t full;
    pthread_mutex_t mutex;
    arg_struct args;
    // int name, mean, deviation, sleep, bufferSize;
    cout << "You have entered " << argc
         << " arguments:"
         << "\n";

    if (argc == 6)
    {
        for (int i = 0; i < argc; ++i)
        {
            cout << argv[i] << "\n";
        }
        // // key_t key = ftok("shmfile", 333);
        // sharedMemory *shmemory;
        shmid = shmget((key_t)105, 4096, 0666);
        shmemory = (sharedMemory *)shmat(shmid, NULL, 0);
        sscanf(argv[5], "%d", &args.bufferSize);
        strcpy(args.name, argv[1]);
        sscanf(argv[2], "%lf", &args.mean);
        sscanf(argv[3], "%lf", &args.deviation);
        sscanf(argv[4], "%d", &args.sleep);

        // args.name = argv[1];
        // args.mean = 0.1;
        // args.deviation = 0.01;
        // args.sleep = 20;

        producer(args);
    }
    else
    {
        std::cerr << "missing arguments";
    }
    return 0;
}