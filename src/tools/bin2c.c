#include <stdio.h>

int main( int argc, char *argv[] )
{
    char *name;
    int c, i;
    unsigned length;

    if( argc >= 1 )
        name = argv[1];
    else
        name = "__blob";

    printf( "const unsigned char %s[] = {", name );
    i = 0;
    length = 0;
    while( EOF != (c=getchar()) )
    {
        if( i == 0 )
            printf( "\n/* %04x */  ", length );

        printf( "0x%02x, ", (unsigned char)c );

        if( ++i == 8 )
            i = 0;

        length++;
    }
    printf( "\n};\n" );
    printf( "const unsigned %s_len = 0x%x;\n", name, length );


    return 0;
}

