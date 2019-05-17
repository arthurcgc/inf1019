#include "vector.h"

/*parsing functions*/

void stripTokens(char *s)
{
    char buffer = s[0];
    int i;
    for (i = 0; *s != '\0'; i++)
    {
        if(*s == '(' || *s == ')' || *s == ',')
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
    char n1[line_cap], n2[line_cap], n3[line_cap];

    sscanf(s, "%s %s %s", n1, n2, n3);
    v[0] = atoi(n1);
    v[1] = atoi(n2);
    v[2] = atoi(n3);
}

/*end of parsing functions*/

Command *create_command(char *program_name, char *time_windows)
{
    Command *c = (Command*)malloc(sizeof(Command));
    strcpy(c->program_name, program_name);
    int v[3];
    parseTimeWindows(v, time_windows);
    for(int i = 0; i < 3; i++)
        c->time_sequence[i] = v[i];
    
    c->inx_time = 0;
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
    dest->inx_time = source->inx_time;
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
