#include "shared.h"

int existsFreeTableOf(int cap,table *tables, int numTables){
	int i;
	for (i = 0; i < numTables; ++i)
	{
		if (!tables[i].status && tables[i].cap >= cap)
		{
			return i;
		}
	}
	return -1;
}

int main(int argc, char const *argv[])
{
	if (argc != 5)
	{
		printf("Wrong number of arguments.\n");
		printf("Use: ./doorman -d time -s shmid\n");

		return 1;
	}

	int i, shmid, maxServiceTime;
	for (i = 1; i < argc; ++i)
	{
		if (!strcmp(argv[i],"-s"))
		{
			shmid=atoi(argv[i + 1]);
		}else if (!strcmp(argv[i],"-d"))
		{
			maxServiceTime=atoi(argv[i + 1]);
		}
	}

	srand(time(NULL));
	int id, err, serviceTime=0;
	sharedStruct *sh;
	sh=(sharedStruct *)shmat(shmid, (void*)0, 0);
	if (sh == (void *)-1) perror("Doorman Attachment.");
	else printf("Doorman arrived in place, getting in position.\n");
	table *tables=(table *) &sh[1];
	int *bar=(int *) &tables[sh->numTables];
	printf("Our restaurant has the following tables:\n");
	for (i = 0; i < sh->numTables; ++i)
	{
		printf("Table %d with status %d and capacity %d\n", i, tables[i].status,tables[i].cap);
	}
	for (i = 0; i < sh->numTables; ++i)	
	{
		if (tables[i].cap > sh->biggestTableCap)
		{
			sh->biggestTableCap=tables[i].cap;
		}
	}
	while(1){
		serviceTime=rand() % maxServiceTime;
		sleep(serviceTime);		
		int value=0,position=-1,servicedBar=0,tableId=-1;
		sem_wait(&sh->tableMutex);
		if (existsFreeTable(tables, sh->numTables) && sh->barCurr)		
		{																
			for (i = sh->barStart; i < sh->barEnd; ++i)
			{
				if (!bar[i])
				{
					continue;
				}
				if ((tableId=existsFreeTableOf(bar[i],tables, sh->numTables)) >= 0)	
				{
					value=bar[i];
					position=i;
					servicedBar=1;
					break;
				}
			}
			if (value <= 2 && position >= 0)		
			{
				if (position == sh->barStart)
				{
					sh->barStart++;
				}
				bar[position]=0;
				printf("Moving company of %d in table %d of capacity %d.\n", value, tableId, tables[tableId].cap);
				sh->barCurr-=value;
				tables[tableId].status=1;
				sh->tableGiven=tableId;
				sem_post(&sh->barQueue2);
			}else if (value <= 4 && position >= 0)
			{
				if (position == sh->barStart)
				{
					sh->barStart++;
				}
				bar[position]=0;
				printf("Moving company of %d in table %d of capacity %d.\n", value, tableId, tables[tableId].cap);
				sh->barCurr-=value;
				tables[tableId].status=1;
				sh->tableGiven=tableId;
				sem_post(&sh->barQueue4);
			}else if (value <= 6 && position >= 0)
			{
				if (position == sh->barStart)
				{
					sh->barStart++;
				}
				bar[position]=0;
				printf("Moving company of %d in table %d of capacity %d.\n", value, tableId, tables[tableId].cap);
				sh->barCurr-=value;
				tables[tableId].status=1;
				sh->tableGiven=tableId;
				sem_post(&sh->barQueue6);
			}else if (value <= 8 && position >= 0)
			{
				if (position == sh->barStart)
				{
					sh->barStart++;
				}
				bar[position]=0;
				printf("Moving company of %d in table %d of capacity %d.\n", value, tableId, tables[tableId].cap);
				sh->barCurr-=value;
				tables[tableId].status=1;
				sh->tableGiven=tableId;
				sem_post(&sh->barQueue8);
			}else{
				sem_post(&sh->tableMutex);
			}
		}else{
			sem_post(&sh->tableMutex);
		}
		if (sh->queueCurr && !servicedBar)	
		{									
			sem_post(&sh->doorman);
			sem_wait(&sh->companySizeReported);
			if (sh->doorCompanySize > sh->biggestTableCap)		
			{
				printf("No big enough table for company of %d, Goodnight!\n", sh->doorCompanySize);
				sh->queueCurr--;
				sh->placedIn=0;
				sem_post(&sh->companyPlaced);
				continue;
			}
			int flagFoundTable=0;
			int tableId=-1;
			int flag=0;
			sem_wait(&sh->tableMutex);
			if(existsFreeTable(tables, sh->numTables)){
				for (i = 0; i < sh->numTables; ++i)
				{
					if (!tables[i].status && tables[i].cap >= sh->doorCompanySize)
					{
						flagFoundTable=1;
						tableId=i;
						printf("Moving company of %d to table %d, capacity %d.\n", sh->doorCompanySize, i, tables[i].cap);
						break;
					}
				}
				if (flagFoundTable)
				{
					tables[tableId].status=1;
					sh->tableGiven=tableId;
					sh->queueCurr--;
					flagFoundTable=0;
					sh->placedIn=2;
					sem_post(&sh->tableMutex);
				}else
				{
					sem_post(&sh->tableMutex);
					if (sh->doorCompanySize <= sh->barCap - sh->barCurr){
						
						bar[sh->barEnd]=sh->doorCompanySize;
						sh->barEnd++;
						sh->barCurr+=sh->doorCompanySize;
						sh->queueCurr--;
						sh->placedIn=1;
						printf("Moving company of %d to bar.\n",sh->doorCompanySize);
					}else{
						printf("Moving company of %d out, no space available.\n",sh->doorCompanySize);
						sh->queueCurr--;
						sh->placedIn=0;
					}
				}
				sem_post(&sh->companyPlaced);
			}else
			{
				sem_post(&sh->tableMutex);
				if (sh->doorCompanySize <= sh->barCap - sh->barCurr){
					bar[sh->barEnd]=sh->doorCompanySize;
					sh->barEnd++;
					sh->barCurr+=sh->doorCompanySize;
					sh->queueCurr--;
					sh->placedIn=1;
					printf("Moving company of %d to bar.\n",sh->doorCompanySize);
				}else{
					sh->queueCurr--;
					sh->placedIn=0;
					printf("Moving company of %d out, no space available.\n",sh->doorCompanySize);
				}
				sem_post(&sh->companyPlaced);
			}
		}
		if (!sh->barCurr && !sh->queueCurr)
		{
			sh->doormanFinished=1;
			break;
		}
	}
	err=shmdt((void *) sh); 
	if (err == -1) perror("Doorman Detachment.");
	else printf("Doorman shift is over.\n");

	return 0;
}