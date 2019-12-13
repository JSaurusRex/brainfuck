#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAXINT 1000
#define MAXCOLON 65535
#define MAXBUFFER 5000
char * code;
int code_length = 0;

unsigned char * intarray; //all variables (array)
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


unsigned char * buffer;
short int display = 0;

short int pos = 0;

int funcs[9000];

void displayBuffer();
void addtoBuffer(char c);
void run_code();
void writeOutput();
void load_file(char * str);

int main(int argc, char **argv)
{
    colon = (int*) calloc(MAXCOLON, sizeof(int));

    //checks arguments
    int isempty = 0;

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
        //load_file("helloworld.txt");
        return 0;
    }
    if(optimazation == 0)printf("\n length: %i\n", code_length);
    else
    {
        printf("\n length: %i\n", pos);
        free(colon);
    }
    intarray = (unsigned char*) calloc(MAXINT, sizeof(unsigned char));
    buffer = (unsigned char*) calloc(MAXBUFFER+1, sizeof(unsigned char));

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
        while(position < pos)
        {
            //(*funcs[position])();
            position++;
            switch(funcs[position])
            {
                case 0: //add
                    position++;
                    intarray[currentarray] += funcs[position];
                    break;

                case 1: //move
                    position++;
                    currentarray+= funcs[position];
                    //if(currentarray >= MAXCOLON) currentarray -= MAXCOLON;
                    //else if(currentarray < 0) currentarray+= MAXCOLON;
                    break;

                case 2: //loop open
                    position++;
                    //printf("loopO ");
                    if(intarray[currentarray] == 0)
                    {
                        position = funcs[position];
                    }
                    break;

                case 3: //loop close
                    position++;
                    //printf("loopC ");
                    if(intarray[currentarray] != 0)
                    {
                        position = funcs[position];
                    }
                    break;

                case 4: //point
                    addtoBuffer(intarray[currentarray]);
                    break;

                case 5: //clear
                    printf("\npos: %i  cel: %i", position, currentarray);
                    intarray[currentarray] = 0;
                    break;
                default:
                    printf("\nredundent %i  %i", position, funcs[position]);
                    break;
            }
            //if(show_commands == 1)
            //printf("   int: %i   position: %i    cel: %i\n", intarray[currentarray], position, currentarray);

        }
    else while(position < code_length)
        {
            run_code();
            position++;
        }

    displayBuffer();
    printf("\nend of program, time: %f\n", (double) (clock() - begin) / CLOCKS_PER_SEC);
    //free(intarray);
    //free(colon);
    //free(colonClose);
    //free(buffer);
    //printf("type anything to continue");
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
int movecount = 0;

int colonCount= 0;
int colonCloseCount = 0;
int colondeep = 0;
char lastchar[3];
void compile (char c)
{
    lastchar[2] = lastchar[1];
    lastchar[1] = lastchar[0];
    lastchar[0] = c;




    if(lastchar[2] == '[' && lastchar[1] == '-'&& c == ']')
    {
        pos-= 2;
        funcs[pos] = 5;
        pos++;
        intcount = 0;
    }
    if(intcount != 0)
    {
        if(c != '-' && c != '+')
        {
            funcs[pos] = 0;
            pos++;
            funcs[pos] = intcount;
            intcount = 0;
            pos++;
        }
    }
    if(c == '+') intcount++;
    if(c == '-') intcount--;

    if(movecount != 0)
    {
        if(c != '<' && c != '>')
        {
            funcs[pos] = 1;
            pos++;
            funcs[pos] = movecount;
            movecount = 0;
            pos++;
        }
    }

    if(c == '>') movecount++;
    if(c == '<') movecount--;

    if(c == '[')
    {
        //printf(" loop open");
        funcs[pos] = 2;
        colonCount++;
        colon[colonCount] = pos;
        //printf(" (colon: %i, pos %i)", colonCount, pos);
        pos+=2;
    }

    if(c == ']')
    {
        //printf(" loop close");
        funcs[pos] = 3;
        funcs[pos+1] = colon[colonCount];
        funcs[colon[colonCount]+1] = pos;
        colonCount--;
        pos+= 2;
    }

    if(c == '.')
    {
    	//printf(" point");
        funcs[pos] = 4;
        pos++;
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
    fclose(fp);
    if(optimazation == 1 && read == 0)
    {
        printf("\ncompiled in %f", (double) (clock() - begin) / CLOCKS_PER_SEC);
    }
    else {
        code = file_output;
        code_length = size;
    }
    if(read == 1) pos = size;
    return;
}





