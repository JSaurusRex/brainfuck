#include <stdio.h>
#include <stdlib.h>

#include "functions.h"



int main(int argc, char **argv)
{
    //checks arguments
    for (int i = 0; i < argc; ++i)
    {
        printf("%s ", argv[i]);
    }



    printf("\nHello world!\n");

    return 0;
}
