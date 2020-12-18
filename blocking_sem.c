/*  
    Name: Ananya Lohani
    Roll Number: 2019018
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

typedef struct my_semaphore
{
    // counting semaphore struct
    pthread_mutex_t mutex;
    pthread_cond_t cv;
    int value;
    int max_value;
} sem_t;

void wait(sem_t *sem)
{
    // blocking wait of semaphores
    pthread_mutex_lock(&(sem->mutex));
    // block the thread if no resource is available
    while (sem->value == 0) pthread_cond_wait(&(sem->cv), &(sem->mutex));
    sem->value--;
    pthread_mutex_unlock(&(sem->mutex));
}

void signal(sem_t *sem)
{
    // signal of semaphores
    pthread_mutex_lock(&(sem->mutex));
    if (sem-> value < sem->max_value) sem->value++;
    // wake up blocked thread when resource becomes available
    if (sem->value > 0) pthread_cond_signal(&(sem->cv));
    pthread_mutex_unlock(&(sem->mutex));
}

void signal_print_value(sem_t sem)
{
    // print value of semaphore (for debugging)
    printf("%d\n", sem.value);
}

void init(sem_t *sem, int value)
{
    // initializing semaphore
    sem->value = value;
    sem->max_value = value;
}

#define LEFT pnum
#define RIGHT (pnum + 1) % num_phil
#define TIME 1

int num_phil;           // number of philosophers
int *phil;              // array of philosophers
sem_t mutex;            // resource semaphore
sem_t *forks;           // semaphores for forks 
sem_t sauce_bowls;      // semaphore for sauce bowls

void pick_left_fork(int pnum);
void pick_right_fork(int pnum);
void pick_sauce_bowls(int pnum);
void *phil_thread(void *num);

int main(int argc, char **argv) 
{
    if (argc < 2)
    {
        printf("Enter the number of philosophers as an argument.\n");
        return 1;
    }

    num_phil = atoi(argv[1]);
    forks = (sem_t *) malloc(num_phil * sizeof(sem_t));
    phil = (int *) malloc(num_phil * sizeof(int));

    init(&mutex, 1);
    init(&sauce_bowls, 1);

    pthread_t phil_tid[num_phil];

    for (int i = 0; i < num_phil; i++)
    {
        init(&forks[i], 1);
        phil[i] = i;
        // creating threads for each philosopher
        pthread_create(&phil_tid[i], NULL, phil_thread, &phil[i]);
    }

    for (int i = 0; i < num_phil; i++)
    {
        // joining the threads for each philosopher
        pthread_join(phil_tid[i], NULL);
    }

    free(forks);
    free(phil);
    
    return 0;
}

void *phil_thread(void *num)
{
    while (1)
    {
        int *i = (int *)num;
        sleep(TIME);    // thinking

        pick_left_fork(*i);
        pick_right_fork(*i);
        // notifying that other philosophers can acquire forks now
        signal(&mutex);
        pick_sauce_bowls(*i);
    }
    pthread_exit(0);
}

void pick_left_fork(int pnum)
{
    // wait to acquire left fork
    wait(&mutex);
    wait(&forks[LEFT]);
}

void pick_right_fork(int pnum)
{
    // wait to acquire right fork
    wait(&forks[RIGHT]);
}

void pick_sauce_bowls(int pnum)
{
    // wait to acquire both the bowls
    wait(&sauce_bowls);

    sleep(TIME);    // eating
    printf("Philosopher %d eats using fork %d and fork %d\n", phil[pnum] + 1, LEFT + 1, RIGHT + 1);

    // release all the resources
    signal(&forks[RIGHT]);
    signal(&forks[LEFT]);
    signal(&sauce_bowls);
}