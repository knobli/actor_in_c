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
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <errno.h>

#define QUEUE_NAME "/messages"
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

  return EXIT_SUCCESS;
}

