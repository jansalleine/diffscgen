#ifndef DIFFSCGEN_H_
#define DIFFSCGEN_H_

#define VERSION     "1.0"

#define DEFAULT_ITD 4
#define DEFAULT_LBL "INFILE_SRC"
#define DEFAULT_OUT "out.asm"
#define DEFAULT_REG "a"

#define INX         "inx"
#define INY         "iny"
#define LDA_IMM     "lda #$"
#define LDX_IMM     "ldx #$"
#define LDY_IMM     "ldy #$"
#define RTS         "rts"
#define STA_ABS     "sta"
#define STX_ABS     "stx"
#define STY_ABS     "sty"

#define SIZE_IMM    2
#define SIZE_ABS    4

char *newstr( char *initial_str );
void point_to_underscore( char *string );
void print_info();
void print_help();

#endif // DIFFSCGEN_H_
