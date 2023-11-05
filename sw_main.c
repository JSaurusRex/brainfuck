#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAXINT 65535
#define MAXCOLON 65535
#define MAXBUFFER 50000

typedef void (*Exec) ();
char * code;
int code_length = 0;

unsigned char intarray[MAXINT]; //all variables (array)
unsigned short int currentarray; //what variable is currently in use
int * colon; //what the position is of the last colon (array)
short int lastcolon = 0;
short int position = 0;

short int show_commands = 0;
short int show_values = 0;
short int debug = 0;
short int optimazation = 0;
short int output = 0;
short int read = 0;


unsigned char buffer[MAXBUFFER];
short int display = 0;

int pos = 0;

//void *((*funcs)());
//Exec  funcs;
short funcs[10000];

void displayBuffer();
void addtoBuffer(char c);
void run_code();
void writeOutput();
void load_file(char * str);
void runJIT ();
//Exec * Funcs;

int main(int argc, char **argv)
{
    colon = (int*) calloc(MAXCOLON, sizeof(int));

    //checks arguments
    int isempty = 0;

    //funcs = (int*) calloc(10000, sizeof(int));



    for (int i = 1  ; i < argc; ++i)
    {
        //printf("%s ", argv[i]);

        if(argv[i][0] == '-'){
            int max = strlen(argv[i]);
            for(int j = 1; j < max; j++)
            {
                if(argv[i][j] == 's') {show_commands = 1; printf("\nshowing process");}
                if(argv[i][j] == 'v') {show_values = 1; printf("\nshowing values");}
                if(argv[i][j] == 'd') {debug = 1; printf("\nDebug mode Active");}
                if(argv[i][j] == 'c') {optimazation = 1; printf("\nOptimization mode Active");}
                if(argv[i][j] == 'o') {output = 1; optimazation = 1; printf("\nOutputting compiled code to: out.obfc");}
                if(argv[i][j] == 'r') {read = 1; optimazation = 1; printf("\nReading compiled file\n");}
            }
        }

        else {load_file(argv[i]); isempty = 1;}
        //if(argv[i] == "-l") {loadfile("./text.txt");}
    }
    if(!isempty) {
        printf("error: no file specified\n");
        optimazation = 1;
        load_file("helloworld.txt");
        //return 0;
    }
    if(optimazation == 0) printf("\n length: %i\n", code_length);
    else
    {
        //printf("\n length: %i\n", pos);
        free(colon);
    }

    // intarray = (unsigned char*) calloc(MAXINT, sizeof(unsigned char));
    // buffer = (unsigned char*) calloc(MAXBUFFER+1, sizeof(unsigned char));
    //printf("starting!");

    //printf("Starting emulation..\n");
    if(output == 1)
    {
        writeOutput();
        printf("\nWrote file\n");
        return 0;
    }
    clock_t begin = clock();
    position = -1;
    if(optimazation == 1)
        runJIT();
    else while(position < code_length)
        {
            run_code();
            position++;
        }
    //printf("\nend of program, time: %f\n", (double) (clock() - begin) / CLOCKS_PER_SEC);
    displayBuffer();
    //free(intarray);
    //free(colon);
    //free(colonClose);
    //free(buffer);
    //printf("type anything to continue");
    //scanf("%i", optimazation);
    return 0;
}

void writeOutput()
{
    FILE * fp;
    /* open the file for writing*/
    fp = fopen ("output.obfc","wb");

    int results = 0;
    /*
    for(int i = 0; i < pos; i++)
    {
        results = fprintf(fp, "%c", funcs[i]);
    }
    */
    fwrite(funcs,pos,1,fp);
    if (results == EOF) {
        printf("\nFailed, to write file");
        // Failed to write do error code here.
    }
    /* close the file*/
    fclose (fp);
    return;
}

unsigned char * currentbuff;

enum {eAdd, eMove, eLoopOpen, eLoopClose, ePoint, eClear, eMoveLoop, eExit};

// #define DEBUG

void runJIT ()
{
    register short currentfunc;
    unsigned char * currentcell;
    currentcell = intarray;
    currentbuff = buffer;
    currentfunc = -1;
    //int *
    //currentfunc = funcs - 1;
    int postmp = pos;
    printf("\n", pos);

    while(currentfunc++ < postmp)
    {

        //(**currentfunc)();
        // position++;
        //addtoBuffer('0' funcs[position]);
        // printf("func: %i, func+1 %i, cell: %i, position: %i should be %i\n", funcs[currentfunc], funcs[currentfunc+1], *currentcell, currentfunc);

        switch(funcs[currentfunc])
        {
            case eAdd: //add
                currentfunc++;
                #ifdef DEBUG
                    printf("add %i\n", funcs[currentfunc]);
                #endif
                *currentcell += funcs[currentfunc];
                break;

            case eMove: //move
                currentfunc++;
                #ifdef DEBUG
                    printf("move %i\n", funcs[currentfunc]);
                #endif
                //currentarray+= funcs[position];
                currentcell += funcs[currentfunc];
                //if(currentcell < intarray) currentcell = intarray + MAXINT;
                break;

            case eLoopOpen: //loop open
                currentfunc++;
                #ifdef DEBUG
                    printf("loopOpen %i\n", funcs[currentfunc]);
                #endif
                if(*currentcell == 0)
                {
                    currentfunc = funcs[currentfunc];
                }
                break;

            case eLoopClose: //loop close
                currentfunc++;
                #ifdef DEBUG
                    printf("loopClose %i\n", funcs[currentfunc]);
                #endif
                if(*currentcell != 0)
                {
                    currentfunc = funcs[currentfunc];
                }
                break;

            case ePoint: //point
                #ifdef DEBUG
                    printf("point %i\n", funcs[currentfunc]);
                #endif
                buffer[display] = *currentcell;
                display++;
                printf("%c", *currentcell);
                break;

            case eClear: //clear
                #ifdef DEBUG
                    printf("clear\n", funcs[currentfunc]);
                #endif
                *currentcell = 0;
                // currentfunc++;
                break;
            case eMoveLoop: //move loop
                currentfunc++;
                #ifdef DEBUG
                    printf("loop %i\n", funcs[currentfunc]);
                #endif
                while( *currentcell != 0)
                {
                    currentcell += funcs[currentfunc];
                }
                break;
        }
    }
        //display = currentbuff - buffer;

}


