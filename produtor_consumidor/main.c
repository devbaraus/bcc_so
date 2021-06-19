
#include <unistd.h>
#include "stdio.h"
#include "stdlib.h"
#include "pthread.h"
#include "semaphore.h"

int *buffer;
int freeSlot;
int bufferSize;
pthread_mutex_t bufferMutex;
sem_t producer_s, consumer_s;


void printb(int *buffer, int size) {
    for (int i = 0; i < size; ++i) {
        if (buffer[i] != -1)
        {
            printf("[%d] %d\n", i, buffer[i]);
        } else {
            printf("[%d] empty\n", i);
        }
    }
}

void *producer()
{
    int produced_item;
    while (1)
    {
        produced_item = rand();

        sem_wait(&producer_s);
        pthread_mutex_lock(&bufferMutex);

        buffer[freeSlot] = produced_item;
        freeSlot++;

        printf("##Producer##\n");
        printf("Next available slot: %d\n", freeSlot);
        printb(buffer, bufferSize);
        printf("Produced item: %d\n\n", produced_item);

        pthread_mutex_unlock(&bufferMutex);
        sem_post(&consumer_s);
        sleep(rand() % 5);
    }
}

void *consumer()
{
    int consumed_item;
    while (1)
    {
        sem_wait(&consumer_s);
        pthread_mutex_lock(&bufferMutex);

        consumed_item = buffer[freeSlot - 1];
        buffer[freeSlot - 1] = -1;
        freeSlot--;
        printf("##Consumer##\n");
        printf("Next available slot: %d\n", freeSlot);
        printb(buffer, bufferSize);
        printf("Consumed item: %d\n\n", consumed_item);

        pthread_mutex_unlock(&bufferMutex);
        sem_post(&producer_s);

        sleep(rand() % 2);
    }
}

int main()
{
    int M, N, B;

    printf("Quantidade de produtores: ");
    scanf("%d", &M);
    printf("Quantidade de consumidores: ");
    scanf("%d", &N);
    printf("Tamanho do buffer: ");
    scanf("%d", &B);

    bufferSize = B;
    buffer = malloc(bufferSize * (sizeof (int)));

    pthread_t *producer_t, *consumer_t;
    sem_init(&consumer_s, 0, 0);
    sem_init(&producer_s, 0, bufferSize);
    pthread_mutex_init(&bufferMutex, NULL);

    for (int i = 0; i < bufferSize; ++i)
    {
        buffer[i] = -1;
    }

    producer_t = malloc(M * (sizeof (pthread_t)));
    consumer_t = malloc(N * (sizeof (pthread_t)));

    for (int i = 0; i < N; ++i)
    {
        pthread_create(&consumer_t[i], NULL, consumer, NULL);
    }

    for (int i = 0; i < M; ++i)
    {
        pthread_create(&producer_t[i], NULL, producer, NULL);
    }

    while (1) {
        sleep(1);
    }

    return 0;
}