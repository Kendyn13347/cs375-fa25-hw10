// join_modular.c  -- modular sync with plain pthreads
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

typedef struct {
    pthread_mutex_t m;
    pthread_cond_t  c;
    int done;
} synchronizer_t;

static void sync_init(synchronizer_t *s) {
    pthread_mutex_init(&s->m, NULL);   // two args: (&mutex, attr)
    pthread_cond_init(&s->c, NULL);    // two args: (&cond,  attr)
    s->done = 0;
}

static void sync_wait(synchronizer_t *s) {
    pthread_mutex_lock(&s->m);
    while (!s->done) {                 // protect predicate with the mutex
        pthread_cond_wait(&s->c, &s->m);
    }
    pthread_mutex_unlock(&s->m);
}

static void sync_signal(synchronizer_t *s) {
    pthread_mutex_lock(&s->m);
    s->done = 1;
    pthread_cond_signal(&s->c);
    pthread_mutex_unlock(&s->m);
}

static void sync_destroy(synchronizer_t *s) {
    pthread_mutex_destroy(&s->m);
    pthread_cond_destroy(&s->c);
}

static void* child(void *arg) {
    synchronizer_t *s = (synchronizer_t*)arg;
    printf("child: begin\n");
    sleep(1);
    printf("child: signal\n");
    sync_signal(s);
    return NULL;
}

int main(void) {
    synchronizer_t s;
    pthread_t t;

    sync_init(&s);

    printf("parent: begin\n");
    pthread_create(&t, NULL, child, &s);

    printf("parent: waiting\n");
    sync_wait(&s);

    pthread_join(t, NULL);
    sync_destroy(&s);
    printf("parent: end\n");
    return 0;
}
