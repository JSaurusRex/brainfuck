#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXINT 1000
#define MAXCOLON 1000
#define MAXBUFFER 50
char * code;
int code_length;

unsigned char * intarray; //all variables (array)
int currentarray; //what variable is currently in use
int * colon; //what the position is of the last colon (array)
int * colonClose;
int lastcolon = 0;
int position;

int show_commands = 0;
int show_values = 0;
int debug = 0;


unsigned char * buffer;
int display = 0;


void (*funcs[200]) ();

void plus1 ()
{
    printf("plus1 ");
    intarray[currentarray]++;
}

void plus2 ()
{
    printf("plus2 ");
    intarray[currentarray]+=2;
}

void plus5()
{
    printf("plus5 ");
    intarray[currentarray]+=5;
}

void min1 ()
{
    printf("min1 ");
    intarray[currentarray]--;
}

void min2 ()
{
    printf("min2 ");
    intarray[currentarray]-=2;
}

void min5()
{
    printf("min5 ");
    intarray[currentarray]-=5;
}

void loopOpen()
{
    printf("loopOpen ");
    lastcolon++;
    if(intarray[currentarray] == 0)
    {
        position = colonClose[lastcolon];
        lastcolon--;
    }
}

void loopClose ()
{
    printf("loopClose ");
    if(intarray[currentarray] != 0)
    {
        position = colon[lastcolon];
    }else lastcolon--;
}

void point ()
{
    addtoBuffer(intarray[currentarray]);
}

void moveRight ()
{
    intarray++;
}

void moveLeft ()
{
    intarray--;
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
    printf("\n length: %i\n", code_length);

    //printf("Starting emulation..\n");
    position = 0;


    while(code[position] != '\0' && position < code_length)
    {
        //run_code();
        (*funcs[position]) ();
        position++;
    }
    displayBuffer();
    printf("\nend of program");
    free(intarray);
    free(colon);
    free(colonClose);
    free(buffer);
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
int pos = 0;
int colonCount= 0;
void compile (char c)
{
    printf("\n%c pos:%i  intcount: %i", c, pos, intcount);
    if(intcount != 0)
    {
        if(c != '-' && c != '+')
        {
            printf("\n");
            if(intcount < 0)
            {
                while(intcount <= -5)
                {
                    printf(" -5");
                    intcount+=5;
                    funcs[pos]=&min5;
                    pos++;
                }
                while(intcount <= -2)
                {
                    printf(" -2");
                    intcount+=2;
                    funcs[pos]=&min2;
                    pos++;
                }
                while(intcount <= -1)
                {
                    printf(" -1");
                    intcount++;
                    funcs[pos]=&min1;
                    pos++;
                }
            }
        if(intcount > 0)
            {
                while(intcount >= 5)
                {
                    printf(" +5");
                    intcount-=5;
                    funcs[pos]=&plus5;
                    pos++;
                }
                while(intcount >= 2)
                {
                    printf(" +2");
                    intcount-=2;
                    funcs[pos]=&plus2;
                    pos++;
                }
                while(intcount >= 1)
                {
                    printf(" +1");
                    intcount--;
                    funcs[pos]=&plus1;
                    pos++;
                }
            }
        }
    }
    if(c == '+') intcount++;
    if(c == '-') intcount--;

    if(c == '>')
    {
        printf(" move right");
        funcs[pos] = &moveLeft;
        pos++;
    }

    if(c == '<')
    {
        printf(" move left");
        funcs[pos] = &moveRight;
        pos++;
    }

    if(c == '[')
    {
        printf(" loop open");
        colonCount++;
        funcs[pos] = &loopOpen;
        colon[colonCount] = pos;
        pos++;
    }

    if(c == ']')
    {
        printf(" loop close");
        funcs[pos] = &loopOpen;
        colonClose[colonCount] = pos;
        colonCount--;
        pos++;
    }

    if(c == '.')
    {
        funcs[pos] = &point;
        pos++;
    }

}

void load_file (char * file)
{
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
        //file_output[i] = ch;
        compile(ch);
        i++;
    }
    fclose(fp);
    printf("\ncompiled");
    //code = file_output;
    code_length = size;
    return;
}





