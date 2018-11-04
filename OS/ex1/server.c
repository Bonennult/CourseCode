#define _REENTRANT
#include <stdio.h>
#include <malloc.h>
#include <windows.h>
#include <pthread.h>
#include <semaphore.h>
#pragma comment(lib, "E:\\Study\\OS\\HW\\pthreads-w32-2-9-1-release\\Pre-built.2\\lib\\x86\\pthreadVC2.lib")

//#define max(a,b) ( ((a)>(b)) ? (a):(b) )
#define SERV_NUM 5
#define CUST_NUM 20

typedef struct Code_Info{
	int serv_code;
	int customer_id;
	struct Code_Info* next;
	//int serv_time;
} code_info;

typedef struct{
	int customer_id;
	//int server_id;
	int entering_time;
	int serv_time;
	int serv_start_time;
	//int serv_end_time;
} customer_info;

pthread_t serv_thread_id[SERV_NUM];       //array of server thread IDs
pthread_t cust_thread_id[CUST_NUM];       //array of customer thread IDs
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;    //exclusively access the code info
//pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;    //exclusively access the customer info
sem_t serv_waiting;
sem_t cust_waiting;
int current_code = 1000;
//int current_time = 0;
int last_service_end_time[SERV_NUM] = {0};  //record the last service end time of each server
customer_info customer[CUST_NUM];         //record info of all customers
code_info* head = NULL;
code_info* tail = NULL;

void *SERVER(void* arg){
	int serv_id = (int)arg;
	int cust_id;
	//int customer_id;
	//int serv_time;
	code_info* temp;
	while(1){
		sem_wait(&cust_waiting);      //see if there is customer waiting

		pthread_mutex_lock(&mutex);   //try access code
		temp = head;
		head = head->next;
		cust_id = temp->customer_id;
		//current_time = current_time + temp->serv_time;   //refresh current time
		customer[cust_id].serv_start_time = max(last_service_end_time[serv_id], customer[cust_id].entering_time);
		last_service_end_time[serv_id] = customer[cust_id].serv_start_time + customer[cust_id].serv_time;
		//customer[cust_id].serv_end_time = last_service_end_time[serv_id];
		pthread_mutex_unlock(&mutex);
		
		//printf("server %d calls No.%d\n", server_id, temp->serv_code);
		//printf("server %d is serving customer %d ...\n", server_id, temp->customer_id);
		//Sleep((customer[cust_id].serv_time)*100);
		printf("***************\ncustomer ID        : %d\nserver ID          : %d\nservice code       : %d\nentering time      : %d\nservice start time : %d\nleaving time       : %d\n***************\n\n", customer[cust_id].customer_id, serv_id, temp->serv_code, customer[serv_id].entering_time, customer[serv_id].serv_start_time, customer[serv_id].serv_start_time + customer[serv_id].serv_time);
		free(temp);
		sem_post(&serv_waiting);
	}
}

void *CUSTOMER(void* arg){
	//customer_info* temp1 = (customer_info*)arg;
	int customer_id = (int)arg;
	code_info* temp2;
	
	pthread_mutex_lock(&mutex);
	//current_time = temp1->entering_time;        //refresh current time
	if(head == NULL){                           //the customer get his code
		head = (code_info*)malloc(sizeof(code_info));
		tail = head;
		head->next = NULL;
	}
	else{
		temp2 = (code_info*)malloc(sizeof(code_info));
		tail->next = temp2;
		tail = tail->next;
	}
	tail->customer_id = customer_id;
	tail->serv_code = current_code;
	//tail->serv_time = temp1->serv_time;
	current_code++;
	pthread_mutex_unlock(&mutex);
	
	sem_post(&cust_waiting);
	sem_wait(&serv_waiting);
}

void main(){
	int i = 0;
	int id, time1, time2;
	int count = 0;
	FILE* fp;
	sem_init(&serv_waiting, 0, SERV_NUM);
	sem_init(&cust_waiting, 0, 0);
	if((fp=fopen("test.txt","r"))==NULL){
		printf("Cannot open file!");
		exit(1);
	}
	fseek(fp, 0,  SEEK_SET);      //reset file pointer
	while(fscanf(fp, "%d %d %d", &id, &time1, &time2) != -1){
		customer[id-1].customer_id = id-1;
		customer[id-1].entering_time = time1;
		customer[id-1].serv_time = time2;
		i++;
	}
	count = i;
	fclose(fp);

	for(i=0; i<SERV_NUM; i++)
		pthread_create(&serv_thread_id[i], NULL, SERVER, (void *)i);
	for(i=0; i<count; i++)
		pthread_create(&cust_thread_id[i], NULL, CUSTOMER, (void*)(customer[i].customer_id));
	for(i=0; i<count; i++)                     //wait for all customer threads end
		pthread_join(cust_thread_id[i], NULL);
	for(i=0; i<SERV_NUM; i++)
		pthread_cancel(serv_thread_id[i]);
	printf("All work done!\n");
	pthread_mutex_destroy(&mutex);
	sem_destroy(&serv_waiting);
	sem_destroy(&cust_waiting);
	return;
}
