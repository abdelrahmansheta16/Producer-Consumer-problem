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
#include <time.h>
#include <ctime>

using namespace std;

struct shmq
{
    int maxsize = 0;
    int current = 0;
    char name[100][15];
    double price[100];
};

static void
displayClock()
{
    struct timespec ts;

    if (clock_gettime(CLOCK_REALTIME, &ts) == -1)
    {
        perror("clock_gettime");
        exit(EXIT_FAILURE);
    }
    time_t now = ts.tv_sec;
    // convert now to string form
    tm *ltm = localtime(&now);
    cout << "[" << 1 + ltm->tm_mon << "/" << ltm->tm_mday << "/" << 1900 + ltm->tm_year << " " << 5 + ltm->tm_hour << ":" << 30 + ltm->tm_min << ":" << ltm->tm_sec << "." << ts.tv_nsec / 1000000 << "] ";
    // cout << "Year:" << 1900 + ltm->tm_year << endl;
    // cout << "Month: " << 1 + ltm->tm_mon << endl;
    // cout << "Day: " << ltm->tm_mday << endl;
    // cout << "Time: " << 5 + ltm->tm_hour << ":";
    // cout << 30 + ltm->tm_min << ":";
    // cout << ltm->tm_sec << endl;
}

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

        displayClock();
        cout << name << ": generating a new value " << price << "\n";

        sem[0].sem_op = -1;
        sem[0].sem_flg = SEM_UNDO;
        sem[2].sem_op = -1;
        sem[2].sem_flg = SEM_UNDO;

        displayClock();
        cout << name << ": trying to get mutex on shared buffer" << "\n";

        semop(semid,&sem[2],1);
        semop(semid,&sem[0],1);
        
        char * sh = (char *) shmat(shmid, (void*)0, 0);
        memcpy(&q, sh, sizeof(q));
        displayClock();
        cout << name << ": placing " << price << " on shared buffer" << "\n";
        strcpy(q.name[q.current],name);
        q.price[q.current] = price;

        q.current++;
        
        memcpy(sh, &q, sizeof(q));
        shmdt(sh);

        sem[0].sem_op = 1;
        sem[1].sem_op = 1;
        semop(semid,&sem[0],1);
        semop(semid,&sem[1],1);
        displayClock();
        cout << name << ": sleeping for " << sleep << " ms" << "\n";
        this_thread::sleep_for(std::chrono::milliseconds(sleep));
    }

    return 0;
}