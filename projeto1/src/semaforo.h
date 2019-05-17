#pragma once
#include <sys/sem.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <string.h>

union semun
{
    int val; // valor do semáforo
    struct semid_ds *buf; // operação do semáforo
    unsigned short *array; // flags da operação
};

void delSemValue(int semId);
int setSemValue(int semId);
int semaforoP(int semId);
int semaforoV(int semId);
