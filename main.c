#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>

#define MAXINT 65535
#define MAXCOLON 65535
#define MAXBUFFER 50000

int colon[MAXCOLON]; //what the position is of the last colon (array)
short int lastcolon = 0;

int pos = 0;

void *funcs[10000];

void displayBuffer();
void addtoBuffer(char c);
void load_file(char * str);
void runJIT ();

int main(int argc, char **argv)
{
    //checks arguments
    int isempty = 0;

    for (int i = 1  ; i < argc; ++i)
    {
        if(argv[i][0] != '-')
        {
            load_file(argv[i]);
            isempty = 1;
        }
    }
    if(!isempty) {
        printf("error: no file specified\n");
        load_file("helloworld.txt");
        //return 0;
    }

    runJIT();
    return 0;
}


enum {eAdd, eMove, eLoopOpen, eLoopClose, ePoint, eClear, eMoveLoop, eExit};

// #define DEBUG
// #define COUNT

 __attribute__((hot)) void runJIT ()
{
    unsigned char intarray[MAXINT]; //all variables (array)
    unsigned char buffer[MAXBUFFER];
    register void **currentfunc = funcs;
    unsigned char * currentcell = intarray;
    unsigned char * currentbuff = buffer;

    int i = 0;
    int cont = 1;
    while(cont)
    {
        switch((long int)funcs[i])
        {
            case eAdd:
                funcs[i] = &&gAdd;
                i++;
                break;
            case eMove:
                funcs[i] = &&gMove;
                i++;
                break;
            case eLoopOpen:
                funcs[i] = &&gLoopOpen;
                i++;
                break;
            case eLoopClose:
                funcs[i] = &&gLoopClose;
                i++;
                break;
            case ePoint:
                funcs[i] = &&gPoint;
                break;
            case eClear:
                funcs[i] = &&gClear;
                break;
            case eMoveLoop:
                funcs[i] = &&gMoveLoop;
                break;
            case eExit:
                funcs[i] = &&gExit;
                cont = 0;
                break;
        }
        i++;
    }

    printf("starting now\n");

    #ifdef COUNT
        int counts[eExit] = {0};
    #endif

    goto **currentfunc;

    gAdd:
        currentfunc++;
        #ifdef DEBUG
            printf("add %i\n", *currentfunc);
        #endif
        #ifdef COUNT
            counts[eAdd]++;
        #endif
        *currentcell += (intptr_t)*currentfunc;
        currentfunc++;
        goto **currentfunc;

    gMove:
        currentfunc++;
        #ifdef DEBUG
            printf("move %i\n", *currentfunc);
        #endif
        #ifdef COUNT
            counts[eMove]++;
        #endif
        currentcell += (intptr_t) *currentfunc;
        currentfunc++;
        goto **currentfunc;

    gLoopOpen:
        #ifdef DEBUG
            printf("[ %i\n", *currentcell);
        #endif
        #ifdef COUNT
            counts[eLoopOpen]++;
        #endif
        currentfunc++;

        // {
        // short flag = *currentcell == 0;

        // currentfunc = (intptr_t)(*currentfunc)*flag + (intptr_t)currentfunc*!flag;
        // }
        
        if(*currentcell == 0)
            currentfunc = *currentfunc;
        
        currentfunc++;
        goto **currentfunc;

    gLoopClose:
        currentfunc++;
        #ifdef DEBUG
            printf("] %i %i\n", *currentcell, *currentcell);
        #endif
        #ifdef COUNT
            counts[eLoopClose]++;
        #endif
        
        if(*currentcell != 0)
        {
            currentfunc = *currentfunc;
        }
        currentfunc++;
        goto **currentfunc;
    
    gClear:
        #ifdef DEBUG
            printf("clear %i\n", currentcell-intarray);
        #endif
        #ifdef COUNT
            counts[eClear]++;
        #endif
        // printf("\nclear!!!\n");
        *currentcell = 0;
        currentfunc++;
        goto **currentfunc;

    gMoveLoop:
        #ifdef DEBUG
            printf("[>] %i\n", currentcell-intarray);
        #endif
        #ifdef COUNT
            counts[eMoveLoop]++;
        #endif
        // printf("\nmoveloop!!!!!\n");
        currentfunc++;
        register intptr_t jump = (intptr_t)*currentfunc;
        while( *currentcell != 0)
        {
            currentcell += jump;
        }
        currentfunc++;
        goto **currentfunc;

    gPoint:
        #ifdef DEBUG
            printf("print %c\t %i\n", *currentcell, *currentcell);
        #endif
        #ifdef COUNT
            counts[ePoint]++;
        #endif
        *currentbuff = *currentcell;
        currentbuff++;
        currentfunc++;
        goto **currentfunc;

    gExit:
    
    *currentbuff = '\0';
    printf("%s", buffer);

    #ifdef COUNT

        char * str[] = {"add", "move", "l open", "l close", "point", "clear", "movel"};
        for(int i = 0; i < eExit; i++)
        {
            printf("%s\t%i\n", str[i], counts[i]);
        }
    #endif
}

