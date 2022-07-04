#include        "stdio.h"
#include        "string.h"
#include        "c.h"
#include        "expr.h"
#include        "gen.h"
#include        "cglbdec.h"

/*
 *	68000 C compiler
 *
 *	Copyright 1984, 1985, 1986 Matthew Brandt.
 *  all commercial rights reserved.
 *
 *	This compiler is intended as an instructive tool for personal use. Any
 *	use for profit without the written consent of the author is prohibited.
 *
 *	This compiler may be distributed freely for non-commercial use as long
 *	as this notice stays intact. Please forward any enhancements or questions
 *	to:
 *
 *		Matthew Brandt
 *		Box 920337
 *		Norcross, Ga 30092
 */

char            infile[20],
                listfile[20],
                outfile[20];
extern TABLE    tagtable;
int		mainflag;
extern int      total_errors;

extern int options();
extern int openfiles();
extern void initsym();
extern int getch();
extern void getsym();
extern void compile();
extern void summary();
extern void release_global();
extern void closefiles();
extern void makename();
extern void list_table();

int main(argc,argv)
int     argc;
char    **argv;
{
        while(--argc) {
                if( **++argv == '-')
                        options();
                else if( openfiles(*argv)) {
                        lineno = 0;
                        initsym();
                        getch();
                        getsym();
                        compile();
                        summary();
                        release_global();
                        closefiles();
                        }
                }
        return 0;
}

int	options()
{
  return 0;
}

int     openfiles(s)
char    *s;
{
/*        int     ofl; */

        strcpy(infile,s);
        strcpy(listfile,s);
        strcpy(outfile,s);
        makename(listfile,".lis");
        makename(outfile,".s");
        if( (input = fopen(infile,"r")) == 0) {
                printf(" cant open %s\n",infile);
                return 0;
                }
/*        ofl = creat(outfile,0);
        if( ofl < 0 )
                {
                printf(" cant create %s\n",outfile);
                fclose(input);
                return 0;
                }
*/
        if( (output = fopen(outfile,"w")) == 0) {
                printf(" cant open %s\n",outfile);
                fclose(input);
                return 0;
                }
        if( (list = fopen(listfile,"w")) == 0) {
                printf(" cant open %s\n",listfile);
                fclose(input);
                fclose(output);
                return 0;
                }
        return 1;
}

void makename(s,e)
char    *s, *e;
{       while(*s != 0 && *s != '.')
                ++s;
        while((*s++ = *e++));
}

void summary()
{       printf("\n -- %d errors found.",total_errors);
        fprintf(list,"\f\n *** global scope symbol table ***\n\n");
        list_table(&gsyms,0);
        fprintf(list,"\n *** structures and unions ***\n\n");
        list_table(&tagtable,0);
}

void closefiles()
{       fclose(input);
        fprintf(output,"\tEND\t");	/* END directive to output file. */
        fclose(output);
        fclose(list);
}
