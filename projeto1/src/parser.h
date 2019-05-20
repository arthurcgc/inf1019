#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "vector.h"

#define line_cap 256
#define command_cap 50

// imprime os programas armazenados em Vector *v
void printPrograms(Vector *v);

// lê comandos de stdin, contanto que as rajadas sejam char
void ReadCommands(Vector *programs);

// lê comandos de test_case.txt -> utilizada na main
void ReadFile(Vector *programs);

char *my_itoa(int num, char *str);