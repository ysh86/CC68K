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

extern void getsym();
extern void error();
extern int getl();
extern int doinclude();
extern int dodefine();
extern char *xalloc();
extern char *litlate();
extern void insert();

FILE            *inclfile[10];
int             incldepth = 0;
int             inclline[10];
char            *lptr;

int  preprocess()
{       ++lptr;
        lastch = ' ';
        getsym();               /* get first word on line */
        if( lastst != id ) {
                error(ERR_PREPROC);
                return getl(incldepth == 0);
                }
        if( strcmp(lastid,"include") == 0 )
                return doinclude();
        else if( strcmp(lastid,"define") == 0 )
                return dodefine();
        else    {
                error(ERR_PREPROC);
                return getl(incldepth == 0);
                }
}

int doinclude()
{       int     rv;
        getsym();               /* get file to include */
        if( lastst != sconst ) {
                error(ERR_INCLFILE);
                return getl(incldepth == 0);
                }
        inclline[incldepth] = lineno;
        inclfile[incldepth++] = input;  /* push current input file */
        input = fopen(laststr,"r");
        if( input == 0 ) {
                input = inclfile[--incldepth];
                error(ERR_CANTOPEN);
                rv = getl(incldepth == 0);
                }
        else    {
                rv = getl(incldepth == 1);
                lineno = -32768;        /* dont list include files */
                }
        return rv;
}

int dodefine()
{       SYM     *sp;
        getsym();               /* get past #define */
        if( lastst != id ) {
                error(ERR_DEFINE);
                return getl(incldepth == 0);
                }
        ++global_flag;          /* always do #define as globals */
        sp =(SYM *) xalloc(sizeof(SYM));
        sp->name = litlate(lastid);
        sp->value.s = litlate(lptr);
        insert(sp,&defsyms);
        --global_flag;
        return getl(incldepth == 0);
}
