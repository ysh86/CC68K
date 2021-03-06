#include        "stdio.h"
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

int     breaklab;
int     contlab;
int     retlab;

extern TYP              stdfunc;
extern struct amode     push[], pop[];

extern char *xalloc();
extern struct amode *make_offset();
extern struct enode *makenodei();
extern struct enode *makenodesp();
extern void initstack();
extern void gen_label();
extern void falsejp();
extern void genstmt();
extern void gen_code();
extern struct amode *make_label();
extern void truejp();
extern struct amode *gen_expr();
extern int natural_size();
extern SYM *gsearch();
extern void insert();
extern struct amode *make_immed();
extern void opt1();

struct amode    *makedreg(r)
/*
 *      make an address reference to a data register.
 */
int     r;
{       struct amode    *ap;
        ap =(struct amode *)xalloc(sizeof(struct amode));
        ap->mode = am_dreg;
        ap->preg = r;
        return ap;
}

struct amode    *makeareg(r)
/*
 *      make an address reference to an address register.
 */
int     r;
{       struct amode    *ap;
        ap =(struct amode *)xalloc(sizeof(struct amode));
        ap->mode = am_areg;
        ap->preg = r;
        return ap;
}

struct amode    *make_mask(mask)
/*
 *      generate the mask address structure.
 */
int     mask;
{       struct amode    *ap;
        ap = (struct amode *)xalloc(sizeof(struct amode));
        ap->mode = am_mask;
        ap->v.i = mask;
        return ap;
}

struct amode    *make_direct(i)
/*
 *      make a direct reference to an immediate value.
 */
int i;
{       return make_offset(makenodei(en_icon,i));
}

struct amode    *make_strlab(s)
/*
 *      generate a direct reference to a string label.
 */
char    *s;
{       struct amode    *ap;
        ap = (struct amode *)xalloc(sizeof(struct amode));
        ap->mode = am_direct;
        ap->v.offset = makenodesp(en_nacon,s);
        return ap;
}

void genwhile(stmt)
/*
 *      generate code to evaluate a while statement.
 */
struct snode    *stmt;
{       int     lab1, lab2;
        initstack();            /* initialize temp registers */
        lab1 = contlab;         /* save old continue label */
        lab2 = breaklab;        /* save old break label */
        contlab = nextlabel++;  /* new continue label */
        gen_label(contlab);
        if( stmt->s1 != 0 )      /* has block */
                {
                breaklab = nextlabel++;
                initstack();
                falsejp(stmt->exp,breaklab);
                genstmt(stmt->s1);
                gen_code(op_bra,0,make_label(contlab),(struct amode *)0);
                gen_label(breaklab);
                breaklab = lab2;        /* restore old break label */
                }
        else                            /* no loop code */
                {
                initstack();
                truejp(stmt->exp,contlab);
                }
        contlab = lab1;         /* restore old continue label */
}

void gen_for(stmt)
/*
 *      generate code to evaluate a for loop
 */
struct snode    *stmt;
{       int     old_break, old_cont, exit_label, loop_label;
        old_break = breaklab;
        old_cont = contlab;
        loop_label = nextlabel++;
        exit_label = nextlabel++;
        contlab = loop_label;
        initstack();
        if( stmt->l.exp0 != 0 )
                gen_expr(stmt->l.exp0,F_ALL | F_NOVALUE
                        ,natural_size(stmt->l.exp0));
        gen_label(loop_label);
        initstack();
        if( stmt->exp != 0 )
                falsejp(stmt->exp,exit_label);
        if( stmt->s1 != 0 )
                {
                breaklab = exit_label;
                genstmt(stmt->s1);
                }
        initstack();
        if( stmt->s2 != 0 )
                gen_expr((struct enode *)stmt->s2,F_ALL | F_NOVALUE,natural_size((struct enode *)stmt->s2));
        gen_code(op_bra,0,make_label(loop_label),(struct amode *)0);
        breaklab = old_break;
        contlab = old_cont;
        gen_label(exit_label);
}

void genif(stmt)
/*
 *      generate code to evaluate an if statement.
 */
struct snode    *stmt;
{       int     lab1, lab2, oldbreak;
        lab1 = nextlabel++;     /* else label */
        lab2 = nextlabel++;     /* exit label */
        oldbreak = breaklab;    /* save break label */
        initstack();            /* clear temps */
        falsejp(stmt->exp,lab1);
        if( stmt->s1 != 0 && stmt->s1->next != 0 )
                {
                if( stmt->s2 != 0 )
                        breaklab = lab2;
                else
                        breaklab = lab1;
                }
        genstmt(stmt->s1);
        if( stmt->s2 != 0 )             /* else part exists */
                {
                gen_code(op_bra,0,make_label(lab2),(struct amode *)0);
                gen_label(lab1);
                if( stmt->s2 == 0 || stmt->s2->next == 0 )
                        breaklab = oldbreak;
                else
                        breaklab = lab2;
                genstmt(stmt->s2);
                gen_label(lab2);
                }
        else                            /* no else code */
                gen_label(lab1);
        breaklab = oldbreak;
}

void gendo(stmt)
/*
 *      generate code for a do - while loop.
 */
