#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "vector.h"

#define line_cap 256
#define command_cap 50

char *my_itoa(int num, char *str);

void printPrograms(Vector *v);

void ReadCommands(Vector *programs);

void ReadFile(Vector *programs);

