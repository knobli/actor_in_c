/*
 * client.c
 *
 *  Created on: 10.06.2014
 *      Author: knobli
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mqueue.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <errno.h>

#define QUEUE_NAME "/messages"
#define RESULT_QUEUE_NAME "/results"
#define MAX_MSG_LEN 1024

int main(int argc, char** argv) {
  int ret;

  struct mq_attr mqAttr;
  mqAttr.mq_maxmsg = 10;
  mqAttr.mq_msgsize = MAX_MSG_LEN;

  // create queue, open for writing
  mqd_t mq = mq_open(QUEUE_NAME, O_CREAT | O_WRONLY, 0644, &mqAttr);
  assert(mq != -1);

  // send a message
  char *msg = strdup("1");
  ret = mq_send(mq, msg, strlen(msg) + 1, 0);
  assert(ret == 0);


  // send a message
  char *msg2 = strdup("2");
  ret = mq_send(mq, msg2, strlen(msg2) + 1, 0);
  assert(ret == 0);

  // send a message
  char *msg3 = strdup("4");
  ret = mq_send(mq, msg3, strlen(msg3) + 1, 0);
  assert(ret == 0);


  // send a message
  char *msg4 = strdup("5");
  ret = mq_send(mq, msg4, strlen(msg4) + 1, 0);
  assert(ret == 0);

  printf("Wait for response\n");
  sleep(1);
  // open queue
  mqd_t mq_res = mq_open(RESULT_QUEUE_NAME, O_RDONLY | O_NONBLOCK);
  assert(mq_res != -1);

  ssize_t nbytes;

  pthread_t *nextListEntry = malloc(sizeof(pthread_t));
  assert(nextListEntry != NULL);

  // wait for and dispatch messages
  char *buf = malloc(sizeof(char) * MAX_MSG_LEN);
  assert(buf != NULL);
  while ((nbytes = mq_receive(mq_res, buf, MAX_MSG_LEN, 0)) > 0) {

	  printf("Result: %s\n", buf);
    buf = malloc(sizeof(char) * MAX_MSG_LEN);
    assert(buf != NULL);

  }

  return EXIT_SUCCESS;
}

