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
#include <bits/stdc++.h>

using namespace std;

struct shmq
{
    int maxsize = 0;
    int current = 0;
    char name[100][15];
    double price[100];
};


int getIndex(vector<string> v, string K)
{
	auto it = find(v.begin(), v.end(), K);

	// If element was found
	if (it != v.end())
	{
	
		// calculating the index
		// of K
		int index = it - v.begin();
		return index;
	}
	else {
		// If the element is not
		// present in the vector
		return -1;
	}
}


int main(int argc, char** argv)
{
    if (argc != 2)
    {
        return -1;
    }
    int buffer = stoi(argv[1]);

    vector<string> names;
    vector<vector<int>> preprices;

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
        sem[0].sem_op = -1;
        sem[0].sem_flg = SEM_UNDO;
        sem[1].sem_op = -1;
        sem[1].sem_flg = SEM_UNDO;
        semop(semid,&sem[1],1);
        semop(semid,&sem[0],1);

        char * sh = (char *) shmat(shmid, (void*)0, 0);
        memcpy(&q, sh, sizeof(q));
        
        int temp = getIndex(names,q.name[0]);
        if (temp == -1)
        {
            names.push_back(q.name[0]);
            int temp2 = getIndex(names,q.name[0]);
            vector<int> temp3;
            preprices.push_back(temp3);
            preprices[temp2].push_back(q.price[0]);
        }
        else
        {
            preprices[temp].push_back(q.price[0]);
            if (preprices[temp].size() > 6)
            {
                preprices[temp].erase(preprices[temp].begin());
            }
            
        }
        
        //Print here

        for (int j = 0; j < q.current; j++)
        {
            strcpy(q.name[j],q.name[j+1]);
            q.price[j] = q.price[j+1];
        }
        q.current--;
        
        memcpy(sh, &q, sizeof(q));
        shmdt(sh);

        sem[0].sem_op = 1;
        sem[2].sem_op = 1;
        semop(semid,&sem[0],1);
        semop(semid,&sem[2],1);
        
        

    }
    shmctl(shmid,IPC_RMID,NULL);
    return 0;
}