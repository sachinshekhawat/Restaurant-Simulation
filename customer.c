#include "shared.h"
int main(int argc, char const *argv[])
{
	if (argc != 7)
	{
		printf("Wrong number of arguments.\n");
		printf("Use: ./costumer -n people -d period -s shmid\n");

		return 1;
	}

	int i, shmid, maxEatingTime, numPersons;
	srand(time(NULL));
	for (i = 1; i < argc; ++i)
	{
		if (!strcmp(argv[i],"-s"))
		{
			shmid=atoi(argv[i + 1]);
		}else if (!strcmp(argv[i],"-d"))
		{
			maxEatingTime=atoi(argv[i + 1]);
		}else if (!strcmp(argv[i],"-n"))
		{
			numPersons=atoi(argv[i + 1]);
		}
	}
	int id, err, eatingTime=0;
	srand(getpid());
	eatingTime=rand() % maxEatingTime + 1;
	sharedStruct *sh;
	sh=(sharedStruct *)shmat(shmid, (void*)0, 0);
	if (sh == (void *)-1) perror("Customer Attachment.");
	table *tables=(table *) &sh[1];
	int *bar=(int *) &tables[sh->numTables];
	int *waiters=(int *) &bar[sh->barTableSize];
	sem_wait(&sh->queue);
	sem_wait(&sh->doorman);
	printf("Doorman is back in door to serve next company.\n");
	clock_t start,end;
	double cpu_time_used;
	start = clock();
	printf("Company of %d persons coming inside.\n", numPersons);
	sh->doorCompanySize=numPersons;
	sh->sumPeople+=numPersons;
	sem_post(&sh->companySizeReported);
	sem_wait(&sh->companyPlaced);
	int tableSat,waiterId;
	printf("\n");
	for(int i=0;i<sh->numTables;i++)
	{
		if(!tables[i].status)
		{
			printf("Table %d is available\n",i);
		}
	}
	printf("\n");
	if (sh->placedIn == 1)
	{
		printf("Company of %d placed in bar.\n", numPersons);
		printf("Total people in bar %d.\n", sh->barCurr);
		sh->placedIn=0;
		sem_post(&sh->queue);
		if (numPersons <= 2)
		{
			sem_wait(&sh->barQueue2);	
		}else if (numPersons <= 4)
		{
			sem_wait(&sh->barQueue4);
		}else if (numPersons <= 6)
		{
			sem_wait(&sh->barQueue6);
		}else if (numPersons <= 8)
		{
			sem_wait(&sh->barQueue8);
		}else{
			printf("Not found\n");
		}
		int r=(rand() % 2);
		if (r == 1)
		{
			tableSat=sh->tableGiven;
			tables[tableSat].status=0;
			printf("Company of %d left bar FURIOUS about their waiting time.\n", numPersons);
			printf("Total people in bar %d.\n", sh->barCurr);
			sem_post(&sh->tableMutex);
		}else{
			tableSat=sh->tableGiven;
			tables[tableSat].status=1;
			printf("Moved company of %d from bar to table %d, capacity %d.\n", numPersons, tableSat, tables[tableSat].cap);
			printf("Total people in bar %d.\n", sh->barCurr);
			for(int i=0;i<sh->numTables;i++)
			{
				if(!tables[i].status)
				{
					printf("\nTable %d is available\n",i);
				}
			}
			sem_post(&sh->tableMutex);
			sem_wait(&sh->waiterMutex);
			sh->orderStatus=0;
			sem_post(&sh->waiterQueue);
			sem_wait(&sh->waiterResponse);
			waiterId=sh->chosenWaiterId;
			sem_post(&sh->waiterMutex);
			printf("Company of %d served by waiter %d\n", numPersons, waiterId);
			sleep(eatingTime);
			while(1){
				sem_wait(&sh->waiterMutex);
				sh->orderStatus=1;
				sh->chosenWaiterId=waiterId;
				sem_post(&sh->waiterQueue);
				sem_wait(&sh->waiterResponse);
				if (sh->rightWaiterFound)
				{
					printf("Company of %d paid Rs. %d to waiter %d.\n", numPersons, sh->orderMoney, waiterId);
					sem_post(&sh->waiterMutex);
					break;
				}
				sem_post(&sh->waiterMutex);
			}

			sem_wait(&sh->tableMutex);
			tables[tableSat].status=0;
			printf("Company of %d finished lunch and left table %d, capacity %d.\n", numPersons, tableSat, tables[tableSat].cap);
			sem_post(&sh->tableMutex);
		}
	}else if(sh->placedIn == 2)
	{
		tableSat=sh->tableGiven;
		sem_post(&sh->queue);
		sem_wait(&sh->waiterMutex);
		sh->orderStatus=0;
		sem_post(&sh->waiterQueue);
		sem_wait(&sh->waiterResponse);
		waiterId=sh->chosenWaiterId;
		sem_post(&sh->waiterMutex);
		printf("Company of %d served by waiter %d.\n", numPersons, waiterId);
		sleep(eatingTime);
		while(1){
			sem_wait(&sh->waiterMutex);
			sh->orderStatus=1;
			sh->chosenWaiterId=waiterId;
			sem_post(&sh->waiterQueue);
			sem_wait(&sh->waiterResponse);
			if (sh->rightWaiterFound)
			{
				printf("Company of %d paid Rs. %d to waiter %d.\n", numPersons, sh->orderMoney, waiterId);
				sem_post(&sh->waiterMutex);
				break;
			}
			sem_post(&sh->waiterMutex);
		}

		sem_wait(&sh->tableMutex);
		tables[tableSat].status=0;
		printf("Company of %d finished lunch and left table %d, capacity %d.\n", numPersons, tableSat, tables[tableSat].cap);
		end = clock();
		cpu_time_used = ((double)(end-start))/ CLOCKS_PER_SEC * 10000;
		printf("Time take by company of %d customers is %f ms\n",numPersons,cpu_time_used);
		printf("\n");
		for(int i=0;i<sh->numTables;i++)
		{
			if(!tables[i].status)
			{
				printf("Table %d is available\n",i);
			}
		}
		printf("\n");
		sem_post(&sh->tableMutex);
	}else{
		sem_post(&sh->queue);
	}
	sem_wait(&sh->tableMutex);
	if ((existsFreeTable(tables, sh->numTables) == sh->numTables) && sh->doormanFinished)
	{
		sem_post(&sh->waiterQueue);
	}
	sem_post(&sh->tableMutex);
	err=shmdt((void *) sh);
	if (err == -1) perror("Costumer Detachment.");
	else 
	{
		printf("Company of %d left restaurant.\n", numPersons);
	}
	return 0;
}
