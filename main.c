#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAXINT 65535
#define MAXCOLON 65535
#define MAXBUFFER 15000
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
        while(position++ < pos)
        {
            //(*funcs[position])();
            //position++;
            //addtoBuffer('0' funcs[position]);
            //printf("%i", funcs[position]);
            switch(funcs[position])
            {
                case 0: //add
                    position++;
                    intarray[currentarray] += funcs[position];
                    break;

                case 1: //move
                    position++;
                    currentarray+= funcs[position];
                    break;

                case 2: //loop open
                    position++;
                    if(intarray[currentarray] == 0)
                    {
                        position = funcs[position];
                    }
                    break;

                case 3: //loop close
                    position++;
                    if(intarray[currentarray] != 0)
                    {
                        position = funcs[position];
                    }
                    break;

                case 4: //point
                    buffer[display] = intarray[currentarray];
				    display++;
				    if(MAXBUFFER < display) displayBuffer();
				    position++;
                    break;

                case 5: //clear
                    intarray[currentarray] = 0;
                    position++;
                    break;
                case 6: //move loop
                    position++;
                    while( intarray[currentarray] != 0)
                    {
                        currentarray += funcs[position];
                    }
                    break;
            }

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
                if(c == ']' && funcs[pos - 2] == 2)
                {
                    //printf("\nclear");
                    pos-= 2;
                    funcs[pos] = 5;
                    pos+=2;
                    intcount = 0;
                    intstate = 0;
                    colonCount--;
                    return;
                }
                funcs[pos] = 0;
                pos++;
                funcs[pos] = intcount;
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
                if(c == ']' && funcs[pos - 2] == 2)
                {
                    //printf("  move loop");
                    pos -= 2;
                    funcs[pos] = 6;
                    funcs[pos+1] = movecount;
                    pos+= 2;

                    movecount = 0;
                    movestate = 0;
                    colonCount--;
                    return;
                }
                funcs[pos] = 1;
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
	        if(funcs[pos -2] == 2)
            {
                printf("\n dubble [[");
                colonCount++;
                colon[colonCount] = pos - 2;
                return;
            }
	        funcs[pos] = 2;
	        colonCount++;
	        colon[colonCount] = pos;
	        pos+=2;
			break;

	    case ']':
	        if(funcs[pos -2] == 3 )
            {
                printf("\n dubble ]]");
                colonCount--;
                return;
            }
	        funcs[pos] = 3;
	        funcs[pos+1] = colon[colonCount] +1;
	        funcs[colon[colonCount]+1] = pos+1;
	        colonCount--;
	        pos+= 2;
	        break;

	    case '.':
	        funcs[pos] = 4;
	        pos+=2;
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





