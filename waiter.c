#include "shared.h"

int main(int argc, char const *argv[])
{
	if (argc != 9)
	{
		printf("Wrong number of arguments.\n");
		printf("Use: ./waiter -d period -m moneyamount -s shmid -p waiterPosition\n");

		return 1;
	}

	int i, shmid, maxServiceTime, maxMoneyAmount, waiterPosition;
	srand(getpid());
	for (i = 1; i < argc; ++i)
	{
		if (!strcmp(argv[i],"-s"))
		{
			shmid=atoi(argv[i + 1]);
		}else if (!strcmp(argv[i],"-d"))
		{
			maxServiceTime=atoi(argv[i + 1]);
		}else if (!strcmp(argv[i],"-m"))
		{
			maxMoneyAmount=atoi(argv[i + 1]);
		}else if (!strcmp(argv[i],"-p"))
		{
			waiterPosition=atoi(argv[i + 1]);
		}
	}
	int id, err, money, serviceTime;
	sharedStruct *sh;
	sh = (sharedStruct *)shmat(shmid, (void*)0, 0);
	if (sh == (void *)-1)
		perror("Waiter Attachment.");
	else
		printf("Waiter %d arrived in place, getting in position.\n", waiterPosition);
	table *tables=(table *) &sh[1];
	int *bar=(int *) &tables[sh->numTables];
	int *waiters=(int *) &bar[sh->barTableSize];
	while(1){
		sem_wait(&sh->waiterQueue);
		sh->chosenWaiterId=waiterPosition;
		if (sh->orderStatus == 1)
		{
			if (sh->chosenWaiterId == waiterPosition)
			{
				money=rand() % maxMoneyAmount + 1;
				sh->sumMoney+=money;
				sh->orderMoney=money;
				sh->rightWaiterFound=1;
				printf("Paying to waiter %d, cost Rs. %d.\n", waiterPosition, money);
				serviceTime=rand() % maxServiceTime + 1;
				sleep(serviceTime);
			}
			sh->orderStatus = -1;
			sem_post(&sh->waiterResponse);
		}else if (!sh->orderStatus)
		{
			sem_post(&sh->waiterResponse);
			serviceTime=rand() % maxServiceTime + 1;
			sleep(serviceTime);
		}else{
			sem_post(&sh->waiterResponse);
		}
		if ((existsFreeTable(tables, sh->numTables) == sh->numTables) && sh->doormanFinished)
		{
			sem_post(&sh->waiterQueue);
			break;
		}
	}
	err=shmdt((void *) sh);
	if (err == -1) perror("Waiter Detachment.");
	else printf("No more people for waiter %d to serve.\n", waiterPosition);
	return 0;
}