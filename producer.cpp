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
    shmq q;
    key_t key = ftok("lab5",6835);
    int shmid = shmget(key,sizeof(shmq),0666);
    if (shmid == -1) 
    {
        perror("Shared memory");
        return 1;
    }
    

    for (int i = 0; i < 30; i++)
    {
        price = distribution(generator);
        
        char * sh = (char *) shmat(shmid, (void*)0, 0);
        memcpy(&q, sh, sizeof(q));
        strcpy(q.name[q.current],name);
        q.price[q.current] = price;

        q.current++;
        if (q.current > buffer)
        {
            for (int i = 0; i < buffer; i++)
            {
                q.price[i] = q.price[i+1];
                strcpy(q.name[i],q.name[i+1]);
            }
            q.current--;
        }
        

        cout << q.name[q.current-1] << " ";
        cout << q.price[q.current-1] << " " << q.current << "\n";
        memcpy(sh, &q, sizeof(q));
        shmdt(sh);
        this_thread::sleep_for(std::chrono::milliseconds(sleep));
    }

    return 0;
}