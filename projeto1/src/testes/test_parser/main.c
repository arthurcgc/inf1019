#include <stdio.h>
#include "parser.h"
#include "vector.h"



int main(int argc, char const *argv[])
{
    Vector *line1;
    //creating vector
    line1 = create_vector(1);
    ReadFile(line1);
    printPrograms(line1);

    return 0;
}
