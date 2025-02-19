#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <stdbool.h>

typedef struct {
    int id;
    int a;
    int b;
} proArgs;

typedef struct {
    int id;
    int c;
    int d;
} conArgs;

sem_t semMagazyn;
sem_t semProducers;
sem_t semConsumers;
#define MAGAZYN_PLIK "magazyn.txt"
int POJEMNOSC; //maksymalna pojemnosc magazynu

void initMagazyn(){
    FILE* magazyn = fopen(MAGAZYN_PLIK, "w");
    if (magazyn == NULL){
        perror("Cannot open file magazyn.txt");
        exit(1);
    }
    fprintf(magazyn, "%d\n", 0);
    fclose(magazyn);
}

int readFromMagazyn(){
    FILE* magazyn = fopen(MAGAZYN_PLIK, "r");
    if (magazyn == NULL){
        perror("Cannot open file magazyn.txt");
        exit(1);
    }
    int stan;
    fscanf(magazyn, "%d", &stan);
    fclose(magazyn);
    return stan;
}

void writeToMagazyn(int stan){
    FILE* magazyn = fopen(MAGAZYN_PLIK, "w");
    if (magazyn == NULL){
        perror("Cannot open file magazyn.txt");
        exit(1);
    }
    fprintf(magazyn, "%d\n", stan);
    fclose(magazyn);
}

void* producer(void* args){
    proArgs* producentArgs = (proArgs*)args;
    int id = producentArgs->id;
    int beg = producentArgs->a;
    int end = producentArgs->b;
    free(args);

    char logPlik[60];
    sprintf(logPlik, "producent%d.log", id);
    bool sukces = true;
    int y = 0;
    while (1){
        if (sukces){
            y = rand() % (end-beg+1)+beg;
        }
        sukces = false;
        sem_wait(&semProducers);
        sem_wait(&semMagazyn);
        sleep(1);
        int stan = readFromMagazyn();
        if (stan + y <= POJEMNOSC){
            sukces = true;
            stan += y;
            writeToMagazyn(stan);
        }
        FILE* log = fopen(logPlik, "a");
        if (sukces){
            fprintf(log, "Producent %d wlozyl %d towarow, stan magazynu %d\n", id, y, stan);
            fclose(log);
        }
        else {
            fprintf(log, "Producentowi %d nie udalo sie wlozyc %d towarow\n", id, y);
            fclose(log);
        }
        
        if (stan <= POJEMNOSC/2){
            sem_post(&semProducers);
        }
        else {
            sem_post(&semConsumers);
        }
        sem_post(&semMagazyn);
        sleep(1);
    }
}

void* consumer(void* args){
    conArgs* consumerArgs = (conArgs*)args;
    int beg = consumerArgs->c;
    int end = consumerArgs->d;
    int id = consumerArgs->id;
    free(args);

    char logPlik[60];
    sprintf(logPlik, "konsument%d.log", id);
    bool sukces = true;
    int x = 0;
    while (1){
        if (sukces){
            x = rand() % (end-beg+1)+beg;
        }
        sukces = false;
        sem_wait(&semConsumers);
        sem_wait(&semMagazyn);
        sleep(1);
        int stan = readFromMagazyn();
        if (stan >= x){
            sukces = true;
            stan -= x;
            writeToMagazyn(stan);
        }
        FILE* log = fopen(logPlik, "a");
        if (sukces){
            fprintf(log, "Konsument %d pobral %d towarow, stan magazynu %d\n", id, x, stan);
            fclose(log);
        }
        else {
            fprintf(log, "Konsumentowi %d nie udalo sie pobrac %d towarow, stan magazynu: %d\n", id, x, stan);
            fclose(log);
        }

        if (stan <= POJEMNOSC/2){
            sem_post(&semProducers);
        }
        else {
            sem_post(&semConsumers);
        }
        sem_post(&semMagazyn);
        sleep(1);
    }
}

int main(int argc, char* argv[]){
    srand(time(NULL));
    int amountProd, amountCon, p1, p2, c1, c2, maxMagazyn;
    if (argc == 7){
        printf("MaxMagazyn jest domyslnie 20\n");
        maxMagazyn = 20;
    }
    else if (argc != 8){
        perror("You have to give all arguments");
        return 1;
    }
    else{
        maxMagazyn = atoi(argv[7]);
    }
    amountProd = atoi(argv[1]);
    amountCon = atoi(argv[2]);
    p1 = atoi(argv[3]);
    p2 = atoi(argv[4]);
    c1 = atoi(argv[5]);
    c2 = atoi(argv[6]);
    POJEMNOSC = maxMagazyn;
    pthread_t prod[amountProd];
    pthread_t con[amountCon];
    initMagazyn();
    sem_init(&semMagazyn, 0, 1);
    sem_init(&semProducers, 0, 1);
    sem_init(&semConsumers, 0, 1);
    int i;
    for (i=0; i<amountProd; i++){
        proArgs* args = malloc(sizeof(proArgs));
        args->id=i;
        args->a=p1;
        args->b=p2;
        if (pthread_create(&prod[i], NULL, &producer, args) != 0){
            perror("Error producer thread\n");
            exit(1);
        }
    }
    int j;
    for (j=0; j<amountCon; j++){
        conArgs* args = malloc(sizeof(conArgs));
        args->id=j;
        args->c=c1;
        args->d=c2;
        if (pthread_create(&con[j], NULL, &consumer, args) != 0){
            perror("Error producer thread\n");
            exit(1);
        }
    }
    for (int i =0; i<amountProd;i++){
        pthread_join(prod[i],NULL);
    }
    for (int i =0; i<amountCon;i++){
        pthread_join(con[i],NULL);
    }
    sem_destroy(&semMagazyn);
    sem_destroy(&semProducers);
    sem_destroy(&semConsumers);
    return 0;
}
