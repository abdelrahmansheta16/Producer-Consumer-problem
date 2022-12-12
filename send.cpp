#include<iostream>
#include<sys/shm.h>
#include<string.h>
using namespace std;

struct sharedMemory
{
    char name[100];
    double mean;
};

int main()
{
int i;
sharedMemory *shared_memory;
char buff[100];
int shmid;
shmid=shmget((key_t)2345, 1024, 0666|IPC_CREAT); //creates shared memory segment with key 2345, having size 1024 bytes. IPC_CREAT is used to create the shared segment if it does not exist. 0666 are the permisions on the shared segment
printf("Key of shared memory is %d\n",shmid);
shared_memory=(sharedMemory *)shmat(shmid,NULL,0); //process attached to shared memory segment
printf("Process attached at %p\n",shared_memory); //this prints the address where the segment is attached with this process
printf("Enter some data to write to shared memory\n");
shared_memory->mean = 0.23;
strcpy(shared_memory->name,"hello");
cout << "You wrote : " << shared_memory->name;
// printf("You wrote : %s\n",shared_memory->name);
}