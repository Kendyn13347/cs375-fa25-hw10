// join_no_state_var.c  (builds everywhere; uses native pthreads)
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

pthread_cond_t  c = PTHREAD_COND_INITIALIZER;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
int done = 0;

void* child(void* arg) {
    printf("child: begin\n");
    sleep(1);

    pthread_mutex_lock(&m);
    done = 1;
    printf("child: signal\n");
    pthread_cond_signal(&c);
    pthread_mutex_unlock(&m);

    return NULL;
}

int main(void) {
    pthread_t p;

    printf("parent: begin\n");
    pthread_create(&p, NULL, child, NULL);

    pthread_mutex_lock(&m);
    printf("parent: check condition\n");
    while (done == 0) {
        printf("parent: wait to be signalled...\n");
        pthread_cond_wait(&c, &m); // atomically releases m, sleeps, then re-acquires
    }
    pthread_mutex_unlock(&m);

    pthread_join(p, NULL);
    printf("parent: end\n");
    return 0;
}
