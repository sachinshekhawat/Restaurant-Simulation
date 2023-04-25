#include "shared.h"
typedef struct
{
	int numTables;
	int maxCapBar;
	int maxNumWaiters;
	int customerPeriod;
	int waiterPeriod;
	int doormanTime;
	int maxMoney;
} configStruct;

int main(int argc, char const *argv[])
{
	if (argc != 5)
	{
		printf("Wrong number of arguments.\n");
		printf("Use: ./restaurant -n customers -l configfile\n");

		return 1;
	}

	int i, numCustomers, statisticsTime;
	FILE *fp;
	configStruct config;
	for (i = 1; i < argc; ++i)
	{
		if (!strcmp(argv[i],"-n"))
		{
			numCustomers = atoi(argv[i + 1]);
		}else if (!strcmp(argv[i],"-l"))
		{
			fp=fopen(argv[i + 1],"r");
			if (fp == NULL)
			{
				printf("Unable to open config file.\n");
				return 1;
			}
			int j=0, attrNum;
			char attr [50];
			while (fscanf(fp,"%s %d",attr,&attrNum) != EOF)
			{
				if (j == 0)
				{
					config.numTables=attrNum;
				}else if (j == 1)
				{
					config.maxMoney=attrNum;
				}else if (j == 2)
				{
					config.maxCapBar=attrNum;
				}else if (j == 3)
				{
					config.maxNumWaiters=attrNum;
				}else if (j == 4)
				{
					config.customerPeriod=attrNum;
				}else if (j == 5)
				{
					config.waiterPeriod=attrNum;
				}else if(j == 6){
					config.doormanTime=attrNum;
				}else{
					config.maxMoney=attrNum;
				}
				j++;
			}
			fclose(fp);
		}
	}

	int id=0 ,err=0, size=-1;
	pid_t pid;
	sharedStruct *sh;
	size_t sizea = sizeof(table)*config.numTables + sizeof(int)*numCustomers + sizeof(int)*config.maxNumWaiters;
	id=shmget(IPC_PRIVATE, (sizeof(sharedStruct) + sizeof(table)*config.numTables), 0666); 
	if (id == -1) perror(" Creation ");
	else printf("Allocated. %d\n", (int)id);
	sh=(sharedStruct *)shmat(id, (void *)0, 0); 
	if (sh == (void *)-1) perror("Attachment.");
	table *tables=(table *) &sh[1];	
	int *bar=(int *) &tables[config.numTables];	
	int *waiters=(int *) &bar[numCustomers];	
	sh->barStart=0;							
	sh->barEnd=0;
	sh->numTables=config.numTables;
	sh->barCap=config.maxCapBar;
	sh->barCurr=0;
	sh->doorCompanySize=0;
	sh->queueCurr=numCustomers;
	sh->placedIn=-1;											
	sh->biggestTableCap=0;
	sh->tableGiven=-1;
	sh->barTableSize=numCustomers;
	sh->waitersNum=config.maxNumWaiters;
	sh->chosenWaiterId=-1;
	sh->doormanFinished=0;
	sh->orderStatus=-1;
	sh->rightWaiterFound=0;
	sh->sumMoney=0;
	sh->orderMoney=0;
	sh->sumPeople=0;
	int semInit;
	semInit = sem_init(&sh->queue, 1, 1);		
	if (semInit) {
		printf("Could not init semaphore mutex\n");
		shmctl(id, IPC_RMID, 0);
		return 1;
	}
	semInit = sem_init(&sh->doorman, 1, 0);			
	if (semInit) {
		printf("Could not init semaphore mutex\n");
		shmctl(id, IPC_RMID, 0);
		return 1;
	}
	semInit = sem_init(&sh->companySizeReported, 1, 0);			
	if (semInit) {
		printf("Could not init semaphore mutex\n");
		shmctl(id, IPC_RMID, 0);
		return 1;
	}
	semInit = sem_init(&sh->companyPlaced, 1, 0);			
	if (semInit) {
		printf("Could not init semaphore mutex\n");
		shmctl(id, IPC_RMID, 0);
		return 1;
	}
	semInit = sem_init(&sh->tableMutex, 1, 1);			
	if (semInit) {
		printf("Could not init semaphore mutex\n");
		shmctl(id, IPC_RMID, 0);
		return 1;
	}
	semInit = sem_init(&sh->barQueue2, 1, 0);			
	if (semInit) {
		printf("Could not init semaphore mutex\n");
		shmctl(id, IPC_RMID, 0);
		return 1;
	}
	semInit = sem_init(&sh->barQueue4, 1, 0);			
	if (semInit) {
		printf("Could not init semaphore mutex\n");
		shmctl(id, IPC_RMID, 0);
		return 1;
	}
	semInit = sem_init(&sh->barQueue6, 1, 0);			
	if (semInit) {
		printf("Could not init semaphore mutex\n");
		shmctl(id, IPC_RMID, 0);
		return 1;
	}
	semInit = sem_init(&sh->barQueue8, 1, 0);			
	if (semInit) {
		printf("Could not init semaphore mutex\n");
		shmctl(id, IPC_RMID, 0);
		return 1;
	}
	semInit = sem_init(&sh->waiterMutex, 1, 1);			
	if (semInit) {
		printf("Could not init semaphore mutex\n");
		shmctl(id, IPC_RMID, 0);
		return 1;
	}
	semInit = sem_init(&sh->waiterQueue, 1, 0);			
	if (semInit) {
		printf("Could not init semaphore mutex\n");
		shmctl(id, IPC_RMID, 0);
		return 1;
	}
	semInit = sem_init(&sh->waiterResponse, 1, 0);			
	if (semInit) {
		printf("Could not init semaphore mutex\n");
		shmctl(id, IPC_RMID, 0);
		return 1;
	}


	srand(time(NULL));
	for (i = 0; i < config.numTables; ++i)
	{
		tables[i].status=0;
		tables[i].cap= (rand() % 4 + 1) * 2;
	}
	for (i = 0; i < sh->barCap; ++i)
	{
		bar[i] = 0;
	}
	for (i = 0; i < config.maxNumWaiters; ++i)
	{
		waiters[i] = -3;
	}
	printf("Restaurant set n ready.\n");
	printf("Start other process.>\n");
	pid_t did;
	did = fork();
	if(did < 0){
		perror("fork");
		shmctl(id, IPC_RMID, 0);
		return 1;
	}else if (did == 0)
	{
		char dtime[20],dshid[20];
		sprintf(dtime,"%d",config.doormanTime);
		sprintf(dshid,"%d",id);
		if (execlp("./doorman","doorman","-d",dtime,"-s",dshid, (char*)NULL) == -1)
		{
			perror("execlp failed");
			shmctl(id, IPC_RMID, 0);
			return 1;
		}
	}
	pid_t wid;
	for (i = 0; i < config.maxNumWaiters; ++i)
	{
		wid=fork();
		if (wid < 0)
		{
			perror("fork");
			shmctl(id, IPC_RMID, 0);
			return 1;
		}else if (wid == 0)
		{
			char wtime[20],wshid[20],moneyAmount[20],waiterPosition[20];
			sprintf(wtime,"%d", config.waiterPeriod);
			sprintf(moneyAmount,"%d", config.maxMoney);
			sprintf(wshid,"%d",id);
			sprintf(waiterPosition,"%d",i);
			if (execlp("./waiter","waiter","-d",wtime,"-m",moneyAmount,"-s",wshid,"-p",waiterPosition, (char*)NULL) == -1)
			{
				perror("execlp failed");
				shmctl(id, IPC_RMID, 0);
				return 1;
			}
		}
	}
	pid_t cid;
	for (i = 0; i < numCustomers; ++i)
	{
		cid=fork();
		if (cid < 0)
		{
			perror("fork");
			shmctl(id, IPC_RMID, 0);
			return 1;
		}else if (cid == 0)
		{
			srand(getpid());
			char maxCustomerPeriod[20],cshid[20],maxPeople[20];
			sprintf(maxCustomerPeriod,"%d", config.customerPeriod);
			sprintf(maxPeople,"%d", rand() % 8 + 1);
			sprintf(cshid,"%d",id);
			if (execlp("./customer","customer","-n",maxPeople,"-d",maxCustomerPeriod,"-s",cshid, (char*)NULL) == -1)
			{
				perror("execlp failed");
				shmctl(id, IPC_RMID, 0);
				return 1;
			}
		}
	}
	wait(NULL);	
	for (i = 0; i < config.maxNumWaiters; ++i)
	{
		wait(NULL);
	}
	for (i = 0; i < numCustomers; ++i)
	{
		wait(NULL);
	}
	printf("Total amount of people serviced %d.\n", sh->sumPeople);
	printf("Total amount of money earned is Rs. %d.\n", sh->sumMoney);
	err=shmctl(id, IPC_RMID, 0) ;
	if (err == -1) perror("Removal.");
	else printf("Restaurant closed, Hope to see you again.\n") ;

	return 0;
}