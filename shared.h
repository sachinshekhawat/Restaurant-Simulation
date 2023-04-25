#ifndef SHSTRUCT
#define SHSTRUCT

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include <semaphore.h>

typedef struct
{
	int status;
	int cap;
}table;

int existsFreeTable(table *tables, int numTables){
	int i,counter=0;
	for (i = 0; i < numTables; ++i)
	{
		if (!tables[i].status)
		{
			counter++;
		}
	}
	return counter;
}


typedef struct
{
	int barStart;
	int barEnd;
	int numTables;
	int barCap;
	int barCurr;
	int doorCompanySize;
	int queueCurr;
	int placedIn;
	int biggestTableCap;
	int tableGiven;
	int barTableSize;
	int waitersNum;
	int chosenWaiterId;
	int doormanFinished;
	int orderStatus;
	int rightWaiterFound;
	int sumMoney;
	int orderMoney;
	int sumPeople;

	sem_t queue;
	sem_t doorman;
	sem_t companySizeReported;
	sem_t companyPlaced;
	sem_t tableMutex;
	sem_t barQueue2;
	sem_t barQueue4;
	sem_t barQueue6;
	sem_t barQueue8;
	sem_t waiterMutex;
	sem_t waiterQueue;
	sem_t waiterResponse;
} sharedStruct;

#endif