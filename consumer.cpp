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
    if (argc != 2)
    {
        return -1;
    }
    int buffer = stoi(argv[1]);
    key_t key = ftok("lab5",6835);
    key_t semkey = ftok("lab5sem",68355);
    int shmid = shmget(key,sizeof(shmq),0666|IPC_CREAT);
    int semid = semget(semkey, 3, 0666|IPC_CREAT);
    
    semctl(semid,0,SETVAL,1);
    semctl(semid,1,SETVAL,0);
    semctl(semid,2,SETVAL,buffer);

    shmq q;
    sembuf sem[3];
    for (int i = 0; i < 3; i++)
    {
        sem[i].sem_num = i;
        sem[i].sem_flg = 0;
        sem[i].sem_op = 0;
    }

    q.maxsize = buffer;

    char * sh = (char *) shmat(shmid, (void*)0, 0);
    memcpy(sh, &q, sizeof(q));
    shmdt(sh);

    for (int i = 0; i < 5; i++)
    {
        cout << "HI!" << "\n";
        this_thread::sleep_for(std::chrono::milliseconds(10000));
    }
    shmctl(shmid,IPC_RMID,NULL);
    return 0;
}