#  CPE 357-01/02
#  -------------------
#  Last Modified: Fri Jan 14 00:39:47 PST 2011
#  @author Matthew Tondreau and Gagandeep 

ALL=mytar
    
    

CC=gcc

# Choose one of these in the CFLAGS line
SUS3=-D_XOPEN_SOURCE=600  -std=c99
SUS2=-D_XOPEN_SOURCE=500  -ansi
SUS1=-D_XOPEN_SOURCE=1    -ansi   -D_XOPEN_SOURCE_EXTENDED=1

# You can choose one of these to set the executable size (if there's a choice).
# Select SIZE64 only when the hardware supports 64-bit executables, and then
# select it in both CFLAGS and also in LDFLAGS.   Most of the time, SIZE32 will
# be the most useful, since these will execute just fine on 64-bit machines to,
# but it doesn't work the other way.
SIZE32=-m32
SIZE64=-m64

# If your compiler is gcc, FORTIFY_SOURCE can help find memory problems at compile-time.  For some
# systems, the MALLOC_CHECK environment variable helps too, but this has to be asserted
# at run time, so there's nothing we can do about it here.
HARDEN=-D_FORTIFY_SOURCE=2

SIZE=$(SIZE32)
CFLAGS=-Wall -g $(SUS2) $(HARDEN) $(SIZE) -pedantic -D DEBUG  
NCFLAGS=-Wall $(SUS2) $(SIZE)
LDFLAGS=$(SIZE)

all:	$(ALL)
	@echo
	@echo All made. You can now \"make docs\"  

mytar:	mytar.o tar.o tarent.o tarUtility.o dirtree.o
	$(CC) $(LDFLAGS) -o $@ $^

mytar.o: mytar.c tar.c tarent.c tarUtility.c dirtree.c
	$(CC) $(CFLAGS) -c $<

.PHONE: Doxyfile docs
docs: 
	doxygen Doxyfile
Doxyfile:
	doxygen -g
	@echo ======================================================
	@echo You MUST modify this Doxyfile to get the right results.
	@echo ======================================================

check:
clean:
	rm -rf examples core* *.s *.o *.gch $(ALL)
