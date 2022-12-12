#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <thread>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stack>

using namespace std;

struct shmq
{
    int maxsize = 0;
    stack<string> name;
    stack<double> price;
};


int main(int argc, char** argv)
{
    if (argc != 6)
    {
        return -1;
    }
    string name = argv[1];
    double mean = stod(argv[2]);
    double sd = stod(argv[3]);
    int sleep = stoi(argv[4]);
    int buffer = stoi(argv[5]);
    double price;
    default_random_engine generator;
    normal_distribution<double> distribution(mean,sd);
    key_t key = ftok("lab5",6835);
    int shmid = shmget(key,1024,0666|IPC_CREAT);
    if (shmid == -1) 
    {
        perror("Shared memory");
        return 1;
    }
    

    for (int i = 0; i < 30; i++)
    {
        price = distribution(generator);
        
        shmq * quu = (shmq *)shmat(shmid, (void*)0, 0);
        cout << quu->maxsize;
        quu->name.push(name);
        quu->price.push(price);
        cout << price << " ";
        cout << quu->price.size() << "\n";
        shmdt(quu);
        this_thread::sleep_for(std::chrono::milliseconds(sleep));
    }

    return 0;
}