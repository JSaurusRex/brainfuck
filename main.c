#include <stdio.h>
#include <stdlib.h>


char * code;
int code_length;

int intarray[1000]; //all variables (array)
int currentarray; //what variable is currently in use
int colon[1000]; //what the position is of the last colon (array)
int lastcolon;
int position;

void run_code();
char * load_file (char *File);


int main(int argc, char **argv)
{

    //checks arguments

    for (int i = 1  ; i < argc; ++i)
    {
        printf("%s ", argv[i]);
        //if(argv[i] == "-l") {loadfile(argv[i+1]);}
        //if(argv[i] == "-l") {loadfile("./text.txt");}
    }

    code = load_file("./text.txt");


    printf("\n\nStarting emulation..\n");

    while(code[position] != '!')
    {
        run_code();
        position++;
    }


    return 0;
}

void run_code()
{
    printf("%c", code[position]);
    if(code[position] == '+') intarray[currentarray]++;
    if(code[position] == '-') intarray[currentarray]--;

    if(code[position] == '.') printf("%i", intarray[currentarray]);
    if(code[position] == ',') scanf("%i", intarray[currentarray]);

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

char * load_file (char *File)
{
    printf("loading file\n");
    //char * buffer = "";
    //free(buffer);
    char * buffer = NULL;
    size_t size = 0;

    /* Open your_file in read-only mode */
    FILE *fp = NULL;
    fp = fopen(File, "rb");

    /* Get the buffer size */
    fseek(fp, 0, SEEK_END); /* Go to end of file */
    size = ftell(fp); /* How many bytes did we pass ? */

    /* Set position of stream to the beginning */
    rewind(fp);

    /* Allocate the buffer (no need to initialize it with calloc) */
    buffer = malloc((size + 1) * sizeof(*buffer)); /* size + 1 byte for the \0 */

    /* Read the file into the buffer */
    fread(buffer, size, 1, fp); /* Read 1 chunk of size bytes from fp into buffer */


    /* NULL-terminate the buffer */
    buffer[size] = '\0';
    fclose(fp);

    //printf("%s\n", buffer);

    return buffer;
}




