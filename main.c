#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXCHAR 5000
#define MAXINT 2000
#define MAXCOLON 1000
char * code;
int code_length;

char * intarray; //all variables (array)
int currentarray; //what variable is currently in use
int * colon; //what the position is of the last colon (array)
int lastcolon = 0;
int position;

int show_commands = 0;
int show_values = 0;
void run_code();
void load_file (const char *File);


int main(int argc, char **argv)
{
    code = malloc(sizeof(char)*MAXCHAR);
    intarray = (char*) calloc(MAXINT, sizeof(char));
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
                if(argv[i][j] == 's') {show_commands = 1; printf("\nBecayse of -s, now showing process");}
                if(argv[i][j] == 'v') {show_values = 1; printf("\nBecayse of -v, now showing values");}

            }
        }

	else {load_file(argv[i]); isempty = 1;}
        //if(argv[i] == "-l") {loadfile("./text.txt");}
    }
    if(!isempty) {
        printf("error: no file specified\n");
        load_file("helloworld.txt");
        //return 0;
    }

    //printf("Starting emulation..\n");
    position = 0;

    while(code[position] != '\0' && position < MAXCHAR)
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
        if(currentarray < 0) currentarray = MAXCOLON;
        if(show_values == 1) printf("|%i|", currentarray);
        return;}
    if(code[position] == '>') {
        currentarray++;
        if(currentarray > MAXCHAR) currentarray = 0;
        if(show_values == 1) printf("|%i|", currentarray);
        return;}

    if(code[position] == '+') {intarray[currentarray]++;return;}
    if(code[position] == '-') {intarray[currentarray]--; if(intarray[currentarray] < (char) 0) intarray[currentarray]=(char) 255; return;}


    if(code[position] == '.') {printf("%c", intarray[currentarray]); if(show_values == 1) printf("(%i)", intarray[currentarray]); return;}
    if(code[position] == ',') {scanf("%c",&intarray[currentarray]); return;}

    if(code[position] == '[') {lastcolon++; colon[lastcolon] == position; return;}
    if(code[position] == ']') {
        if(intarray[currentarray] <= 0){
            //if the loop gets exited
            lastcolon--;
            if(lastcolon < 0) printf("\nthere is an error in your program, ] is before [\n");
            return;
        }else{
            //if it loops
            position = colon[lastcolon];
            //printf("%d", position);
    }}
}

void load_file (const char *File)
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




