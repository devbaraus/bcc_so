#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

pthread_mutex_t *mutexes;
float *x, *y, *z;

typedef struct {
    float *vetor;
    unsigned int posInicial;
    unsigned int posFinal;
    unsigned int contMutexThread;
} thread_argPreenche, *ptr_thread_argPreenche;

typedef struct {
    float *x;
    float *y;
    float *z;
    unsigned int posInicial;
    unsigned int posFinal;
    unsigned int contMutexThread;
} thread_argSoma, *ptr_thread_argSoma;

float random_number() {
    return (float) (rand() % 10000) / 10000.0;
}

void *somaVetores(void *argPtr) {
    ptr_thread_argSoma targ = (ptr_thread_argSoma) argPtr;
    pthread_mutex_lock(&(mutexes[targ->contMutexThread]));
    for (unsigned int i = targ->posInicial; i < targ->posFinal; i++) {
        targ->z[i] = targ->x[i] + targ->y[i];
    }
    pthread_mutex_unlock(&(mutexes[targ->contMutexThread]));

    return NULL;
}

void *preencheVetores(void *argPtr) {
    ptr_thread_argPreenche targ = (ptr_thread_argPreenche) argPtr;
    pthread_mutex_lock(&(mutexes[targ->contMutexThread]));
    for (unsigned int i = targ->posInicial; i < targ->posFinal; i++) {
        targ->vetor[i] = random_number();
    }
    pthread_mutex_unlock(&(mutexes[targ->contMutexThread]));
    return NULL;
}

void verificarVetores(int tamanhoVetores) {
    for (int i = 0; i < tamanhoVetores; i++) {
        if (x[i] == 0) {
            printf("Valor 0 em X");
            break;
        }
        if (y[i] == 0) {
            printf("Valor 0 em Y");
            break;
        }
        if (z[i] == 0) {
            printf("Valor 0 em Z");
            break;
        }

        if (x[i] + y[i] != z[i]) {
            printf("A soma não está correta");
            break;
        }
    }
}

int main(int argc, char **argv) {
    int n_threads, tam_vetores;

    // Pega input usuário
    printf("Deseja utilizar quantas threads?\n--> ");
    scanf("%i", &n_threads);

    do {
        printf("Qual o tamanho dos vetores a serem somados?\n--> ");
        scanf("%i", &tam_vetores);
        if (tam_vetores % n_threads != 0) {
            printf("\n\033[0;31mO tamanho do vetor deve ser divisivel pela quantidade de threads...\033[0m\n\n");
        }
    } while (tam_vetores % n_threads != 0);

    // gera seed
    srand((unsigned) time(NULL));

    // inicializa vetores
    mutexes = malloc(n_threads * sizeof(pthread_mutex_t));
    pthread_t threads[n_threads];
    thread_argPreenche argPreenche[n_threads];
    thread_argSoma argSoma[n_threads];

    x = malloc(tam_vetores * sizeof(float));
    y = malloc(tam_vetores * sizeof(float));
    z = malloc(tam_vetores * sizeof(float));

    int tam_pos = tam_vetores / n_threads;

    // distribui tamanhos e mutexes
    for (int i = 0; i < n_threads; i++) {
        argPreenche[i].posInicial = tam_pos * i;
        argPreenche[i].posFinal = tam_pos * (i + 1);
        argPreenche[i].contMutexThread = i;

        argSoma[i].posInicial = tam_pos * i;
        argSoma[i].posFinal = tam_pos * (i + 1);
        argSoma[i].contMutexThread = i;

        pthread_mutex_init(&mutexes[i], NULL);
    }

    for (int i = 0; i < n_threads; i++) {
        printf("A thread %u processará os elementos dos vetores nas posições de %u a %u\n", i,
               argPreenche[i].posInicial, argPreenche[i].posFinal - 1);
    }

    clock_t begin = clock();
    // preenche x
    for (int i = 0; i < n_threads; i++) {
        argPreenche[i].vetor = x;
        pthread_create(&(threads[i]), NULL, preencheVetores, &argPreenche[i]);
    }

    // preenche y
    for (int i = 0; i < n_threads; i++) {
        argPreenche[i].vetor = y;
        pthread_create(&(threads[i]), NULL, preencheVetores, &argPreenche[i]);
    }

    // somar x + y = z
    for (int i = 0; i < n_threads; i++) {
        argSoma[i].x = x;
        argSoma[i].y = y;
        argSoma[i].z = z;
        pthread_create(&(threads[i]), NULL, somaVetores, &argSoma[i]);
    }

    // evita o programa terminar antes de as threads executarem
    for (int i = 0; i < n_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    clock_t end = clock();
    double time_spent = (double) (end - begin) / CLOCKS_PER_SEC / n_threads;
    printf("Tempo decorrido: %f segundos\n", time_spent);

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--check") == 0) {
            verificarVetores(tam_vetores);
        }

        if (strcmp(argv[i], "--debug") == 0) {
            for (int i = 0; i < tam_vetores; i++) {
                printf("[%i]    %f    %f    %f    %s\n", i, x[i], y[i], z[i], (x[i] + y[i] == z[i]) ? "true" : "false");
            }
        }
    }

    return 0;
}