struct snode    *stmt;
{       int     oldcont, oldbreak;
        oldcont = contlab;
        oldbreak = breaklab;
        contlab = nextlabel++;
        gen_label(contlab);
        if( stmt->s1 != 0 && stmt->s1->next != 0 )
                {
                breaklab = nextlabel++;
                genstmt(stmt->s1);      /* generate body */
                initstack();
                truejp(stmt->exp,contlab);
                gen_label(breaklab);
                }
        else
                {
                genstmt(stmt->s1);
                initstack();
                truejp(stmt->exp,contlab);
                }
        breaklab = oldbreak;
        contlab = oldcont;
}

void call_library(lib_name)
/*
 *      generate a call to a library routine.
 */
char    *lib_name;
{       SYM     *sp;
        sp = gsearch(lib_name);
        if( sp == 0 )
                {
                ++global_flag;
                sp =(SYM *) xalloc(sizeof(SYM));
                sp->tp = &stdfunc;
                sp->name = lib_name;
                sp->storage_class = sc_external;
                insert(sp,&gsyms);
                --global_flag;
                }
        gen_code(op_jsr,0,make_strlab(lib_name),(struct amode *)0);
}

void genswitch(stmt)
/*
 *      generate a linear search switch statement.
 */
struct snode    *stmt;
{       int             curlab;
        struct snode    *defcase;
        struct amode    *ap;
        curlab = nextlabel++;
        defcase = 0;
        initstack();
        ap = gen_expr(stmt->exp,F_DREG | F_VOL,4);
        if( ap->preg != 0 )
                gen_code(op_move,4,ap,makedreg(0));
        stmt = stmt->s1;
        call_library("_C_SWITCH"); /* TODO: asm format */
        while( stmt != 0 )
                {
                if( stmt->s2 )          /* default case ? */
                        {
                        stmt->l.label = curlab;
                        defcase = stmt;
                        }
                else
                        {
                        gen_code(op_dc,4,make_label(curlab),
                                         make_direct(stmt->l.label));
                        stmt->l.label = curlab;
                        }
                if( stmt->s1 != 0 && stmt->next != 0 )
                        curlab = nextlabel++;
                stmt = stmt->next;
                }
        if( defcase == 0 )
                gen_code(op_dc,4,make_direct(0),
                                 make_label(breaklab));
        else
                gen_code(op_dc,4,make_direct(0),
                                 make_label(defcase->l.label));
}

void gencase(stmt)
/*
 *      generate all cases for a switch statement.
 */
struct snode    *stmt;
{       while( stmt != 0 )
                {
                if( stmt->s1 != 0 )
                        {
                        gen_label(stmt->l.label);
                        genstmt(stmt->s1);
                        }
                else if( stmt->next == 0 )
                        gen_label(stmt->l.label);
                stmt = stmt->next;
                }
}

void genxswitch(stmt)
/*
 *      analyze and generate best switch statement.
 */
struct snode    *stmt;
{       int     oldbreak;
        oldbreak = breaklab;
        breaklab = nextlabel++;
        genswitch(stmt);
        gencase(stmt->s1);
        gen_label(breaklab);
        breaklab = oldbreak;
}

void genreturn(stmt)
/*
 *      generate a return statement.
 */
struct snode    *stmt;
{       struct amode    *ap;
        if( stmt != 0 && stmt->exp != 0 )
                {
                initstack();
                ap = gen_expr(stmt->exp,F_ALL,4);
                if( ap->mode != am_dreg || ap->preg != 0 )
                        gen_code(op_move,4,ap,makedreg(0));
                }
        if( retlab == -1 )
                {
                retlab = nextlabel++;
                gen_label(retlab);
                if( save_mask != 0 )
                        gen_code(op_movem,4,pop,make_mask(save_mask));
                gen_code(op_unlk,0,makeareg(6),(struct amode *)0);
                gen_code(op_rts,0,(struct amode *)0,(struct amode *)0);
                }
        else
                gen_code(op_bra,0,make_label(retlab),(struct amode *)0);
}

void genstmt(stmt)
/*
 *      genstmt will generate a statement and follow the next pointer
 *      until the block is generated.
 */
struct snode    *stmt;
{       while( stmt != 0 )
                {
                switch( stmt->stype )
                        {
                        case st_label:
                                gen_label(stmt->l.label);
                                break;
                        case st_goto:
                                gen_code(op_bra,0,make_label(stmt->l.label),
                                                   (struct amode *)0);
                                break;
                        case st_expr:
                                initstack();
                                gen_expr(stmt->exp,F_ALL | F_NOVALUE,
                                        natural_size(stmt->exp));
                                break;
                        case st_return:
                                genreturn(stmt);
                                break;
                        case st_if:
                                genif(stmt);
                                break;
                        case st_while:
                                genwhile(stmt);
                                break;
                        case st_for:
                                gen_for(stmt);
                                break;
                        case st_continue:
                                gen_code(op_bra,0,make_label(contlab),
                                                  (struct amode *)0);
                                break;
                        case st_break:
                                gen_code(op_bra,0,make_label(breaklab),
                                                  (struct amode *)0);
                                break;
                        case st_switch:
                                genxswitch(stmt);
                                break;
                        default:
                                printf("DIAG - unknown statement.\n");
                                break;
                        }
                stmt = stmt->next;
                }
}

void genfunc(stmt)
/*
 *      generate a function body.
 */
struct snode    *stmt;
{       retlab = contlab = breaklab = -1;
        if( lc_auto & 1 )       /* if frame size odd */
                ++lc_auto;      /* make it even */
        gen_code(op_link,0,makeareg(6),make_immed(-lc_auto));
        opt1(stmt);
        genstmt(stmt);
        genreturn((struct snode *)0);
}
