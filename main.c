#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAXINT 1000
#define MAXCOLON 1000
#define MAXBUFFER 50
char * code;
int code_length = 0;

unsigned char * intarray; //all variables (array)
int currentarray; //what variable is currently in use
int * colon; //what the position is of the last colon (array)
int * colonClose;
int lastcolon = 0;
int lastcolonClose = 0;
int position = 0;

short int show_commands = 0;
short int show_values = 0;
short int debug = 0;
short int optimazation = 0;



unsigned char * buffer;
int display = 0;

int compileLength;
int pos = 0;


void (*funcs[20000]) ();

void plus()
{
    if(show_commands == 1) printf("plus ");
    position++;
    intarray[currentarray] += funcs[position];
}

void min ()
{
    if(show_commands == 1) printf("min ");
    position++;
    intarray[currentarray] -= (int) funcs[position];
}

void loopOpen()
{
    if(show_commands == 1) printf("loopOpen ");
    lastcolon++;
    if(intarray[currentarray] == 0)
    {
        position = funcs[position+1];
    }
    position++;
}

void loopClose ()
{
    if(show_commands == 1) printf("loopClose ");
    if(intarray[currentarray] != 0)
    {
    	//loop
        position = funcs[position+1];
    }
    position++;
}

void point ()
{
    addtoBuffer(intarray[currentarray]);
    //printf("point: (%c), %i ", intarray[currentarray], intarray[currentarray]);
}

void input ()
{
	displayBuffer();
	scanf("%c",&intarray[currentarray]);
}

void moveRight ()
{
	if(show_commands == 1) printf("moveRight ");
	position++;

    currentarray+= funcs[position];
    if(currentarray >= MAXCOLON) currentarray -= MAXCOLON;
}

void moveLeft ()
{
	if(show_commands == 1) printf("moveLeft ");
	position++;
    currentarray -= (int) funcs[position];
    if(currentarray < 0) currentarray += MAXCOLON;
}



int main(int argc, char **argv)
{
    intarray = (unsigned char*) calloc(MAXINT, sizeof(unsigned char));
    colon = (int*) calloc(MAXCOLON, sizeof(int));
    colonClose = (int*) calloc(MAXCOLON, sizeof(int));



    buffer = (unsigned char*) calloc(MAXBUFFER, sizeof(unsigned char));





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
                if(argv[i][j] == 'o') {optimazation = 1; printf("\nOptimization mode Active");}

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
    else printf("\n length: %i\n", pos);

    //printf("Starting emulation..\n");
    clock_t begin = clock();
    if(optimazation == 1)
        while(position < pos)
        {
            (*funcs[position])();
            if(show_commands == 1) printf("   int: %i position: %i colon: %i cel: %i\n", intarray[currentarray], position, lastcolon, currentarray);
            position++;
        }
    else while(position < code_length)
        {
            run_code();
            position++;
        }

    displayBuffer();
    printf("\nend of program, time: %ld\n", (begin - clock()) / CLOCKS_PER_SEC);
    //free(intarray);
    //free(colon);
    //free(colonClose);
    //free(buffer);
    //printf("type anything to continue");
    return 0;
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
    if(MAXBUFFER - 1 < display) displayBuffer();
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
void compile (char c)
{
    //printf("\n%c pos:%i  intcount: %i", c, pos, intcount);
	//printf(" |");
    if(intcount != 0)
    {
        if(c != '-' && c != '+')
        {
            if(intcount < 0)
            {
                funcs[pos] = min;
                pos++;
                funcs[pos] = -intcount;
                intcount = 0;
                pos++;
            }else if (intcount > 0)
            {
                funcs[pos] = plus;
                pos++;
                funcs[pos] = intcount;
                intcount = 0;
                pos++;
            }
        }
    }
    if(c == '+') intcount++;
    if(c == '-') intcount--;

    if(movecount != 0)
    {
        if(c != '<' && c != '>')
        {
            if(movecount < 0)
            {
                funcs[pos] = moveLeft;
                pos++;
                funcs[pos] = -movecount;
                movecount = 0;
                pos++;
            }else if(movecount > 0)
            {
                funcs[pos] = moveRight;
                pos++;
                funcs[pos] = movecount;
                movecount = 0;
                pos++;
            }
        }
    }

    if(c == '>') movecount++;
    if(c == '<') movecount--;

    if(c == '[')
    {
        //printf(" loop open");
        funcs[pos] = loopOpen;
        colonCount++;
        colon[colonCount] = pos;
        //printf(" (colon: %i, pos %i)", colonCount, pos);
        pos+=2;
    }

    if(c == ']')
    {
        //printf(" loop close");
        funcs[pos] = loopClose;
        funcs[pos+1] = colon[colonCount];
        funcs[colon[colonCount]+1] = pos;
        //printf(" (colon: %i, pos %i)", colonCount, pos);
        colonCount--;
        pos+= 2;
    }

    if(c == '.')
    {
    	//printf(" point");
        funcs[pos] = point;
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
        if(optimazation == 0) file_output[i] = ch;
        else compile(ch);
        i++;
    }
    fclose(fp);
    if(optimazation == 1)printf("\ncompiled in %ld", (begin - clock()) / CLOCKS_PER_SEC);
    else {
        code = file_output;
        code_length = size;
    }
    return;
}





