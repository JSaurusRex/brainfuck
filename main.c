#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAXINT 65535
#define MAXCOLON 65535
#define MAXBUFFER 2

unsigned char * intarray; //all variables (array)
unsigned short int currentarray; //what variable is currently in use
int * colon; //what the position is of the last colon (array)
short int lastcolon = 0;
short int position = 0;

unsigned char * buffer;
short int display = 0;

int pos = 0;
int * funcs;

void writeOutput();
void load_file(char * str);
void run();

int main(int argc, char **argv)
{
    colon = (int*) calloc(MAXCOLON, sizeof(int));
    funcs = (int*) calloc(10000, sizeof(int));


    if(argc == 0) {
        printf("error: no file specified\n");
        return 0;
    }

    load_file(argv[1]);

    intarray = (unsigned char*) calloc(MAXINT, sizeof(unsigned char));
    buffer = (unsigned char*) calloc(MAXBUFFER+1, sizeof(unsigned char));

    clock_t begin = clock();
    run();
    printf("\nend of program, time: %f\n", (double) (clock() - begin) / CLOCKS_PER_SEC);
    displayBuffer();
    return 0;
}

void run ()
{
    int * currentfunc = funcs;
    int * currentcell = intarray;
    position = -1;
    while(currentfunc++ < pos)
        {
            switch(*currentfunc)
            {
                case 0: //add
                    currentfunc++;
                    *currentcell += *currentfunc;
                    break;

                case 1: //move
                    currentfunc++;
                    currentcell += *currentfunc;
                    //if(currentcell < intarray) currentcell = intarray + MAXINT;
                    break;

                case 2: //loop open
                    currentfunc++;
                    if(*currentcell == 0)
                    {
                        currentfunc = *currentfunc;
                    }
                    break;

                case 3: //loop close
                    currentfunc++;
                    if(*currentcell != 0)
                    {
                        currentfunc = *currentfunc;
                    }
                    break;

                case 4: //point
                    buffer[display] = *currentcell;
                    display++;
                    if(MAXBUFFER <= display) displayBuffer();
                    currentfunc++;
                    break;

                case 5: //clear
                    *currentcell = 0;
                    currentfunc++;
                    break;
                case 6: //move loop
                    currentfunc++;
                    while( *currentcell != 0)
                    {
                        currentcell += *currentfunc;
                    }
                    break;
            }

        }

}

void displayBuffer()
{
    if(display == 0) return;
    buffer[display+1] = '\0';
    printf("%s", buffer);
    display = 0;
}


int intcount = 0;
int movecount = 0;

int colonCount= 0;

void compile (char c)
{

    if(intcount != 0) //+ / -
    {
        if(c == '<' || c == '>' || c == '.' || c == '[' || c == ']')
        {
            if(c == ']' && pos > 1)
            {
                if(funcs[pos - 2] == 2)
                {
                    pos-= 2;
                    funcs[pos] = 5;
                    pos+=2;
                    intcount = 0;
                    colonCount--;
                    return;
                }
            }
            funcs[pos] = 0;
            funcs[pos+1] = intcount;
            intcount = 0;
            pos+=2;
    }
    }

    if(movecount != 0) // > / <
    {
        if(c == '+' || c == '-' || c == '.' || c == '[' || c == ']')
        {
            if(c == ']' && pos > 1)
            {
                if(funcs[pos - 2] == 2)
                {
                    pos -= 2;
                    funcs[pos] = 6;
                    funcs[pos+1] = movecount;
                    pos+= 2;

                    movecount = 0;
                    colonCount--;
                    return;
                }
            }
            funcs[pos] = 1;
            funcs[pos+1] = movecount;
            movecount = 0;
            pos+=2;
        }
    }



    switch(c)
    {

	    case '+':
	    	intcount++;
	    	break;
	    case '-':
	    	intcount--;
	    	break;

	    case '>':
	    	movecount++;
	    	break;
	    case '<':
	    	movecount--;
	    	break;

	    case '[':
	        funcs[pos] = 2;
	        colonCount++;
	        colon[colonCount] = pos + funcs;
	        pos+=2;
			break;

	    case ']':

	        funcs[pos] = 3;
	        pos++;
	        funcs[pos] = colon[colonCount] +1;
	        funcs[colon[colonCount]+1] = pos + funcs;
	        colonCount--;
	        pos++;
	        break;

	    case '.':
	        funcs[pos] = 4;
	        pos+=2;
	        break;
    }

}

void load_file (char * file)
{
    FILE *fp;
    fp = fopen(file, "r"); // read mode
    rewind(fp);
    fseek(fp, 0, 2);
    int size = ftell(fp);
    rewind(fp);

    if (fp == NULL)
    {
        printf("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }
    int i = 0;
    char ch;
    while(i < size)
    {
        compile(fgetc(fp));
        i++;
    }
    fclose(fp);
    return;
}





