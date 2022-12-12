#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <thread>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stack>
#include <string.h>
#include <sys/sem.h>

using namespace std;

struct shmq
{
    int maxsize = 0;
    int current = 0;
    char name[100][15];
    double price[100];
};

int main(int argc, char** argv)
{
    if (argc != 6)
    {
        return -1;
    }
    char * name = argv[1];
    double mean = stod(argv[2]);
    double sd = stod(argv[3]);
    int sleep = stoi(argv[4]);
    int buffer = stoi(argv[5]);
    double price;
    default_random_engine generator;
    normal_distribution<double> distribution(mean,sd);
    key_t key = ftok("lab5",6835);
    key_t semkey = ftok("lab5sem",68355);
    int shmid = shmget(key,sizeof(shmq),0666|IPC_CREAT);
    int semid = semget(semkey, 3, 0666|IPC_CREAT);
    
    shmq q;
    sembuf sem[3];
    for (int i = 0; i < 3; i++)
    {
        sem[i].sem_num = i;
        sem[i].sem_flg = 0;
        sem[i].sem_op = 0;
    }
    

    while (1)
    {
        price = distribution(generator);

        sem[0].sem_op = -1;
        sem[0].sem_flg = SEM_UNDO;
        sem[2].sem_op = -1;
        sem[2].sem_flg = SEM_UNDO;

        semop(semid,&sem[2],1);
        semop(semid,&sem[0],1);
        
        char * sh = (char *) shmat(shmid, (void*)0, 0);
        memcpy(&q, sh, sizeof(q));
        strcpy(q.name[q.current],name);
        q.price[q.current] = price;

        q.current++;
        
        cout << q.name[q.current-1] << " ";
        cout << q.price[q.current-1] << " " << q.current << "\n";
        memcpy(sh, &q, sizeof(q));
        shmdt(sh);

        sem[0].sem_op = 1;
        sem[1].sem_op = 1;
        semop(semid,&sem[0],1);
        semop(semid,&sem[1],1);

        this_thread::sleep_for(std::chrono::milliseconds(sleep));
    }

    return 0;
}