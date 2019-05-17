#include "vector.h"

/*parsing functions*/

void stripTokens(char *s)
{
    char buffer = s[0];
    int i;
    for (i = 0; *s != '\0'; i++)
    {
        if(*s == '(' || *s == ')' || *s == ',' || *s == '\n')
            *s = ' ';
        s++;
    }
    s = s-i;
}

void parseTimeWindows(int *v, char *s)
{
    char buffer = 'f';
    stripTokens(s);
    int i = 0;
    char *sbuffer;
    char n[line_cap];
    int test = 0;

    for(int i = 0;*s!='\0';i++)
    {
        while(*s == ' ')
            s++;
        test = sscanf(s, "%s", n);
        if(test == -1)
            break;
        *(v+i) = atoi(n);
        while(*s == ' ')
            s++;
        s++;
    }
}

/*end of parsing functions*/

Command *create_command(char *program_name, char *time_windows)
{
    Command *c = (Command*)malloc(sizeof(Command));
    strcpy(c->program_name, program_name);
    int v[10];
    int i = 0;
    parseTimeWindows(c->time_sequence, time_windows);
    for(i;;i++)
    {
        if(c->time_sequence[i] == 0)
            break;
    }

    c->time_sequence_tam = i;

    c->itime = 0;
    c->next = NULL;
    c->prvs = NULL;

    return c;

}

int size(Vector *v)
{
    return v->size;
}

Vector *create_vector()
{
    Vector *v = (Vector*)malloc(sizeof(Vector));
    v->begin = NULL;
    v->end = v->begin;
    v->size = 0;
    return v;
}

void push_back(Vector *v, Command *c)
{
    v->size++;
    if(v->end!=NULL)
    {
        v->end->next = c;
        c->prvs = v->end;
        v->end = c;
    }
    else
    {
        v->begin = c;
        v->end = v->begin;
    }
}


void shallowCopy(Command *dest, Command *source)
{
    int tam_time_seq = sizeof(source->time_sequence)/sizeof(int);
    strcat(dest->program_name, source->program_name);
    for(int i = 0; i < tam_time_seq; i++)
        dest->time_sequence[i] = source->time_sequence[i];
    dest->next = source->next;
    dest->prvs = source->prvs;
    dest->itime = source->itime;
}

Command *pop_curr(Vector *v)
{
    Command *ret = create_command("", "");
    shallowCopy(ret, v->curr);
    v->curr->next->prvs = v->curr->prvs;
    v->curr->prvs->next = v->curr->next;

    v->size--;
    free(v->curr);
    return ret;
}
