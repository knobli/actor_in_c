#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mqueue.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <errno.h>

#include <concurrentLinkedList.h>

#define CLEANUP_FREQUENCY 10
#define QUEUE_NAME "/messages"
#define MAX_MSG_LEN 1024

struct actor_arg {
  char *msg;
  ssize_t length;
};

/* the wanna-be actor. is run by
 * a thread per incoming message */
void *run_actor(void *arg) {
  struct actor_arg *params = arg;

  printf("Actor received Message(%zu): [%s]\n", params->length, params->msg);
  int x = atoi(params->msg);
  int y = x*x;
  printf("Result = %12d\n", y);

  free(params->msg);
  pthread_exit(NULL);
}

void *clean_up(void *arg) {
  ConcurrentLinkedList *threadList = (ConcurrentLinkedList *) arg;

  printf("Hi from cleanup thread \n");
  pthread_t *thread_to_clean; 

  int num = 0;
  while (1) { 
    sleep(CLEANUP_FREQUENCY);
    printf("Cleanup Thread: Start Removing Threads\n");

    num = 0;
    // gives back the shallow copy of the element
    while(getFirstListElement(threadList, (void *)&thread_to_clean) > 1){ 

      int ret = pthread_join(*thread_to_clean, NULL);
      assert(ret == 0);
      free(thread_to_clean);

      removeFirstListElement(threadList);
      num++;
    }
    printf("Removed %d Threads\n", num);
  }
  // Should never happen!
  printf("Bye Bye from cleanup thread - ERROR!\n");
  return (void *) -1;
}

/* single thread constantly waiting
 * for new messages in the queue.
 * creates actors per message */
void *dispatch(void *arg) {
  ConcurrentLinkedList *threadList = (ConcurrentLinkedList *) arg;
  printf("Hi from dispatcher thread \n");

  // open queue
  mqd_t mq = mq_open(QUEUE_NAME, O_RDONLY );
  assert(mq != -1);

  int ret;
  ssize_t nbytes;
  struct actor_arg *aarg;

  pthread_t *nextListEntry = malloc(sizeof(pthread_t));
  assert(nextListEntry != NULL);

  // wait for and dispatch messages
  char *buf = malloc(sizeof(char) * MAX_MSG_LEN);
  assert(buf != NULL);
  while ((nbytes = mq_receive(mq, buf, MAX_MSG_LEN, 0)) > 0) {
    aarg = malloc(sizeof(struct actor_arg));
    aarg->msg = buf;
    aarg->length = nbytes;

    ret = pthread_create(nextListEntry, NULL, run_actor, aarg);
    assert(ret == 0);

    buf = malloc(sizeof(char) * MAX_MSG_LEN);
    assert(buf != NULL);

    appendListElement(threadList,(void *) &nextListEntry, sizeof(pthread_t), "NoName");
    nextListEntry = malloc(sizeof(pthread_t));
    assert(nextListEntry != NULL);
  }
  printf("Bye Bye from dispatcher thread - ERROR!\n");
  pthread_exit(NULL);
}

int main(int argc, char** argv) {
  int ret;

  ConcurrentLinkedList *thread_list = newList();

  // spawn the dispatcher and housekeeping
  pthread_t housekeeper;
  ret = pthread_create(&housekeeper, NULL, clean_up, thread_list);
  assert(ret == 0);
  pthread_t dispatcher;
  ret = pthread_create(&dispatcher, NULL, dispatch, thread_list);
  assert(ret == 0);

  // wait for the threads to complete - will not happen
  pthread_join(dispatcher, NULL);
  pthread_join(housekeeper, NULL);

  mq_unlink(QUEUE_NAME);

  return EXIT_SUCCESS;
}
