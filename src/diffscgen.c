#include <ctype.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "diffscgen.h"

int main( int argc, char *argv[] )
{
    // vars depending on #defines
    char*   inx                 = INX;
    char*   iny                 = INY;
    char    label[128]          = DEFAULT_LBL;
    char*   lda_imm             = LDA_IMM;
    char*   ldx_imm             = LDX_IMM;
    char*   ldy_imm             = LDY_IMM;
    char    outfile_name[128]   = DEFAULT_OUT;
    char    reg[2]              = DEFAULT_REG;
    char*   rts                 = RTS;
    char*   sta_abs             = STA_ABS;
    char*   stx_abs             = STX_ABS;
    char*   sty_abs             = STY_ABS;

    int     intendation         = DEFAULT_ITD;
    int     size_imm            = SIZE_IMM;
    int     size_abs            = SIZE_ABS;

    // vars - init just to be "super-safe" :-P
    char*   diff[256]           = { NULL };
    char    *inc_cmd            = NULL;
    char    *infile_dst_name    = NULL;
    char    *infile_dst_nopath  = NULL;
    char    *infile_src_name    = NULL;
    char    *infile_src_nopath  = NULL;
    char    *load_cmd           = NULL;
    char    *save_string        = NULL;
    char    *store_cmd          = NULL;
    char    *temp_string        = NULL;
    char    *whitespaces        = NULL;

    FILE    *infile_src         = NULL;
    FILE    *infile_dst         = NULL;
    FILE    *outfile            = NULL;

    int     c                   = 0;
    int     i                   = 0;
    int     index_max           = 0;
    int     input_src           = 0;
    int     input_dst           = 0;
    int     offset              = 0;
    int     skipbytes           = 0;
    int     template_index      = 0;

    print_info();

    if ( ( argc == 1 ) ||
         ( strcmp( argv[1], "-h" ) == 0 ) ||
         ( strcmp( argv[1], "-help" ) == 0 ) ||
         ( strcmp( argv[1], "-?" ) == 0 ) ||
         ( strcmp( argv[1], "--help" ) == 0 ) )
    {
        print_help();
        exit( EXIT_SUCCESS );
    }

    // getopt cmdline-argument handler
    opterr = 1;

    while ( ( c = getopt ( argc, argv, "i:l:o:s:r:" ) ) != -1 )
    {
        switch ( c )
        {
            case 'i':
                if ( sscanf( optarg, "%i", &intendation) != 1 )
                {
                    printf( "\nError: -i needs an integer value for string intendation\n" );
                    exit( EXIT_FAILURE );
                }
                break;
            case 'l':
                if ( strlen( optarg ) > 127 )
                {
                    printf( "\nError: -l label parameter too long\n" );
                    exit( EXIT_FAILURE );
                }
                else
                {
                    strcpy( label, optarg );
                }
                break;
            case 'o':
                if ( strlen( optarg ) > 127 )
                {
                    printf( "\nError: -o outfile parameter too long\n" );
                    exit( EXIT_FAILURE );
                }
                else
                {
                    strcpy( outfile_name, optarg);
                }
                break;
            case 's':
                if ( sscanf( optarg, "%i", &skipbytes) != 1 )
                {
                    printf( "\nError: -s needs an integer value for file offset\n" );
                    exit( EXIT_FAILURE );
                }
                break;
            case 'r':
                if ( ( strlen( optarg ) == 1 ) &&
                     ( optarg[0] == 'a' ||
                       optarg[0] == 'x' ||
                       optarg[0] == 'y') )
                {
                    strcpy( reg, optarg );
                }
                else
                {
                    printf( "\nError: valid values for -r are: a, x, y\n" );
                    exit( EXIT_FAILURE );
                }
                break;
        }
    }

    // make sure at least two files are given
    if ( ( optind ) == argc )
    {
        printf( "\nError: no files specified.\n" );
        exit( EXIT_FAILURE );
    }
    else if ( ( optind + 1 ) == argc )
    {
        printf( "\nError: no destination file given.\n" );
        exit( EXIT_FAILURE );
    }

    // create outfile and make sure it's writeable
    outfile = fopen( outfile_name, "w" );
    if ( outfile == NULL )
    {
        printf( "\nError: couldn't create file \"%s\".\n", outfile_name );
        exit( EXIT_FAILURE );
    }
    else
    {
        fclose( outfile );
    }

    // prepare whitespaces string
    whitespaces = malloc( intendation + 1 );
    for ( i = 0; i < intendation; i++ )
    {
        whitespaces[i] = ' ';
    }
    whitespaces[intendation] = '\0';

    // set template index and assembler commands depending on register
    switch ( reg[0] )
    {
        case 'a':
            load_cmd = newstr( lda_imm );
            store_cmd = newstr( sta_abs );
            template_index = 0;
            break;
        case 'x':
            inc_cmd = newstr( inx );
            load_cmd = newstr( ldx_imm );
            store_cmd = newstr( stx_abs );
            template_index = 1;
            break;
        case 'y':
            inc_cmd = newstr( iny );
            load_cmd = newstr( ldy_imm );
            store_cmd = newstr( sty_abs );
            template_index = 2;
            break;
    }

    // mainloop
    do
    {
        infile_src_name = newstr( argv[optind] );
        infile_dst_name = newstr( argv[optind+1] );
        infile_src_nopath = basename( infile_src_name );
        infile_dst_nopath = basename( infile_dst_name );

        printf( "src filename:      %s\n", infile_src_nopath );
        printf( "dst filename:      %s\n", infile_dst_nopath );
        printf( "skip bytes:        %d\n", skipbytes );
        printf( "output filename:   %s\n", outfile_name );
        printf( "label:             %s\n", label );
        printf( "register:          %s\n", reg );

        // open files
        infile_src = fopen( infile_src_name, "r" );
        if ( infile_src == NULL )
        {
            printf( "\nError: couldn't read file \"%s\".\n", infile_src_name );
            exit( EXIT_FAILURE );
        }
        infile_dst = fopen( infile_dst_name, "r" );
        if ( infile_dst == NULL )
        {
            printf( "\nError: couldn't read file \"%s\".\n", infile_dst_name );
            exit( EXIT_FAILURE );
        }
        outfile = fopen( outfile_name, "a" );

        // make acme label/symbol
        temp_string = malloc( strlen( infile_src_nopath ) +
                              strlen( "_to_" ) +
                              strlen( infile_dst_nopath ) +
                              strlen( ":\n" ) +
                              1 );
        sprintf( temp_string, "%s_to_%s:\n", infile_src_nopath, infile_dst_nopath );
        point_to_underscore( temp_string );
        printf( "acme symbol:       %s\n", temp_string );
        fputs( temp_string, outfile );
        free( temp_string );

        // fill diff array with empty strings
        for ( i = 0; i <= 0xFF; i++ )
        {
            diff[i] = newstr( "" );
        }

        // forward infiles according to skipbytes
        fseek( infile_src, skipbytes, 0 );
        fseek( infile_dst, skipbytes, 0 );

        // parse files and write store commands to diff array
        offset = 0;

        while  ( ( input_src = fgetc( infile_src ) ) != EOF )
        {
            input_dst = fgetc( infile_dst );

            if ( input_src != input_dst )
            {
                // store string format "    sta INFILE_SRC+$0000\n"
                temp_string = malloc( strlen( whitespaces ) +
                                      strlen( store_cmd ) +
                                      strlen( " " )+
                                      strlen( label ) +
                                      strlen( "+$" )+
                                      size_abs +
                                      strlen( "\n" ) +
                                      1 );
                sprintf( temp_string, "%s%s %s+$%04x\n", whitespaces, store_cmd, label, offset );

                save_string = newstr( diff[input_dst] );
                free( diff[input_dst] );

                diff[input_dst] = malloc ( strlen( save_string ) + strlen( temp_string ) + 1 );
                sprintf( diff[input_dst], "%s%s", save_string, temp_string );

                free( save_string );
                free( temp_string );
            }
            offset += 1;
        }

        // check array and set index_max
        for ( i = 0; i <= 0xFF; i++ )
        {
            if ( strlen( diff[i] ) != 0 )
            {
                index_max = i;
            }
        }

        // print array to outfile
        for ( i = 0; i <= index_max; i++ )
        {
            if ( strlen( diff[i] ) != 0 )
            {
                if ( ( i == 0 ) ||
                     ( template_index == 0 ) ||
                     ( strlen( diff[i-1] ) == 0 ) )
                {
                    // load string format "    lda #$00\n"
                    temp_string = malloc( strlen( whitespaces ) +
                                          strlen( load_cmd ) +
                                          size_imm +
                                          strlen( "\n" ) +
                                          1 );
                    sprintf( temp_string, "%s%s%02x\n", whitespaces, load_cmd, i );
                }
                else
                {
                    // inc string format "    inx\n"
                    temp_string = malloc( strlen( whitespaces ) +
                                          strlen( inc_cmd ) +
                                          strlen( "\n" ) +
                                          1 );
                    sprintf( temp_string, "%s%s\n", whitespaces, inc_cmd );
                }

                fputs( temp_string, outfile );
                fputs( diff[i], outfile );

                free( temp_string );
            }
        }
        // print rts
        temp_string = malloc( strlen( whitespaces ) +
                              strlen( rts ) +
                              strlen( "\n" ) +
                              1 );
        sprintf( temp_string, "%s%s\n", whitespaces, rts );
        fputs( temp_string, outfile );
        free( temp_string );

        // memory clean up
        for ( i = 0; i <= 0xFF; i++ )
        {
            free( diff[i] );
        }

        free( infile_src_name );
        free( infile_dst_name );

        fclose( infile_src );
        fclose( infile_dst );
        fclose( outfile );
    }
    while ( ++optind < ( argc - 1 ) );

    // final memory clean up
    if ( template_index != 0 )
    {
        free( inc_cmd );
    }
    free( load_cmd );
    free( store_cmd );
    free( whitespaces );

    printf( "done!\n" );

    exit( EXIT_SUCCESS );
}

