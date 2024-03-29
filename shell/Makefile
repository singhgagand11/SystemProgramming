# Makefile for "mice" project
# Author: Gagandeep - Student 
# Author: Dr. Philip Nico
# Author: Dr. Kevin O'Gorman (minor tweaks and targets)
# $Id:$

CC 	= gcc $(SIZE)
# Choose one of these in the CFLAGS line
SUS3=-D_XOPEN_SOURCE=600  -std=c99
SUS2=-D_XOPEN_SOURCE=500  -ansi
SUS1=-D_XOPEN_SOURCE=1    -ansi   -D_XOPEN_SOURCE_EXTENDED=1

# You can choose one of these to set the executable size (if there's a choice).
# Select SIZE64 only when the hardware supports 64-bit executables, and then
# select it in both CFLAGS and also in LDFLAGS.   Most of the time, SIZE32 will
# be the most useful, since these will execute just fine on 64-bit machines to,
# but it doesn't work the other way.
# Whichever you chose, it has to be in all compile as well as linking rules.
SIZE32=-m32
SIZE64=-m64

# If your compiler is gcc, FORTIFY_SOURCE can help find memory problems at compile-time.  For some
# systems, the MALLOC_CHECK environment variable helps too, but this has to be asserted
# at run time, so there's nothing we can do about it here.
HARDEN=-D_FORTIFY_SOURCE=2

SIZE=$(SIZE32)
CFLAGS  = -Wall -g $(SUS2) $(SIZE) $(HARDEN) -pedantic 
LD 	= $(CC) $(SIZE)
LDFLAGS = $(SIZE)
LEX     = flex
LFLAGS  = -I
YACC    = bison
YFLAGS  = -d
PROG    = mice
OBJS	= main.o stringstuff.o lex.yy.o parser.tab.o \
	  pipeline.o stringlist.o myline.o miceUtility.o  
SRCS	= main.c stringstuff.c parser.y lexer.l \
	  pipeline.c stringlist.c myline.c   miceUtility.c 
HDRS	= pipeline.h stringstuff.h stringlist.h parser.h \
	  builtin.h config.h myline.h
LIBS 	= 
EXTRACLEAN = parser.output $(PROG) $(PROG).tar vg.out
BISONFILES  = lex.yy.c parser.tab.c parser.tab.h 

all: 	$(PROG)

$(PROG): $(OBJS)
	$(LD) $(LDFLAGS) -o $(PROG) $(OBJS) $(LIBS)

allclean: clean
	rm -f $(EXTRACLEAN)

clean:	
	rm -f $(OBJS) $(BISONFILES) *~ TAGS

tar:  $(PROG).tar

tarthis: allclean
	export here=$$(basename $$(pwd)); \
	( cd .. ; \
	tar cf $$here.tar $$here ) 

tarminus: allclean
	export here=$$(basename $$(pwd)); \
	( cd .. ; \
	tar cf $$here.tar $$here --exclude myline.c ) 

$(PROG).tar: Makefile $(SRCS) $(HDRS)
	tar cvf $(PROG).tar Makefile.hornet Makefile $(SRCS) $(HDRS)

tags : $(SRCS) $(HDRS)
	etags $(SRCS) $(HDRS)  

test:	$(PROG)
	rm -f vg.out
	MALLOC_CHECK=2 valgrind --log-file=vg.out --leak-check=full --show-reachable=yes --quiet $(PROG)
	if test -s vg.out ; then cat vg.out ; echo YOUR PROGRAM NEEDS FIXING; else echo ; echo NO PROBLEMS FOUND; echo; fi

lex.yy.c: lexer.l 
	$(LEX) $(LFLAGS) lexer.l

lex.yy.o: lex.yy.c parser.tab.h
	$(CC) -g -c lex.yy.c

parser.tab.o: parser.h parser.tab.h parser.y 
	$(CC) -g -c parser.tab.c

parser.tab.h: parser.tab.c
	@#nothing (see below)

parser.tab.c: parser.y
	$(YACC) $(YFLAGS) parser.y

# These dependencies generated by makedepend, but cleaned up for
# portability

lex.yy.o:	stringstuff.h  parser.h stringlist.h pipeline.h   parser.tab.h\
		config.h

main.o:		pipeline.h stringlist.h parser.h parser.tab.h config.h

pipeline.o:	pipeline.h stringlist.h parser.h parser.tab.h config.h

sigint.o:	pipeline.h stringlist.h config.h

stringlist.o:	stringlist.h config.h

stringstuff.o:	stringstuff.h parser.h stringlist.h pipeline.h parser.tab.h\
		config.h

y.tab.o:	pipeline.h stringlist.h parser.h parser.tab.h config.h

myline.o:	myline.c myline.h
