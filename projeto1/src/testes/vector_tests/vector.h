#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define line_cap 256
#define command_cap 50

typedef struct vector Vector;
typedef struct command Command;

struct vector
{
    Command *begin;
    Command *curr;
    Command *end;
    int size;
};

struct command
{
    char program_name[line_cap];
    int time_sequence[3];
    int inx_time;
    Command *next;
    Command *prvs;
};

Command *create_command(char *program_name, char *time_windows);
void push_back(Vector *v, Command *c);
Vector *create_vector();
int size(Vector *v);
Command *pop_curr(Vector *v);