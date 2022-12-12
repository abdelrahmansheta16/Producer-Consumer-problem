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
    int shmid = shmget(key,sizeof(shmq),0666|IPC_CREAT);
    if (shmid == -1) 
    {
        perror("Shared memory");
        return 1;
    }

    shmq q;

    q.maxsize = buffer;

    char * sh = (char *) shmat(shmid, (void*)0, 0);
    memcpy(sh, &q, sizeof(q));
    shmdt(sh);

    for (int i = 0; i < 10; i++)
    {
        cout << "HI!" << "\n";
        this_thread::sleep_for(std::chrono::milliseconds(10000));
    }
    shmctl(shmid,IPC_RMID,NULL);
    return 0;
}