void run_code()
{
    if(show_commands == 1) addtoBuffer(code[position]);

    if(code[position] == '<') {
        currentarray--;
        if(currentarray < 0) currentarray = MAXCOLON;
        if(debug == 1) printf("|%i|", currentarray);
        return;}
    if(code[position] == '>') {
        currentarray++;
        if(currentarray > MAXCOLON) currentarray = 0;
        if(debug == 1) printf("|%i|", currentarray);
        return;}

    if(code[position] == '+') {
        while(code[position] == '+')
        {
            intarray[currentarray]++;
            position++;
        }
        position--;
        return;}
    if(code[position] == '-'){
        while(code[position] == '-')
        {
            intarray[currentarray]--;
            position++;
        }
        position--;
        return;}

    if(code[position] == '.') {

        addtoBuffer(intarray[currentarray]);
        if(show_values == 1) printf("[%i]", intarray[currentarray]);

        return;
    }
    if(code[position] == ',') {displayBuffer(); scanf("%c",&intarray[currentarray]); return;}

    if(code[position] == '[') {
        if(intarray[currentarray] == 0)
        {
            int tmp = 1;
            while(tmp != 0)
            {
                position++;
                if(code[position] == '[') tmp++;
                if(code[position] == ']') tmp--;
            }

        }else {
            lastcolon++;
            colon[lastcolon] = position;
        }
        return;
    }
    if(code[position] == ']') {
        if(intarray[currentarray] == 0){
            //if the loop gets exited
            lastcolon--;
            if(lastcolon < 0) printf("\nthere is an error in your program, ] is before [\n");
            return;
        }else{
            //if it loops
            position = colon[lastcolon];
    }}
}

void addStrToBuffer (char * c)
{
    int len = strlen(c);
    for(int i = 0; i < len; i++)
    {
        addtoBuffer(c[i]);
    }
}

void addtoBuffer (char c)
{
    buffer[display] = c;
    display++;
    if(MAXBUFFER < display) displayBuffer();
}

void displayBuffer()
{
    if(display == 0) return;
    buffer[display+1] = '\0';
    printf("%s", buffer);
    display = 0;
}

int intcount = 0;
int intstate = 0;
int movecount = 0;
int movestate = 0;

int colonCount= 0;
void compile (char c)
{
    //printf("\ncompiling %c, %i ints: %i moves: %i ", c, pos, intcount, movecount);

    if(intstate == 1) //+ / -
    {
        if(c == '<' || c == '>' || c == '.' || c == '[' || c == ']')
        {
            if(intcount == 0)
            {

            } else {
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
                //printf("\n");
                //for(int i = -4; i < 0; i++)
                //    printf("%i ", funcs[pos+i]);

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
	        if(funcs[pos -2] == 2 && 1 == 0)
            {
                printf("\n dubble [[");
                colonCount++;
                colon[colonCount] = pos - 2;
                return;
            }
	        funcs[pos] = eLoopOpen;
	        //printf("\n pos: %i  funcs: %i, loopOpen: %i", pos, (int) funcs[pos], &loopOpen);
	        colonCount++;
	        colon[colonCount] = pos;
	        pos+=2;
			break;

	    case ']':
	        if(funcs[pos -2] == 3 && 1 == 0)
            {
                printf("\n dubble ]]");
                colonCount--;
                return;
            }

	        funcs[pos] = eLoopClose;
	        pos++;
	        funcs[pos] = colon[colonCount] +1;
	        funcs[colon[colonCount]+1] = pos;
	        colonCount--;
	        pos++;
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
        if(read == 1)
        {
            printf(" %i ", ch);
            funcs[i] = ch;
        } else {
            if(optimazation == 0) file_output[i] = ch;
            else compile(ch);
        }
        i++;
    }
    if(optimazation)
    {
        funcs[pos] = eExit;
    }
    fclose(fp);
    if(optimazation == 1 && read == 0)
    {
        //printf("\ncompiled in %f", (double) (clock() - begin) / CLOCKS_PER_SEC);
    }
    else {
        code = file_output;
        code_length = size;
    }
    if(read == 1) pos = size;
    return;
}





