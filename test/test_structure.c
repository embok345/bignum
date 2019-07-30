#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <pthread.h>
#include "../src/bignum.h"

void* test1(void *v) {
    printf("1\n");
    assert( 1 == 1 );
    return NULL;
}
void* test2(void *v) {
    printf("2\n");
    assert( 1 == 2 );
    return NULL;
}

int main(void) {
    pthread_t t1, t2;

    pthread_create(&t1, NULL, test1, NULL);
    pthread_create(&t2, NULL, test2, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

}
