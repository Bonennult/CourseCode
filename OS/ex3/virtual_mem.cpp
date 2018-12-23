#define _REENTRANT
#include <cstdio>
#include <iostream>
#include <iomanip>
#include <windows.h>
using namespace std;

#define dwSize 4*1024
HANDLE hSemaphoreAllo;
HANDLE hSemaphoreTrac;
LPVOID baseAddress;
SYSTEM_INFO systemInfo;
MEMORY_BASIC_INFORMATION memoryInfo;

void getSysInfo(){
	GetSystemInfo(&systemInfo);
	cout<<setw(30) << "Page Size: " <<systemInfo.dwPageSize<<endl
		<<setw(30) << "Allocation Granularity: " <<systemInfo.dwAllocationGranularity<<endl
		<<setw(30) << "Maximum Application Address: " <<systemInfo.lpMaximumApplicationAddress<<endl
		<<setw(30) << "Minimum Application Address: " <<systemInfo.lpMinimumApplicationAddress<<endl
		<<setw(30) << "Active Processor Mask: " <<systemInfo.dwActiveProcessorMask<<endl
		<<setw(30) << "Number Of Processors: " <<systemInfo.dwNumberOfProcessors<<endl
		<<setw(30) << "Processor Type: " <<systemInfo.dwProcessorType<<endl
		<<setw(30) << "Processor Level: " <<systemInfo.wProcessorLevel<<endl
		<<setw(30) << "Processor Revision: " <<systemInfo.wProcessorRevision<<endl<<endl;
	return ;
}

void getMemInfo(){
	DWORD dwResult;
	dwResult = VirtualQuery(baseAddress,
		&memoryInfo,
		sizeof(MEMORY_BASIC_INFORMATION));

	cout<<setw(25)<<"Mmemory base address: "<<memoryInfo.BaseAddress<<endl;
	cout<<setw(25)<<"Allocation base: "<<memoryInfo.AllocationBase<<endl;
	cout<<setw(25)<<"Region Size: "<<memoryInfo.RegionSize<<endl;
	cout<<setw(25)<<"State: ";
	switch(memoryInfo.State){
		case MEM_COMMIT:  cout<<"Commit"<<endl;  break;
		case MEM_RESERVE:  cout<<"Reserve"<<endl;  break;
		case MEM_FREE:  cout<<"Free"<<endl;  break;
		default:  cout<<"Others"<<endl;  break;
	}
	cout<<setw(25)<<"Type: ";
	switch(memoryInfo.Type){
		case MEM_IMAGE:  cout<<"Image"<<endl;  break;
		case MEM_MAPPED:  cout<<"Mapped"<<endl;  break;
		case MEM_PRIVATE:  cout<<"Private"<<endl;  break;
		default:  cout<<"Others"<<endl;  break;
	}
	cout<<setw(25)<<"Allocation Protect: ";
	switch(memoryInfo.AllocationProtect){
		case PAGE_READONLY:  cout<<"_R_";  break;
		case PAGE_READWRITE:  cout<<"_RW_";  break;
		case PAGE_EXECUTE:  cout<<"_E_";  break;
		case PAGE_EXECUTE_READWRITE:  cout<<"_ERW_";  break;
		case PAGE_EXECUTE_READ:  cout<<"_ER_";  break;
		case PAGE_EXECUTE_WRITECOPY:  cout<<"_EWC_";  break;
		default:  cout<<"Others";  break;
	}
	cout<<"(0x"<<hex<<memoryInfo.Protect<<")"<<endl<<endl;
}

DWORD WINAPI Allocator(LPVOID lpParam){
	bool lock;
	bool free;
	WaitForSingleObject(hSemaphoreAllo, INFINITE);
	baseAddress = VirtualAlloc(NULL,                 // reserve 4K memory
		dwSize,
		MEM_RESERVE,
		PAGE_READWRITE);
	printf("---After VirtualAlloc(REASERVE):---\n");
	ReleaseSemaphore(hSemaphoreTrac, 1, NULL);
	WaitForSingleObject(hSemaphoreAllo,INFINITE);
	baseAddress = VirtualAlloc(baseAddress,          // commit 4K memory
		dwSize,
		MEM_COMMIT,
		PAGE_READWRITE);
	printf("\n---After VirtualAlloc(COMMIT):---\n");
	ReleaseSemaphore(hSemaphoreTrac, 1, NULL);
	WaitForSingleObject(hSemaphoreAllo,INFINITE);
	lock = VirtualLock(baseAddress,                  // lock 4K memory
		dwSize);
	printf("\n---After VirtualLock:---\n");
	if(!lock){
		printf("Lock error: %d\n", GetLastError());
	}
	ReleaseSemaphore(hSemaphoreTrac, 1, NULL);
	WaitForSingleObject(hSemaphoreAllo,INFINITE);
	lock = VirtualUnlock(baseAddress,                // unlock 4K memory
		dwSize);
	printf("\n---After VirtualUnlock:---\n");
	if(!lock){
		printf("Unlock error: %d\n", GetLastError());
	}
	ReleaseSemaphore(hSemaphoreTrac, 1, NULL);
	WaitForSingleObject(hSemaphoreAllo,INFINITE);
	free = VirtualFree(baseAddress,                  // decommit 4K memory
		dwSize,
		MEM_DECOMMIT);
	printf("\n---After VirtualFree(DECOMMIT):---\n");
	if(!free){
		printf("Decommit error: %d\n", GetLastError());
	}
	ReleaseSemaphore(hSemaphoreTrac, 1, NULL);
	WaitForSingleObject(hSemaphoreAllo,INFINITE);
	free = VirtualFree(baseAddress,                  // release 4K memory
		0,
		MEM_RELEASE);
	printf("\n---After VirtualFree(RELEASE):---\n");
	if(!free){
		printf("Release error: %d\n", GetLastError());
	}
	ReleaseSemaphore(hSemaphoreTrac, 1, NULL);
	return 0;
}

DWORD WINAPI Tracker(LPVOID lpParam){
	printf("\n---System Information:---\n");
	getSysInfo();
	ReleaseSemaphore(hSemaphoreAllo, 1, NULL);
	for(int i=0; i<6; i++){
		WaitForSingleObject(hSemaphoreTrac,INFINITE);
		getMemInfo();
		ReleaseSemaphore(hSemaphoreAllo, 1, NULL);
	}
	return 0;
}

void main(){
	HANDLE hThread[2];
	DWORD threadID[2];

	hSemaphoreAllo = CreateSemaphore(NULL, 0, 1, "allo");
	hSemaphoreTrac = CreateSemaphore(NULL, 0, 1, "trac");

	hThread[0] = CreateThread(
		NULL,                 // default security attributes
		0,                    // use default stack size
		Tracker,              // thread function 
		NULL,                 // argument to thread function
		0,                    // use default creation flags
		&threadID[0]);        // returns the thread identifier
	if (hThread[0] == NULL){
		ExitProcess(0);
		printf("Exit process. Err : %d", GetLastError());
	}
	hThread[1] = CreateThread(
		NULL,                 // default security attributes
		0,                    // use default stack size
		Allocator,            // thread function 
		NULL,                 // argument to thread function
		0,                    // use default creation flags
		&threadID[1]);        // returns the thread identifier
	if (hThread[1] == NULL){
		ExitProcess(0);
		printf("Exit process. Err : %d", GetLastError());
	}

	WaitForMultipleObjects(2, hThread, TRUE, INFINITE);
	CloseHandle(hThread);
	CloseHandle(hSemaphoreAllo);
	CloseHandle(hSemaphoreTrac);
	return ;
}
