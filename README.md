October 1989
============

 I have further modified the compiler to generate symbols and
syntax compatible with Motorola's FREEWARE 68332 assembler;  it is also
compatible with Motorola's 68K family macro assembler as released for
MS-DOS. I used Microsoft Quick C to compile and debug it, and the Quick
C make file (CC68K.MAK) is included.  The original comments of the
author (Matthew Brandt) and the modifier (Ron Fox) are included.

 This compiler generates 68000 assembly code, and as such it
doesn't use any of the extra addressing modes or instructions supported
by the 68332. It is meant to be used as a quick and dirty tool to
evaluate Motorola's 68000 microprocessors;  it can be used to generate
production code but I must emphasize that this tool is NOT a supported
product of Motorola Inc.  If you are developing a product and you need a
C compiler for the 68K family, please purchase the official Motorola C
compiler and macro assembler package, since the price is reasonable and
it is supported by Motorola.

Scott Howard  
Field Applications Engineer  
Motorola Semiconductor Products, Canada

-----------------------------------------------------------------------

This archive contains a modified version of Matthew Brandt's C68K
compiler. Where Brandt's compiler generated UNIX assembler code, this
version generates code compatible with MASM, Motorola's resident
Assembler.

  The current version has been revised and quite a few bugs fixed.  It
should be more robust than the previous version. The following is the
copyright notice which appeared in the original version:

```c
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
 *	as this notice stays intact. Please forward any enhancements or question
s
 *	to:
 *
 *		Matthew Brandt
 *		Box 920337
 *		Norcross, Ga 30092
 */
```

  I performed the original modifications using VMS C, and have therefore
had to edit each module to stick a <> around the stdio.h #include statements.
I have not tested this compiler on an IBM compatible.  Note that the original
documentation for the compiler states that there will be compilation warnings
when building the compiler.  This is still true, due to some mixed mode
int vs pointer assignments.  Take note large memory model users.

Bugs/restrictions in the original have not been fixed, these include:
1. Errors involving char parameters to functions
2. Limits on the size of functions due to the fact that a function is
   completely parsed before code is generated.
3. No support for floats, however float declarations do not produce errors.
4. Preprocessor support is limited to #include and symbol replacement #define
   directives, that is #define's without arguments.
5. There is no real run time library supplied.  This includes a lack of the
   'standard' useful functions such as printf, scanf and so on.

To rebuild:  
  Compile all .C files except sieve.c which is a test program.  
  Link these together with the run time library of your C compiler.  
  Name the output file C68M.  Assemble LIB.SA with the Motorola 68000  
  assembler.

To use:  
Type:
```bash
   C68M  file.c
```

Where file.c is the name of the file you want to compile.
The file file.lis and file.sa will be produced. The .lis file is a listing,
and the .sa file is valid assembler input for MASM.  It should be linked
with MLINK and whatever other modules you wish to include.

Ron Fox  
NSCL  
Michigan State University  
East Lansing, MI 48824-1321

The following is the original READ.ME file for C68K as distributed by
Matthew Brandt, the original author.  Documentation above supersedes the
information in the file below.

Original READ.ME
===========================================================================


NOTICE:
```
	68000 C compiler

	Copyright 1984, 1985, 1986 Matthew Brandt.
	all commercial rights reserved.

	This compiler is intended as an instructive tool for personal use. Any
	use for profit without the written consent of the author is prohibited.

	This compiler may be distributed freely for non-commercial use as long
	as this notice stays intact. Please forward any enhancements or questions
	to:

		Matthew Brandt
		Box 920337
		Norcross, Ga 30092
```

This compiler is an optimizing C compiler for the Motorola 68000 processor.
It has successfully compiled itself on UNIX system V running on a Motorola
VME-10. Since this code was written for a machine with long integers it may
exhibit some irregularity when dealing with long integers on the IBM-PC.
The author makes no guarantees. This is not meant as a serious developement
tool although it could, with little work, be made into one. The bugs and
limitations of this compiler are listed below:

* Although you may declare floating point types the code generator does
not know how to deal with them. They should therefore be avoided.

* The preprocessor does not support arguments to #define'd macros or
any of #line #ifdef... etc. Only #include and #define are supported.

* Function arguments declared as char may not work properly. Declare
them as int.

* The size of functions is slightly limited due to the fact that the
entire function is parsed before any code is generated.

* The output of the compiler is in the UNIX 68000 assembler format.

To run the compiler type `cc68 sieve.c`. This will compile the program sieve
and produce two files; sieve.lis is a source listing with a symbol type
reference; sieve.s is the 68000 assembly language produced.

The file lib.s is the assembly source for some runtime support routines
which must be loaded with the final code. The is no standard runtime library
support.

The compiler can be compiled by microsoft C version 3.0 or higher. MSC will
issue lots of warnings but they can be ignored. The file make.bat will
rebuild the compiler if MSC is available.

If you wish to make commercial use of all or part of this package please
contact me at the above address or (404)662-0366. Any voluntary contribution
from non-commercial users will be greatly appreciated but is by no means
necessary. enjoy...

Matt Brandt