int intcount = 0;
int intstate = 0;
int movecount = 0;
int movestate = 0;

int colonCount= 0;
void compile (char c)
{
    if(intstate == 1) //+ / -
    {
        if(c == '<' || c == '>' || c == '.' || c == '[' || c == ']')
        {
            if(intcount == 0)
            {

            } else {
                if(c == ']' && pos > 1)
                    if(funcs[pos - 2] == (void*)eLoopOpen)
                    {
                        pos-= 2;
                        funcs[pos] = (void*)eClear;
                        pos++;
                        intcount = 0;
                        intstate = 0;
                        colonCount--;
                        return;
                    }
                funcs[pos] = (void*)eAdd;
                pos++;
                funcs[pos] = (void*)(intptr_t)intcount;
                //printf("\n pos: %i  funcs: %i, add: %i", pos, (int) funcs[pos-1], &add);
                intcount = 0;
                intstate = 0;
                pos++;
            }
        }
    } else intcount = 0;

    if(movestate == 1) // > / <
    {
        if(c == '+' || c == '-' || c == '.' || c == '[' || c == ']')
        {
            if(movecount == 0)
            {
                movestate = 0;
            }else
            {
                //printf("\n");
                //for(int i = -4; i < 0; i++)
                //    printf("%i ", funcs[pos+i]);

                if(c == ']' && pos > 1)
                    if(funcs[pos - 2] == (void*)eLoopOpen)
                    {
                        //printf("  move loop");
                        pos -= 2;
                        funcs[pos] = (void*)eMoveLoop;
                        funcs[pos+1] = (void*)(intptr_t)movecount;
                        pos+= 2;

                        movecount = 0;
                        movestate = 0;
                        colonCount--;
                        return;
                    }

                funcs[pos] = (void*)eMove;

                pos++;
                funcs[pos] = (void*)(intptr_t)movecount;
                movecount = 0;
                movestate = 0;
                pos++;
            }
        }
    }else movecount=0;



    switch(c)
    {

	    case '+':
	        intstate = 1;
	    	intcount++;
	    	break;
	    case '-':
	        intstate = 1;
	    	intcount--;
	    	break;

	    case '>':
	        movestate = 1;
	    	movecount++;
	    	break;
	    case '<':
	        movestate = 1;
	    	movecount--;
	    	break;

	    case '[':
	        // if(funcs[pos -2] == 2 && 1 == 0)
            // {
            //     printf("\n dubble [[");
            //     colonCount++;
            //     colon[colonCount] = pos - 2;
            //     return;
            // }
	        funcs[pos] = (void*)eLoopOpen;
	        //printf("\n pos: %i  funcs: %i, loopOpen: %i", pos, (int) funcs[pos], &loopOpen);
	        colonCount++;
	        colon[colonCount] = pos;
	        pos+=2;
			break;

	    case ']':
	        // if(pos > 2 && funcs[pos -2] == (void*)eLoopClose)
            // {
            //     printf("\n dubble ]]");
            //     pos -= 2;
            // }

	        funcs[pos] = (void*)eLoopClose;
	        pos++;
	        funcs[pos] = colon[colonCount] + 1 + funcs;
	        funcs[colon[colonCount]+1] = pos + funcs;
            // printf("%d == %d?\n", pos-colon[colonCount] + 1, funcs[pos]);
	        colonCount--;
	        pos++;
	        break;

	    case '.':
	        funcs[pos] = (void*)ePoint;
	        pos++;
	        break;
    }

}

void load_file (char * file)
{
    clock_t begin = clock();
    pos = 0;
    FILE *fp;
    fp = fopen(file, "r"); // read mode
    rewind(fp);
    fseek(fp, 0, 2);
    int size = ftell(fp);
    rewind(fp);
    char * file_output  = (char *) malloc(sizeof(char) * size);

    if (fp == NULL)
    {
        printf("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }
    int i = 0;
    char ch;
    while(i < size)
    {
        ch = fgetc(fp);
        compile(ch);
        i++;
    }
    funcs[pos] = (void*)eExit;
    fclose(fp);
    return;
}