// string helper functions

char *newstr( char *initial_str )
{
    int num_chars;
    char *new_str;

    num_chars = strlen( initial_str ) + 1;
    new_str = malloc ( num_chars );

    strcpy ( new_str, initial_str );

    return new_str;
}

void point_to_underscore( char *string )
{
    int i;

    for ( i = 0; i <= strlen(string); i++ )
    {
        if ( string[i] == '.' )
        {
            string[i] = '_';
        }
    }
}

// screen print stuff

void print_info()
{
    const char* version = VERSION;

    printf( "===============================================================================\n" );
    printf( "diffscgen - Diff Speedcode Generator - Version %s\n", version );
    printf( "                                                            by Spider Jerusalem\n");
    printf( "===============================================================================\n" );
    printf( "\n" );
}

void print_help()
{
    printf( "Description:\n" );
    printf( "============\n" );
  //printf( "===============================================================================\n" );
    printf( "diffscgen creates 6502 speedcode that is needed to generate the destination    \n" );
    printf( "file(s) from the given source file(s).\n" );
    printf( "\n" );
    printf( "If more than two input files are given multiple routines will be created. I.e.\n" );
    printf( "for 3 files:\n" );
    printf( "   routine1 : file1 -> file2\n" );
    printf( "   routine2 : file2 -> file3\n" );
    printf( "\n" );
    printf( "So if you want make an animation loop, you could use the command:\n" );
    printf( "   diffscgen file1 file2 file3 file1\n" );
    printf( "\n" );
    printf( "Hint: use sane filenames for your infiles, because the assembler labels for\n");
    printf( "the routines will be created automagically depending on the filenames.\n");
    printf( "I.e. \"file1_to_file1:\", \"my_srcfile_prg_to_my_dstfile_prg:\"\n");
    printf( "\n" );

    printf( "Usage:\n" );
    printf( "======\n" );
    printf( "   diffscgen [options] {files}\n" );
    printf( "\n" );

    printf( "Usage examples:\n" );
    printf( "===============\n" );
    printf( "   diffscgen infile_src infile_dst\n" );
    printf( "   diffscgen -o outfile infile_src infile_dst\n" );
    printf( "   diffscgen -o outfile -l mylabel infile_src infile_dst\n" );
    printf( "   diffscgen -o outfile -l mylabel -r x infile_src infile_dst\n" );
    printf( "   diffscgen -o outfile -l mylabel -r y -s 0x7e infile_src infile_dst\n" );
    printf( "\n" );
  //printf( "===============================================================================\n" );
    printf( "Command line options:\n" );
    printf( "=====================\n" );
    printf( "   -i spaces  : intendation of exported speedcode (space characters).\n" );
    printf( "                [default: 4]\n" );
    printf( "   -l mylabel : label to be used in the output speedcode before offset.\n" );
    printf( "                [default: INFILE_SRC]\n" );
    printf( "   -o outfile : name of the output file for the speedcode.\n" );
    printf( "                [default: out.asm]\n" );
    printf( "   -r [a,x,y] : register to be used for the speedcode (a, x or y). If set to\n" );
    printf( "                x or y the speedcode will use inx/iny for successive values.\n" );
    printf( "                Depending on the files this will reduce speedcode size.\n" );
    printf( "                [default: a]\n" );
    printf( "   -s skip    : number of bytes to be skipped. I.e. set -s 2 for .PRG files\n" );
    printf( "                or -s 0x7e for .SID files.\n" );
    printf( "                [default: 0]\n" );
    printf( "\n" );
    printf( "Have fun!\n" );
}
