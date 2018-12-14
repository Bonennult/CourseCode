#include<windows.h>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#define NUM 100000
#define MAX_THREADS 1000
#define OFFSET 65536   //基址偏移需要为64K的整数倍

typedef struct _RANGE {
	DWORD threadID;
	int up;
	int down;
} RANGE, *PRANGE;

//global variables
HANDLE hFileMap;
//HANDLE hThread[MAX_THREADS];
DWORD dwFileSize;
int threadCount=0;

int quick_sort_kernel(int* a, int up, int down){
	int thresh = a[down];
	int i = up;
	int j = down;
	while(i>j){
		while(a[i]>=thresh && i>j)
			i--;
		a[j] = a[i];
		while(a[j]<=thresh && j<i)
			j++;
		a[i] = a[j];
	}
	a[j] = thresh;
	return j;
}

void quick_sort(int* a, int up, int down){
	int swap;
	if(up <= down)
		return ;
	swap = quick_sort_kernel(a, up, down);
	quick_sort(a, up, swap+1);
	quick_sort(a, swap-1, down);
}

void generate_random(char* fname){
	FILE* fp;
	int a[NUM];
	int i=0;
	srand(time(NULL));
	for(i=0;i<NUM;i++)                      //generate random data
		a[i] = rand();

	if((fp=fopen(fname,"wb"))==NULL){       //open file of test data
		printf("Cannot open file!");
		exit(1);
	}
	fseek(fp, 0,  SEEK_SET);                //reset file pointer
	fwrite(a, sizeof(int), NUM, fp);
	//for(i=0;i<NUM;i++)
		//fprintf(fp, "%d ",  a[i]);          //write data
	fclose(fp);
	return ;
}

void read_data(char* fname){
	FILE* fp;
	int a[NUM];
	int i=0;
	if((fp=fopen(fname,"rb"))==NULL){       //open file of test data
		printf("Cannot open file!");
		exit(1);
	}
	fseek(fp, 0,  SEEK_SET);
	fread(a, sizeof(int), NUM, fp);
	for(i=0;i<NUM;i++){
		printf("%7d ", a[i]);
		if((i+1)%10 == 0)
			printf("\n");
	}
	fclose(fp);
	return ;
}

void valid_data(int* a){
	int flag=1;
	int i=0;
	for(i=1;i<NUM;i++){
		if(a[i] < a[i-1]){
			flag = 0;
			printf("Sort error : %d\n", i);
			return;
		}
	}
	if(flag)
		printf("All sort right!\n");
	return;
}

DWORD WINAPI ThreadProc(LPVOID lpParam) {
	//DWORD dwThreadId[2];          //线程ID
	HANDLE hThread[2];              //线程句柄
	//DWORD dwFileOffsetLow;
	int up,down,*buffer;
	int swap;
	PRANGE pRange,pRange1,pRange2;
	pRange = (PRANGE)lpParam;
	up = pRange->up;
	down = pRange->down;
	pRange1 = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,sizeof(RANGE));
	pRange2 = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,sizeof(RANGE));

	buffer = MapViewOfFile(hFileMap,
		FILE_MAP_ALL_ACCESS,
		0,
		0,//sizeof(int)*down/OFFSET*OFFSET,    //必须是操作系统的分配粒度的整数倍，对于Windows操作系统，分配粒度固定为64KB
		dwFileSize);//sizeof(int)*(up-down+1));//

	if(up-down+1 < 1000){
		quick_sort(buffer, up, down);//up-sizeof(int)*down/OFFSET*OFFSET/sizeof(int), down-sizeof(int)*down/OFFSET*OFFSET/sizeof(int));
		printf("thread %8d | down: %8d | up: %8d\n", pRange->threadID, down, up);
	}
	else{
		swap = quick_sort_kernel(buffer, up, down);//up-sizeof(int)*down/OFFSET*OFFSET/sizeof(int), down-sizeof(int)*down/OFFSET*OFFSET/sizeof(int));
		pRange1->up = up;
		pRange1->down = swap+1;//+sizeof(int)*down/OFFSET*OFFSET/sizeof(int);
		pRange2->up = swap-1;//+sizeof(int)*down/OFFSET*OFFSET/sizeof(int);
		pRange2->down = down;
		hThread[0] = CreateThread(
			NULL,                 // default security attributes
			0,//4194304,                    // use default stack size  
			ThreadProc,           // thread function 
			pRange1,              // argument to thread function
			0,                    // use default creation flags
			&(pRange1->threadID));      // returns the thread identifier
		if (hThread[0] == NULL){
			ExitProcess(0);
			printf("Exit process. Err : %d", GetLastError());
		}
		hThread[1] = CreateThread( 
			NULL,                 // default security attributes
			0,//4194304,                    // use default stack size  
			ThreadProc,           // thread function 
			pRange2,              // argument to thread function
			0,                    // use default creation flags
			&(pRange2->threadID));      // returns the thread identifier
		if (hThread[1] == NULL){
			ExitProcess(1);
			printf("Exit process. Err : %d", GetLastError());
		}

		WaitForMultipleObjects(2, hThread, TRUE, INFINITE);
		
	}

	CloseHandle(hThread[0]);  // Close thread handles
	CloseHandle(hThread[1]);

	if (!UnmapViewOfFile(buffer)) {
		printf("Could not unmap view of file (%d).\n", GetLastError()); 
		return 0;
	}
	HeapFree(GetProcessHeap(), 0, pRange);
	return 0;
}

void main(){
	HANDLE hFile;
	char* fname="data";
	PRANGE pRange;
	int dwFileSizeHigh;
	int* array;
	int i;
	generate_random(fname);
	//read_data(fname);

	hFile = CreateFile(TEXT("data"),
		GENERIC_READ|GENERIC_WRITE,
		FILE_SHARE_READ|FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,           // normal file
		NULL);                           // no attr. template
	if (hFile == INVALID_HANDLE_VALUE){
		printf("Could not open file (error %d)\n", GetLastError()); 
		return;
	}
	dwFileSize = GetFileSize(hFile, &dwFileSizeHigh); 
	hFileMap = CreateFileMapping(hFile,
		NULL,                            // default security 
		PAGE_READWRITE,                  // readonly access
		0,                               // max. object size 
		dwFileSize,                      // buffer size  
		TEXT("DATA"));                   // name of mapping object
	if (hFileMap == NULL || hFileMap == INVALID_HANDLE_VALUE) { 
		printf("Could not create file mapping object (%d).\n", GetLastError()); 
		return;
	}

	pRange = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,sizeof(RANGE));
	pRange->threadID = 0;
	pRange->up = dwFileSize/sizeof(int)-1;   //-1 important!!
	pRange->down = 0;
	ThreadProc(pRange);

	//Sleep(10000);
	//WaitForMultipleObjects(MAX_THREADS, hThread, TRUE, INFINITE);
	//for(i=0; i<threadCount; i++)
		//CloseHandle(hThread[i]);

	//read_data(fname);
	array = MapViewOfFile(hFileMap, 
		FILE_MAP_READ,
		0,
		0,
		dwFileSize);
	valid_data(array);
	if (!UnmapViewOfFile(array)) { 
		printf("Could not unmap view of file (%d).\n", GetLastError()); 
		return;
	}
	printf("All work done!\n");
	return;
}
