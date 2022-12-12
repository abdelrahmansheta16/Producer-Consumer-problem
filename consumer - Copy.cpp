#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <thread>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stack>
#include <shared_mutex>

using namespace std;

struct shmq
{
    int maxsize = 0;
    stack<string> name;
    stack<double> price;
};


int main(int argc, char** argv)
{
    if (argc != 2)
    {
        return -1;
    }
    int buffer = stoi(argv[1]);
    shmq * q;
    key_t key = ftok("lab5",6835);
    int shmid = shmget(key,1024,0666|IPC_CREAT);
    if (shmid == -1) 
    {
        perror("Shared memory");
        return 1;
    }

    void * sh = shmat(shmid, (void*)0, 0);
    q = new (sh) shmq;
    q->maxsize = buffer;
    shmdt(sh);

    for (int i = 0; i < 30; i++)
    {
        cout << "HI!" << "\n";
        this_thread::sleep_for(std::chrono::milliseconds(10000));
    }
    
    return 0;
}