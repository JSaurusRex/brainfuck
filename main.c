#include <stdio.h>
#include <stdlib.h>

#define MAXCHAR 1000
char code[MAXCHAR];
int code_length;

int intarray[1000]; //all variables (array)
int currentarray; //what variable is currently in use
int colon[1000]; //what the position is of the last colon (array)
int lastcolon;
int position;

void run_code();
void load_file (char *File);


int main(int argc, char **argv)
{

    //checks arguments

    for (int i = 1  ; i < argc; ++i)
    {
        printf("%s ", argv[i]);
        //if(argv[i] == "-l") {loadfile(argv[i+1]);}
        //if(argv[i] == "-l") {loadfile("./text.txt");}
    }

    load_file("./text.txt");


    printf("\n\nStarting emulation..\n");

    while(code[position] != '\0')
    {
        run_code();
        position++;
    }


    return 0;
}

void run_code()
{
    printf("%c", code[position]);

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
        if(intarray[currentarray] == 0){
            //if the loop gets exited
            lastcolon--;
        }else{
            //if it loops
            position = colon[lastcolon];
    }}
}

void load_file (char *File)
{
    printf("loading file\n");
    FILE *fp;

    fp = fopen(File, "r");
    if (fp == NULL){
        printf("Could not open file %s",File);
        return 1;
    }
    while (fgets(code, MAXCHAR, fp) != NULL)
        printf("%s", code);
    fclose(fp);
    return;
}




