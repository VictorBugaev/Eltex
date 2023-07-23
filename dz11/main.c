#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_SHOPS 5
#define INITIAL_PRODUCTS_COUNT 1000
#define NUM_CONSUMERS 3
#define INITIAL_NEEDS 3000
#define SESSION_AMOUNT 300
#define LOAD_AMOUNT 100

typedef struct {
    int products_count;
    pthread_mutex_t lock;
} Shop;

typedef struct {
    int needs;
    int shop_id;
} Consumer;

Shop shops[NUM_SHOPS];
Consumer consumers[NUM_CONSUMERS];
int remaining_consumers = NUM_CONSUMERS;

void initialize_shops_and_consumers() {
    int i;
    for (i = 0; i < NUM_SHOPS; ++i) {
        shops[i].products_count = INITIAL_PRODUCTS_COUNT;
        pthread_mutex_init(&shops[i].lock, NULL);
        printf("%d\t", shops[i].products_count);
    }
    printf("\n");
    for (i = 0; i < NUM_CONSUMERS; ++i) {
        consumers[i].needs = INITIAL_NEEDS;
        consumers[i].shop_id = i % NUM_SHOPS;
        printf("Consumer %d needs = %d, shop_id = %d\n", i, consumers[i].needs, consumers[i].shop_id);
    }
    printf("\n");
}

void *shop_loader(void *args) {
    int i;
    int run = 1;
    while (run) {
        for (i = 0; i < NUM_SHOPS; ++i) {
            pthread_mutex_lock(&shops[i].lock);
            shops[i].products_count += LOAD_AMOUNT;
            printf("Loaded in shop %d, shop[%d] = %d\n", i, i, shops[i].products_count);
            fflush(stdout);
            pthread_mutex_unlock(&shops[i].lock);
            
            if (remaining_consumers <= 0) {
                run = 0;
                break;
            }
            sleep(1);
        }
    }
    return NULL;
}

void *consumer_shopping(void *args) {
    Consumer *consumer = (Consumer *)args;
    while (consumer->needs > 0) {
        int shop_id = consumer->shop_id;
        pthread_mutex_lock(&shops[shop_id].lock);
        if (shops[shop_id].products_count > 0) {
            int products_taken = (consumer->needs >= shops[shop_id].products_count) ? shops[shop_id].products_count : consumer->needs;
            consumer->needs -= products_taken;
            shops[shop_id].products_count -= products_taken;
            printf("Consumer needs = %d, shop[%d] = %d\n", consumer->needs, shop_id, shops[shop_id].products_count);
            fflush(stdout);
        }
        pthread_mutex_unlock(&shops[shop_id].lock);
    }
    --remaining_consumers;
    printf("Remaining consumers = %d\n", remaining_consumers);
    fflush(stdout);
    printf("End consumer shopping, needs = %d\n", consumer->needs);
    return NULL;
}

void cleanup_shops_and_consumers() {
    int i;
    for (i = 0; i < NUM_SHOPS; ++i) {
        pthread_mutex_destroy(&shops[i].lock);
    }
}

int main() {
    initialize_shops_and_consumers();
    
    pthread_t consumers_threads[NUM_CONSUMERS];
    pthread_t loader_thread;
    int *status;
    int i;
    for (i = 0; i < NUM_CONSUMERS; ++i) {
        pthread_create(&consumers_threads[i], NULL, consumer_shopping, (void *)&consumers[i]);
    }
    pthread_create(&loader_thread, NULL, shop_loader, NULL);
    
    for (i = 0; i < NUM_CONSUMERS; ++i) {
        pthread_join(consumers_threads[i], (void **)&status);
    }
    pthread_join(loader_thread, (void **)&status);
    
    cleanup_shops_and_consumers();
    printf("End program\n");
    return 0;
}
