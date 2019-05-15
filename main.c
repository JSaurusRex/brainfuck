#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXCHAR 5000
char code[MAXCHAR];
int code_length;

int intarray[2000]; //all variables (array)
int currentarray; //what variable is currently in use
int colon[1000]; //what the position is of the last colon (array)
int lastcolon;
int position;

int show_commands = 0;

void run_code();
void load_file (char *File);


int main(int argc, char **argv)
{

    //checks arguments

    for (int i = 1  ; i < argc; ++i)
    {
        //printf("%s ", argv[i]);
        char * temp = argv[i];
        if(strcmp(argv[i], "-l") == 0) load_file(argv[i+1]);
        else if(strcmp(argv[i], "-s") == 0) {show_commands = 1; printf("\nbecause of -s, now showing process\n");}
	else load_file(argv[i]);
        //if(argv[i] == "-l") {loadfile("./text.txt");}
    }
    int isempty = 0;
    for(int i = 0; i < MAXCHAR; i++)
    {
	if(code[i]) isempty=1;
    }
    if(!isempty) {
	printf("error: no file specified");
	return;
    }

    printf("Starting emulation..\n");

    while(code[position] != '\0')
    {
        run_code();
        position++;
    }
    //printf("type anything to continue");
    printf("\n");
    return 0;
}

void run_code()
{
    if(show_commands == 1) printf("%c", code[position]);

    if(code[position] == '<') {
        currentarray--;
        if(currentarray < 0) currentarray = MAXCHAR;
        if(currentarray > MAXCHAR) currentarray = 0;}
    if(code[position] == '>') {
        currentarray--;
        if(currentarray < 0) currentarray = MAXCHAR;
        if(currentarray > MAXCHAR) currentarray = 0;}

    if(code[position] == '+') intarray[currentarray]++;
    if(code[position] == '-') intarray[currentarray]--;

    if(code[position] == '.') printf("%c", intarray[currentarray]);
    if(code[position] == ',') scanf("%c", &intarray[currentarray]);

    if(code[position] == '[') {lastcolon++; colon[lastcolon] == position;}
    if(code[position] == ']') {
        if(intarray[currentarray] <= 0){
            //if the loop gets exited
            lastcolon--;
            if(lastcolon < 0) printf("there is an error in your program, ] is before [");
        }else{
            //if it loops
            position = colon[lastcolon];
            printf("%d", position);
    }}
}

void load_file (char *File)
{
    printf("loading file\n");
    FILE *fp;

    fp = fopen(File, "r");
    if (fp == NULL){
        printf("Could not open file ");
        return;
    }
    while (fgets(code, MAXCHAR, fp) != NULL)
    //printf("%s", code);
    fclose(fp);
    return;
}




