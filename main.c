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

char funcs[10000];

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


enum {eAdd, eMove, eMap, eLoopOpen, eLoopClose, ePoint, eClear, eMoveLoop, eExit};

// #define DEBUG
// #define COUNT
// #define SPECIALS
// #define DOUBLES
// #define BRANCHLESS



 __attribute__((hot)) void runJIT ()
{
    unsigned char intarray[MAXINT]; //all variables (array)
    unsigned char buffer[MAXBUFFER];
    register char *restrict currentfunc = funcs;
    register unsigned char * restrict currentcell = intarray;
    unsigned char * currentbuff = buffer;

    #if defined(DEBUG) ||defined(COUNT)
        char * str[] = {"add", "move", "map", "l open", "l close", "point", "clear", "movel"};
    #endif

    {
        int r = 0;
        int w = 0;
        int cont = 1;
        while(cont)
        {
            switch((long int)funcs[r])
            {
                case eAdd:
                    
                    funcs[w] = eAdd;
                    funcs[w+1] = funcs[r+1];
                    #ifdef SPECIALS
                        int counter = 1;
                        if(funcs[r+counter*2] == eMove)
                        {
                            #ifdef DEBUG
                                printf(" %i,", funcs[r+1]);
                            #endif
                            int add = funcs[r+1];
                            while(funcs[r+counter*2] == eAdd || funcs[r+counter*2] == eMove)
                            {
                                #ifdef DEBUG
                                    printf("%i,", funcs[r+counter*2+1]);
                                #endif
                                funcs[w+counter+2] = funcs[r+counter*2+1];
                                counter++;
                            }
                            counter--;
                            funcs[w] = eMap;
                            funcs[w+1] = counter+1;
                            funcs[w+2] = add;
                            r += counter*2;
                            w += counter+1;
                            #ifdef DEBUG
                                printf("m ");
                            #endif
                        }
                        #ifdef DEBUG
                        else
                            printf("+%i ", funcs[r+1]);
                        #endif
                    #else
                        #ifdef DEBUG
                            printf("+%i ", funcs[r+1]);
                        #endif
                    #endif
                    w++;
                    r++;
                    break;
                case eMove:
                    #ifdef DEBUG
                        printf(">%i ", funcs[r+1]);
                    #endif
                    funcs[w] = eMove;
                    funcs[w+1] = funcs[r+1];
                    w++;
                    r++;
                    break;
                case eLoopOpen:
                    #ifdef DEBUG
                        printf("[%i ", funcs[r+1]);
                    #endif
                    funcs[w] = eLoopOpen;
                    // funcs[w+1] = &funcs[(&funcs[r+1]-funcs)/sizeof-(r-w)];
                    w+=2;
                    r+=2;
                    break;
                case eLoopClose:
                    #ifdef DEBUG
                        printf("]%i ", funcs[r+1]);
                    #endif
                    funcs[w] = eLoopClose;
                    // funcs[w+1] = &funcs[r+1];
                    w+=2;
                    r+=2;
                    break;
                case ePoint:
                    #ifdef DEBUG
                        printf(". ");
                    #endif
                    funcs[w] = ePoint;
                    break;
                case eClear:
                    #ifdef DEBUG
                        printf("(-) ");
                    #endif
                    funcs[w] = eClear;
                    break;
                case eMoveLoop:
                    #ifdef DEBUG
                        printf("{>%i} ", funcs[r+1]);
                    #endif
                    funcs[w] = eMoveLoop;
                    funcs[w+1] = funcs[r+1];
                    w++;
                    r++;
                    break;
                case eExit:
                    #ifdef DEBUG
                        printf("e%i", w);
                    #endif
                    funcs[w] = eExit;
                    printf("r %i\n", r);
                    cont = 0;
                    break;
            }
            r++;
            w++;
        }
    }

    printf("\n");

    int colonIndex = 0;
    //fix pointers
    for(int i = 0; i < 10000; i++)
    {
        // printf("%i| %s : %i\n", i, str[funcs[i]], funcs[i+1]);
        int cont = 1;
        switch(funcs[i])
        {
            case eAdd:
            case eMove:
            case eMoveLoop:
                i++;
                break;

            case ePoint:
            case eClear:
                break;

            case eMap:
                int add = funcs[i+1]+1;
                i += add;
                // printf("map : %i  \ti : %i\n", add, i);
                break;
                        
            case eLoopOpen:
                colon[colonIndex] = i+1;
                colonIndex++;
                i+=2;
                // printf("colon found %i\n", i-1);
                break;

            case eLoopClose:
                colonIndex--;
                int j = colon[colonIndex];
                // printf("colon %i [%i]\t%d\n", j, colonIndex, &funcs[i+1]-&funcs[j]);
                // if(abs(&funcs[j] - &funcs[i+1]) > 255)
                    // printf("\n\noh oh %i\n\n", abs(&funcs[j] - &funcs[i+1]));
                
                // short * ptr = 
                *((short*)(&funcs[j])) = &funcs[i+1] - &funcs[j];
                *((short*)(&funcs[i+1])) = &funcs[j] - &funcs[i+1];
                // printf("what it should be %i, what it is %i\n", *((short*)(&funcs[j])), &funcs[i+1] - &funcs[j]);
                // funcs[j] = &funcs[i+1] - &funcs[j];
                // funcs[i+1] = &funcs[j] - &funcs[i+1];
                i+=2;
                break;

            case eExit:
                cont = 0;
                printf("exit\n");
                break;
        }

        if(!cont)
            break;
    }

    printf("starting now\n");

    #ifdef COUNT
        int counts[eExit] = {0};
    #endif

    float startTime = (float)clock()/CLOCKS_PER_SEC;

    #ifdef DOUBLES
        short lastFunc = -1;
    #endif

    // printf("%d\n", &&gAdd-&&gExit);

    const void * const lookup[] = {&&gAdd, &&gMove, &&gMap, &&gLoopOpen, &&gLoopClose, &&gPoint, &&gClear, &&gMoveLoop, &&gExit};

    goto *lookup[*currentfunc];

    gAdd:
    {
        currentfunc++;
        #ifdef DEBUG
            printf("add %i\n", *currentfunc);
        #endif
        #ifdef COUNT
            counts[eAdd]++;
        #endif
        #ifdef DOUBLES
            if(lastFunc == eAdd)
                printf("double Add\n");
            lastFunc = eAdd;
        #endif
        *currentcell += *currentfunc;
        currentfunc++;
        goto *lookup[*currentfunc];
    }

    gMove:
    {
        currentfunc++;
        #ifdef DEBUG
            printf("move %i\n", *currentfunc);
        #endif
        #ifdef COUNT
            counts[eMove]++;
        #endif
        #ifdef DOUBLES
            if(lastFunc == eMove)
                printf("double Move\n");
            lastFunc = eMove;
        #endif
        currentcell += *currentfunc;
        currentfunc++;
        goto *lookup[*currentfunc];
    }

    gMap:
    {
        currentfunc++;
        #ifdef DEBUG
            printf("map %i\n", *currentfunc);
        #endif
        #ifdef COUNT
            counts[eMap]++;
        #endif
        #ifdef DOUBLES
            if(lastFunc == eMap)
                printf("double Map\n");
            lastFunc = eMap;
        #endif
        short amount = *currentfunc;
        currentfunc++;
        for(short i = 0; i < amount; i++)
        {
            *currentcell += *currentfunc;
            currentfunc++;

            i++;
            if(i >= amount)
                break;
            //move
            currentcell += *currentfunc;
            currentfunc++;
        }
        // printf("currentfunc %i\t%i\n", currentfunc, &runJIT);
        goto *lookup[*currentfunc];
    }

    gLoopOpen:
        #ifdef DEBUG
            printf("[ %i\n", *currentcell);
        #endif
        #ifdef COUNT
            counts[eLoopOpen]++;
        #endif
        #ifdef DOUBLES
            if(lastFunc == eLoopOpen)
                printf("double LoopOpen\n");
            lastFunc = eLoopOpen;
        #endif
        currentfunc++;

        #ifdef BRANCHLESS
        {
            // short flag = *currentcell == 0;

            currentfunc += *currentfunc*(!*currentcell);
        }
        #else
        if(!*currentcell)
            currentfunc += *((short*)currentfunc);
        #endif

        currentfunc+=2;
        goto *lookup[*currentfunc];

    gLoopClose:
        currentfunc++;
        #ifdef DEBUG
            printf("] %i %i\n", *currentcell, *currentcell);
        #endif
        #ifdef COUNT
            counts[eLoopClose]++;
        #endif
        #ifdef DOUBLES
            // if(lastFunc == eLoopClose)
            //     printf("double LoopClose\n");

            //normal to be double :(
            lastFunc = eLoopClose;
        #endif
        

        #ifdef BRANCHLESS
        {
            currentfunc += (*currentfunc)*(*currentcell != 0);
        }
        #else
        if(*currentcell)
            currentfunc += *(short*)currentfunc;
        #endif

        currentfunc+=2;
        goto *lookup[*currentfunc];
    
    gClear:
        #ifdef DEBUG
            printf("clear %i\n", currentcell-intarray);
        #endif
        #ifdef COUNT
            counts[eClear]++;
        #endif
        #ifdef DOUBLES
            if(lastFunc == eClear)
                printf("double Clear\n");
            lastFunc = eClear;
        #endif
        // printf("\nclear!!!\n");
        currentfunc++;
        *currentcell = 0;
        goto *lookup[*currentfunc];

    gMoveLoop:
    {
        #ifdef DEBUG
            printf("[>] %i\n", currentcell-intarray);
        #endif
        #ifdef COUNT
            counts[eMoveLoop]++;
        #endif
        #ifdef DOUBLES
            if(lastFunc == eMoveLoop)
                printf("double MoveLoop\n");
            lastFunc = eMoveLoop;
        #endif
        // printf("\nmoveloop!!!!!\n");
        currentfunc++;
        register char jump = *currentfunc;
        while( *currentcell != 0)
        {
            currentcell += jump;
        }
        currentfunc++;
        goto *lookup[*currentfunc];
    }

    gPoint:
    {
        #ifdef DEBUG
            printf("print %c\t %i\n", *currentcell, *currentcell);
        #endif
        #ifdef COUNT
            counts[ePoint]++;
        #endif
        #ifdef DOUBLES
            if(lastFunc == ePoint)
                printf("double Point\n");
            lastFunc = ePoint;
        #endif
        *currentbuff = *currentcell;
        currentbuff++;
        // printf("%c", cell);
        currentfunc++;
        goto *lookup[*currentfunc];
    }

    gExit:
    
    float endTime = (float)clock()/CLOCKS_PER_SEC;

    *currentbuff = '\0';
    printf("%s", buffer);

    #ifdef COUNT
        for(int i = 0; i < eExit; i++)
        {
            printf("%s\t%i\n", str[i], counts[i]);
        }
    #endif

    printf("\ntime %.3f\n", endTime-startTime);
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
                #ifdef SPECIALS
                    if(c == ']' && pos > 1)
                        if(funcs[pos - 2] == eLoopOpen)
                        {
                            pos-= 2;
                            funcs[pos] = eClear;
                            pos++;
                            intcount = 0;
                            intstate = 0;
                            colonCount--;
                            return;
                        }
                #endif
                
                funcs[pos] = eAdd;
                pos++;
                funcs[pos] = intcount;
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
                #ifdef SPECIALS
                    if(c == ']' && pos > 1)
                        if(funcs[pos - 2] == eLoopOpen)
                        {
                            //printf("  move loop");
                            pos -= 2;
                            funcs[pos] = eMoveLoop;
                            funcs[pos+1] = movecount;
                            pos+= 2;

                            movecount = 0;
                            movestate = 0;
                            colonCount--;
                            return;
                        }
                #endif
                
                funcs[pos] = eMove;
                pos++;
                funcs[pos] = movecount;
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
	        funcs[pos] = eLoopOpen;
	        //printf("\n pos: %i  funcs: %i, loopOpen: %i", pos, (int) funcs[pos], &loopOpen);
	        // colonCount++;
	        // colon[colonCount] = pos;
	        pos+=3;
			break;

	    case ']':
            // int overwrite = 0;
	        // if(pos > 2 && funcs[pos -2] == eLoopClose)
            // {
            //     // printf("\n dubble ]]");
            //     pos -= 2;
            //     overwrite = 1;
            // }

	        funcs[pos] = eLoopClose;
	        // pos++;
	        // if(!overwrite)
            // funcs[pos] = colon[colonCount] + 1 + funcs;
	        // funcs[colon[colonCount]+1] = pos + funcs;
            // printf("%d == %d?\n", pos-colon[colonCount] + 1, funcs[pos]);
	        // colonCount--;
	        pos+=3;
	        break;

	    case '.':
	        funcs[pos] = ePoint;
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
    funcs[pos] = eExit;
    fclose(fp);
    return;
}





