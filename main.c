#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>

#define MAXINT 65535
#define MAXCOLON 65535
#define MAXBUFFER 8000

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


enum {eAdd, eMove, eMul, eLoopOpen, eLoopClose, ePoint, eClear, eMoveLoop, eExit};

// #define DEBUG
// #define COUNT
#define SPECIALS
#define NEXTFUNC
// #define BRANCHCOUNT
// #define DOUBLES
// #define BRANCHLESS



 __attribute__((hot)) void runJIT ()
{
    unsigned char intarray[MAXINT] = {0}; //all variables (array)
    unsigned char buffer[MAXBUFFER];
    void *restrict const*restrict currentfunc = funcs;
    unsigned char * restrict currentcell = intarray;
    unsigned char * restrict currentbuff = buffer;
    
    {
        int r = 0;
        int w = 0;
        int cont = 1;
        intptr_t last = -1;
        while(cont)
        {
            intptr_t tmp = funcs[r];
            switch((long int)funcs[r])
            {
                case eAdd:
                    
                    funcs[w] = &&gAdd;
                    funcs[w+1] = funcs[r+1];
                    #ifdef SPECIALS
                        int counter = 1;
                        if(funcs[r+counter*2] == eMove && last == eLoopOpen)
                        {
                            int totalMove = 0;
                            char condition = 0;
                            while(funcs[r+counter*2] == eAdd || funcs[r+counter*2] == eMove)
                            {
                                if(funcs[r+counter*2] == eMove)
                                    totalMove += (intptr_t)funcs[r+counter*2+1];
                                counter++;
                                if(totalMove == 0 && funcs[r+counter*2] == eLoopClose)
                                    condition = 1;
                            }

                            if(condition)
                            {
                                counter = 1;
                                #ifdef DEBUG
                                    printf(" %i,", funcs[r+1]);
                                #endif
                                int add = (intptr_t)funcs[r+1];

                                w -= 2;
                                
                                while(funcs[r+counter*2] == eAdd || funcs[r+counter*2] == eMove)
                                {
                                    #ifdef DEBUG
                                        printf("%i,", funcs[r+counter*2+1]);
                                    #endif
                                    funcs[w+counter+2] = funcs[r+counter*2+1];
                                    counter++;
                                }
                                counter--;
                                if(counter == 1)
                                {
                                    funcs[w] = &&gMul2;
                                    funcs[w+1] = (void*)(counter);
                                    funcs[w+2] = (void*)add;
                                    r += counter*2 + 2;
                                    w += counter; //overwrite last one
                                }else if(counter == 2)
                                {
                                    funcs[w] = &&gMul4;
                                    funcs[w+1] = (void*)(counter);
                                    funcs[w+2] = (void*)add;
                                    r += counter*2 + 2;
                                    w += counter; //overwrite last one
                                }else
                                {
                                    funcs[w] = &&gMul;
                                    funcs[w+1] = (void*)(counter);
                                    funcs[w+2] = (void*)add;
                                    r += counter*2 + 2;
                                    w += counter; //overwrite last one
                                }
                                #ifdef DEBUG
                                    printf("m ");
                                #endif
                            }
                        }
                        #ifdef DEBUG
                        else
                            printf("+");
                        #endif
                    #else
                        #ifdef DEBUG
                            printf("+");
                        #endif
                    #endif
                    w++;
                    r++;
                    break;
                case eMove:
                    #ifdef DEBUG
                        printf(">");
                    #endif
                    funcs[w] = &&gMove;
                    funcs[w+1] = funcs[r+1];
                    w++;
                    r++;
                    break;
                case eLoopOpen:
                    #ifdef DEBUG
                        printf("[");
                    #endif
                    funcs[w] = &&gLoopOpen;
                    funcs[w+1] = &funcs[((intptr_t)&funcs[r+1]-(intptr_t)funcs)/sizeof(intptr_t)-(r-w)];
                    w++;
                    r++;
                    break;
                case eLoopClose:
                    #ifdef DEBUG
                        printf("]");
                    #endif
                    funcs[w] = &&gLoopClose;
                    funcs[w+1] = &funcs[r+1];
                    w++;
                    r++;
                    break;
                case ePoint:
                    #ifdef DEBUG
                        printf(".");
                    #endif
                    funcs[w] = &&gPoint;
                    break;
                case eClear:
                    #ifdef DEBUG
                        printf("(-)");
                    #endif
                    funcs[w] = &&gClear;
                    break;
                case eMoveLoop:
                    #ifdef DEBUG
                        printf("{>%i}", funcs[r+1]);
                    #endif
                    funcs[w] = &&gMoveLoop;
                    funcs[w+1] = funcs[r+1];
                    w++;
                    r++;
                    break;
                case eExit:
                    #ifdef DEBUG
                        printf("e");
                    #endif
                    funcs[w] = &&gExit;
                    cont = 0;
                    break;
            }
            r++;
            w++;
            last = tmp;
        }
    }

    printf("\n");
    //fix pointers
    for(int i = 0; i < 10000; i++)
    {
        if(funcs[i] == &&gLoopOpen)
        {
            int loops = 1;
            int j = i+1;
            while(loops != 0 && funcs[j] != &&gExit && j < 10000)
            {
                if(funcs[j] == &&gLoopOpen)
                {
                    loops++;
                    // printf("open ");
                }
                if(funcs[j] == &&gLoopClose)
                {
                    // printf("close ");
                    loops--;
                }
                // printf("\tfunc %i\tloop %i\ti %i\n", funcs[j], loops, j);
                j++;
            }

            int k = j;
            while(funcs[k+1] == &&gLoopClose)
            {
                // printf("found double ]] %i\n", (k-j+3)/2);
                k+=2;
            }
            if(loops != 0)
                printf("could not find end of loop\n");
            funcs[i+1] = &funcs[k];
            funcs[j] = &funcs[i+1];
            #ifdef BRANCHLESS
                funcs[i+1] = &funcs[j] - &funcs[i+1];
                funcs[j] = &funcs[i+1] - &funcs[j];
            #endif
        }

        if(funcs[i] == &&gExit)
            break;
    }

    printf("starting now\n");

    #ifdef BRANCHCOUNT
        #ifndef COUNT
            #define COUNT
        #endif
    #endif

    #ifdef COUNT
        int counts[eExit] = {0};
    #endif

    #ifdef BRANCHCOUNT
        int branches[2] = {0};
    #endif

    float startTime = (float)clock()/CLOCKS_PER_SEC;

    #ifdef DOUBLES
        short lastFunc = -1;
    #endif

    goto **currentfunc;

    gAdd:
    {
        #ifdef NEXTFUNC
            const void * restrict const nextfunc = *(currentfunc+2);
        #endif
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
        *currentcell += (intptr_t)*currentfunc;
        currentfunc++;
        #ifdef NEXTFUNC
            goto *nextfunc;
        #else
            goto **currentfunc;
        #endif
    }

    gMove:
    {
        #ifdef NEXTFUNC
            const void * restrict const nextfunc = *(currentfunc+2);
        #endif
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
        currentcell += (intptr_t) *currentfunc;
        currentfunc++;
        #ifdef NEXTFUNC
            goto *nextfunc;
        #else
            goto **currentfunc;
        #endif
    }

    gMul2:
    {
        currentfunc++;
        #ifdef DEBUG
            printf("Mul %i\n", *currentfunc);
        #endif
        #ifdef COUNT
            counts[eMul]++;
        #endif
        #ifdef DOUBLES
            if(lastFunc == eMul)
                printf("double Mul\n");
            lastFunc = eMul;
        #endif

        #ifdef NEXTFUNC
            const void * restrict const nextfunc = *(currentfunc+3+1);
        #endif

        currentfunc++;
        unsigned char multiplier = 0;
        char div = *currentfunc;
        unsigned char cell = *currentcell;
        // multiplier = cell / (-div);
        // if(cell % -div)

        while(cell)
        {
            cell += div;
            multiplier++;
        }
        *currentcell = 0;
        *currentfunc++;

        unsigned char * restrict tmpcell = currentcell;
        void *const* tmpfunc = currentfunc;
        currentfunc += 2;

        tmpcell += (intptr_t)*tmpfunc;
        tmpfunc++;
        *tmpcell += ((intptr_t)*tmpfunc)*multiplier;
        tmpfunc++;       
        
        #ifdef NEXTFUNC
            goto *nextfunc;
        #else
            goto **currentfunc;
        #endif
    }

    gMul4:
    {
        currentfunc++;
        #ifdef DEBUG
            printf("Mul %i\n", *currentfunc);
        #endif
        #ifdef COUNT
            counts[eMul]++;
        #endif
        #ifdef DOUBLES
            if(lastFunc == eMul)
                printf("double Mul\n");
            lastFunc = eMul;
        #endif

        #ifdef NEXTFUNC
            const void * restrict const nextfunc = *(currentfunc+5+1);
        #endif

        currentfunc++;
        unsigned char multiplier = 0;
        char div = *currentfunc;
        unsigned char cell = *currentcell;
        // multiplier = cell / (-div);
        // if(cell % -div)

        while(cell)
        {
            cell += div;
            multiplier++;
        }
        *currentcell = 0;
        *currentfunc++;

        unsigned char * restrict tmpcell = currentcell;
        void *const* tmpfunc = currentfunc;
        currentfunc += 4;

        tmpcell += (intptr_t)*tmpfunc;
        tmpfunc++;
        *tmpcell += ((intptr_t)*tmpfunc)*multiplier;
        tmpfunc++;
        tmpcell += (intptr_t)*tmpfunc;
        tmpfunc++;
        *tmpcell += ((intptr_t)*tmpfunc)*multiplier;
        tmpfunc++;  
        
        #ifdef NEXTFUNC
            goto *nextfunc;
        #else
            goto **currentfunc;
        #endif
    }

    gMul:
    {
        currentfunc++;
        #ifdef DEBUG
            printf("Mul %i\n", *currentfunc);
        #endif
        #ifdef COUNT
            counts[eMul]++;
        #endif
        #ifdef DOUBLES
            if(lastFunc == eMul)
                printf("double Mul\n");
            lastFunc = eMul;
        #endif


        char amount = (intptr_t)*currentfunc;
        #ifdef NEXTFUNC
            const void * restrict const nextfunc = *(currentfunc+amount+1);
        #endif

        currentfunc++;
        unsigned char multiplier = 0;
        unsigned char div = -(intptr_t)*currentfunc;
        unsigned char cell = *currentcell;
        // multiplier = cell / (-div);
        // if(cell % -div)

        while(cell)
        {
            cell -= div;
            multiplier++;
        }
        *currentcell = 0;
        *currentfunc++;
        amount-=1;

        unsigned char * restrict tmpcell = currentcell;
        void *const* tmpfunc = currentfunc;
        currentfunc += amount;

        for(char i = 0; i < amount-1; i+=2)
        {
            tmpcell += (intptr_t)*tmpfunc;
            tmpfunc++;
            *tmpcell += ((intptr_t)*tmpfunc)*multiplier;
            tmpfunc++;       
        }
        #ifdef NEXTFUNC
            goto *nextfunc;
        #else
            goto **currentfunc;
        #endif
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

            currentfunc += (intptr_t)*currentfunc*(!*currentcell);
        }
        #else
        if(!*currentcell)
        {
            currentfunc = *currentfunc;
            #ifdef BRANCHCOUNT
                branches[0]++;
            #endif
        }
        #endif

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
        #ifdef DOUBLES
            // if(lastFunc == eLoopClose)
            //     printf("double LoopClose\n");
            if(lastFunc == eLoopOpen)
                printf("[ to ]\n");

            //normal to be double :(
            lastFunc = eLoopClose;
        #endif
        

        #ifdef BRANCHLESS
        {
            currentfunc += (intptr_t)(*currentfunc)*(*currentcell != 0);
        }
        #else

        void ** nextfunc = *currentfunc;
        if(!*currentcell)
        {
            // currentfunc++;
            #ifdef BRANCHCOUNT
                branches[1]++;
            #endif
            nextfunc = currentfunc;
        }
        #endif

        currentfunc = nextfunc;
        currentfunc++;
        goto **currentfunc;
    
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
        goto **currentfunc;

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
        #ifdef NEXTFUNC
            const void * restrict const nextfunc = *(currentfunc+2);
        #endif
        // printf("\nmoveloop!!!!!\n");
        currentfunc++;
        register intptr_t jump = (intptr_t)*currentfunc;
        while( *currentcell != 0)
        {
            currentcell += jump;
        }
        currentfunc++;
        #ifdef NEXTFUNC
            goto *nextfunc;
        #else
            goto **currentfunc;
        #endif
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
        #ifdef NEXTFUNC
            const void * restrict const nextfunc = *(currentfunc+1);
        #endif
        *currentbuff = *currentcell;
        currentbuff++;
        // printf("%c", cell);
        currentfunc++;
        #ifdef NEXTFUNC
            goto *nextfunc;
        #else
            goto **currentfunc;
        #endif
    }

    gExit:
    
    float endTime = (float)clock()/CLOCKS_PER_SEC;

    *currentbuff = '\0';
    printf("%s\n", buffer);
    // printf("display %i\n", currentbuff-buffer);
    #ifdef COUNT

        char * str[] = {"add", "move", "Mul", "l open", "l close", "point", "clear", "movel"};
        for(int i = 0; i < eExit; i++)
        {
            printf("%s\t%i\n", str[i], counts[i]);
        }
    #endif

    #ifdef BRANCHCOUNT

        printf("branch 1 %f\n", (float)branches[0]/counts[eLoopOpen]*100);
        printf("branch 2 %f\n", (float)branches[1]/counts[eLoopClose]*100);
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
                #endif
                
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
                #ifdef SPECIALS
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
                #endif
                
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
            // int overwrite = 0;
	        // if(pos > 2 && funcs[pos -2] == (void*)eLoopClose)
            // {
            //     // printf("\n dubble ]]");
            //     pos -= 2;
            //     overwrite = 1;
            // }

	        funcs[pos] = (void*)eLoopClose;
	        pos++;
	        // if(!overwrite)
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





