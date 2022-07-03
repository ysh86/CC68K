PROJ	=CC68K
DEBUG	=0
CC	=qcl
CFLAGS_G	= /AS /W1 /Ze 
CFLAGS_D	= /Zi /Zr /Gi$(PROJ).mdt /Od 
CFLAGS_R	= /Od /DNDEBUG 
CFLAGS	=$(CFLAGS_G) $(CFLAGS_R)
LFLAGS_G	= /CP:0xffff /NOI /SE:0x80 /ST:0x1000 
LFLAGS_D	= /CO /INCR 
LFLAGS_R	= 
LFLAGS	=$(LFLAGS_G) $(LFLAGS_R)
RUNFLAGS	=
OBJS_EXT = 	
LIBS_EXT = 	

all:	$(PROJ).exe

analyze.obj:	analyze.c

cglbdef.obj:	cglbdef.c

cmain.obj:	cmain.c

decl.obj:	decl.c

expr.obj:	expr.c

func.obj:	func.c

gencode.obj:	gencode.c

genstmt.obj:	genstmt.c

getsym.obj:	getsym.c

init.obj:	init.c

intexpr.obj:	intexpr.c

list.obj:	list.c

memmgt.obj:	memmgt.c

optimize.obj:	optimize.c

outcode.obj:	outcode.c

peepgen.obj:	peepgen.c

preproc.obj:	preproc.c

register.obj:	register.c

searchkw.obj:	searchkw.c

stmt.obj:	stmt.c

symbol.obj:	symbol.c

$(PROJ).exe:	analyze.obj cglbdef.obj cmain.obj decl.obj expr.obj func.obj gencode.obj \
	genstmt.obj getsym.obj init.obj intexpr.obj list.obj memmgt.obj optimize.obj outcode.obj \
	peepgen.obj preproc.obj register.obj searchkw.obj stmt.obj symbol.obj $(OBJS_EXT)
	echo >NUL @<<$(PROJ).crf
analyze.obj +
cglbdef.obj +
cmain.obj +
decl.obj +
expr.obj +
func.obj +
gencode.obj +
genstmt.obj +
getsym.obj +
init.obj +
intexpr.obj +
list.obj +
memmgt.obj +
optimize.obj +
outcode.obj +
peepgen.obj +
preproc.obj +
register.obj +
searchkw.obj +
stmt.obj +
symbol.obj +
$(OBJS_EXT)
$(PROJ).exe

$(LIBS_EXT);
<<
	link $(LFLAGS) @$(PROJ).crf

run: $(PROJ).exe
	$(PROJ) $(RUNFLAGS)

