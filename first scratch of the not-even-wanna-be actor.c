#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mqueue.h>
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <errno.h>

#define QUEUE_NAME "/messages"

struct actor_arg {
    char *msg;
    ssize_t length;
};

/* the wanna-be actor. is run by
 * a thread per incoming message */
void *run_actor(void *arg) {
    struct actor_arg *params = arg;
    printf("Actor received Message(%zu): [%s]\n",
            params->length, params->msg);
    free(params->msg);

    pthread_exit(NULL);
}

/* single thread constantly waiting
 * for new messages in the queue.
 * creates actors per message */
void *dispatch(void *arg) {
    // open queue
    mqd_t mq = mq_open(QUEUE_NAME, O_RDONLY);
    assert(mq != -1);

    int ret;
    ssize_t nbytes;
    char *buf;
    pthread_t *actor;
    struct actor_arg *aarg;

    // todo: make the following a loop

    // wait for and dispatch messages
    buf = malloc(sizeof(char) * 1024);
    assert(buf != NULL);
    nbytes = mq_receive(mq, buf, 1024, NULL);
    actor = malloc(sizeof(pthread_t));
    aarg = malloc(sizeof(struct actor_arg));
    aarg->msg = buf;
    aarg->length = nbytes;


    ret = pthread_create(actor, NULL, run_actor, aarg);
    assert(ret == 0);

    pthread_join(*actor, NULL);
    pthread_exit(NULL);
}

int main(int argc, char** argv) {
    int ret;

    // create queue, open for writing
    mqd_t mq = mq_open(QUEUE_NAME, O_CREAT | O_WRONLY, 0644, 0);
    assert(mq != -1);

    // spawn the dispatcher
    pthread_t dispatcher;
    ret = pthread_create(&dispatcher, NULL, dispatch, NULL);
    assert(ret == 0);

    // send a message
    char *msg = strdup("hello");
    ret = mq_send(mq, msg, strlen(msg)+1, 0);
    assert(ret == 0);

    // wait for dispatcher to complete
    pthread_join(dispatcher, NULL);

    mq_unlink(QUEUE_NAME);

    return EXIT_SUCCESS;
}
