#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>

#define ACTION_REGULATOR 7 //each action is bounded with this time

//function prototypes
void pickup_forks(int pid);

void* thread_entry_point(void* pid);

pthread_mutex_t* chopsticks; //chopstick mutexes
int num_phil;

int main(int argc, char** argv){
	//handle invalid input to program
	if (argc<=1) {
		printf("INVALID params!! Run something like '%s 20' \n",argv[0]);
		exit(1);
	}else if(atoi(argv[1])<=0){
		printf("Please provide number of philosopher greater than 0\n");
		exit(1);
	}
	
	num_phil = atoi(argv[1]);
	//allocate memory and initialise n number of chopstick mutexes	
	chopsticks = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t)*num_phil);
	for (int i = 0; i < num_phil; ++i)
		pthread_mutex_init(&chopsticks[i],NULL);
	
	//allocate memory and initalise n number of philosopher threads
	pthread_t* phils = (pthread_t*)malloc(sizeof(pthread_t)*num_phil);
	for (int i = 0; i < num_phil; ++i)
	{
		int* phil_id = (int*)malloc(sizeof(int));
		*phil_id = i; 
		pthread_create(&phils[i],NULL,thread_entry_point,(void*)phil_id);
	}

	for (int i = 0; i < num_phil; ++i)
		pthread_join(phils[i],NULL);

	return 0;
}



void pickup_forks(int pid){
	int left = pid;
	int right= pid-1;
	if (pid==0)
		right = num_phil-1;
	pthread_mutex_lock(&chopsticks[left]); //left chopstick
	
	pthread_mutex_lock(&chopsticks[right]); //right chopstick
	printf("Philosopher %d waiting...\n",pid);
}

void return_forks(int pid){
	int left = pid;
	int right= pid-1;
	if (pid==0)
		right = num_phil-1;
	pthread_mutex_unlock(&chopsticks[left]);  //left
	pthread_mutex_unlock(&chopsticks[right]); //right

}

void* thread_entry_point(void* pid){
	//seed random number
	struct timeval start; 
	gettimeofday(&start, NULL);
	srand(start.tv_usec);  //microsecond precision

	int phil_id = *((int *) pid);
	while (1) {
		printf("philosopher %d is thinking...\n",phil_id);
		int think = rand()%ACTION_REGULATOR;
		sleep(think);
		pickup_forks(phil_id);			
		printf("philosopher %d is eating...\n", phil_id);
		int eat = rand()%ACTION_REGULATOR;
		sleep(eat);
		return_forks(phil_id);
		printf("Philosopher %d has put down chopstick....\n", phil_id);		
	}
	return NULL;
}