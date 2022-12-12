#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/shm.h>
#include <string.h>
#include "queue.h"

struct arg_struct
{
    int name;
    double mean;
    double deviation;
    int sleep;
};

struct price
{
    struct Queue *prevValues[11];
    struct Queue *prevAvg[11];
};

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

#define BufferSize 5

sem_t s, n, e;
int i;
struct price *shared_memory;
char buff[100];
int shmid;

// void *updateSM(double randomNum, int comm)
// {
// }
void *producer(void *arguments)
{
    printf("1");
    struct arg_struct *args = (struct arg_struct *)arguments;
    int comm = args->name;
    double randomNum;
    printf("2");
    while (true)
    {
        printf("3");
        double currentPrice = randn(args->mean, args->deviation);
        sem_wait(&e);
        sem_wait(&s);
        double currentAvg;
        int priceDigits;
        int avgDigits;
        // update prices
        printf("4");
        if (shared_memory->prevValues[comm]->size < 4)
        {
            enqueue(shared_memory->prevValues[comm], currentPrice);
        }
        else
        {
            dequeue(shared_memory->prevValues[comm]);
            enqueue(shared_memory->prevValues[comm], currentPrice);
        }
        if (shared_memory->prevValues[comm]->size > 0)
        {
            int size = shared_memory->prevValues[comm]->size;
            double sum = 0, avg = 0;
            for (int i = 0; i < size; i++)
            {
                sum += shared_memory->prevValues[comm]->array[i];
            }
            avg = sum / size;
            enqueue(shared_memory->prevAvg[comm], avg);
        }
        printf("5");
        sleep(args->sleep);
        sem_post(&s);
        sem_post(&n);
        printf("6");
    }
}

void *consumer()
{
    while (true)
    {
        sem_wait(&n);
        sem_wait(&s);
        // struct price *shared_memory;
        int shmid;
        shmid = shmget((key_t)2345, 1024, 0666);
        shared_memory = shmat(shmid, NULL, 0);
        printf("Process attached at %p\n", shared_memory);
        sem_post(&s);
        sem_post(&e);
    }
}

void main()
{
    int num; // this makes a vairable for you to store the value in
    int x = 0;

    printf("enter number of processes: "); // this prints a prompt for the user
    scanf("%d", &num);
    sem_init(&s, 0, 0);
    sem_init(&n, 0, 0);
    sem_init(&e, 0, BufferSize);
    shmid = shmget((key_t)2345, sizeof(struct price) * 100, 0666 | IPC_CREAT); // creates shared
    shared_memory = (struct price *)shmat(shmid, NULL, 0);
    for (int i = 0; i < 11; i++)
    {
        shared_memory->prevValues[i] = createQueue(4);
        shared_memory->prevAvg[i] = createQueue(4);
    }
    pthread_t pro[num], con;
    struct arg_struct args[num];
    for (int i = 0; i < num; i++)
    {
        printf("hello\n");
        printf("Enter Commodity name of process %d:\n ", i); // this prints a prompt for the user
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
    sem_destroy(&s);
    sem_destroy(&n);
    sem_destroy(&e);
}