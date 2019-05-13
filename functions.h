
char * loadfile (char * str) {
    FILE * fp;
    long lSize;
    char *buffer;
    fp = fopen ( str , "rb" );
    if( !fp ) printf("%s, does not exist. or there is an error", str);
    //opens file

    fseek( fp , 0L , SEEK_END);
    lSize = ftell( fp );
    rewind( fp );
    //gets size


    buffer = calloc( 1, lSize+1 );
    if( !buffer ) fclose(fp),fputs("memory alloc fails",stderr),exit(1);
    //allocates memory for file in ram


    fclose(fp);



    return buffer;
}
