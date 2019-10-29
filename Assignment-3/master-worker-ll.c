#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <pthread.h>

struct node {
  int data;
  node* next;
};

int item_to_produce;
int total_items, max_buf_size, num_workers;
pthread_mutex_t lock;
pthread_cond_t count_cond = PTHREAD_COND_INITIALIZER;
int num_items = 0;
node* start = NULL;

// declare any global data structures, variables, etc that are required
// e.g buffer to store items, pthread variables, etc

void append(int num) {
  node *t, *temp;

  t = (struct node*)malloc(sizeof(struct node));

  if (start == NULL) {
    start = t;
    start->data = num;
    start->next = NULL;
    return;
  }

  temp = start;

  while(temp-> next != NULL)
    temp = temp-> next;

  temp->next = t;
  t->data = num;
  t-> next = NULL;

}

void print_produced(int num) {

  printf("Produced %d\n", num);
}

void print_consumed(int num, int worker) {

  printf("Consumed %d by worker %d\n", num, worker);
  
}

void *consume_requests_loop(void *data)
{
  int thread_id = *((int *)data);

  node* temp = start;
  pthread_mutex_lock(&lock);

  while(num_items <= 0) {
    pthread_cond_wait(&count_cond, &lock);

  }
  while(num_items >= 1 && start != NULL) {
    print_consumed(start->data, thread_id);
    start = start->next;
    free(temp);
    num_items = num_items - 1;
  }

  pthread_mutex_unlock(&lock);

  return 0;
}

/* produce items and place in buffer (array or linked list)
 * add more code here to add items to the buffer (these items will be consumed
 * by worker threads)
 * use locks and condvars suitably
 */
void *generate_requests_loop(void *data)
{
  int thread_id = *((int *)data);

  //pthread_mutex_lock(&lock);
  while(1)
    {
      if(item_to_produce >= total_items)
	break;
      pthread_mutex_lock(&lock);
      append(item_to_produce);
      num_items++;
      print_produced(item_to_produce);
      item_to_produce++;
      
      if (num_items>0) pthread_cond_signal(&count_cond);
      pthread_mutex_unlock(&lock);
    }
  //pthread_mutex_unlock(&lock);
  return 0;
}

//write function to be run by worker threads
//ensure that the workers call the function print_consumed when they consume an item

int main(int argc, char *argv[])
{
 
  int master_thread_id = 0;
  pthread_t master_thread;
  item_to_produce = 0;
  
  if (argc < 4) {
    printf("./master-worker #total_items #max_buf_size #num_workers e.g. ./exe 10000 1000 4\n");
    exit(1);
  }
  else {
    num_workers = atoi(argv[3]);
    total_items = atoi(argv[1]);
    max_buf_size = atoi(argv[2]);
  }
  
  // Initlization code for any data structures, variables, etc
  // for (int i = 0; i < 10000; i++) shared_buff[i] = -1;

  //create master producer thread
  pthread_create(&master_thread, NULL, generate_requests_loop, (void *)&master_thread_id);

  //create worker consumer threads
  pthread_t worker_threads[num_workers];
  int worker_thread_id[num_workers];

  pthread_mutex_init(&lock, NULL);

  for (int i = 0; i < num_workers; i++)
  {
    worker_thread_id[i] = i+1;
  }

  for (int i = 0; i < num_workers; i++)
  {
    pthread_create(&worker_threads[i], NULL, consume_requests_loop, (void *)&worker_thread_id[i]);
  }
  

  //wait for all threads to complete
  pthread_join(master_thread, NULL);
  printf("master joined\n");

  //deallocate and free up any memory you allocated
  
  return 0;
}