#  CPE 357
#  -------------------
#  CPE 357 - 01
#  Project 2 - word count
#  @author Gagandeep Singh

ALL=proj2

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
CFLAGS=-Wall -g $(SUS2) -pedantic $(SIZE) $(HARDEN)
NCFLAGS=-Wall $(SUS2) $(SIZE)
LDFLAGS=

all:	$(ALL)
	@echo
	@echo All made.  You can now \"make docs\"

# One could omit this target because the defaults do the same thing.
proj2:	proj2.o myhash.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^


proj2.o:    proj2.c proj2.h
	$(CC) $(CFLAGS) -c $<

myhash.o: myhash.c myhash.h
	$(CC) $(CFLAGS) -c $<


.PHONY:	Doxyfile docs
docs:	
	doxygen Doxyfile


Doxyfile:
	doxygen -g
	@echo ======================================================
	@echo You MUST modify this Doxyfile to get the right results.
	@echo ======================================================

clean:
	rm -f core* *.s *.o *.gch $(ALL)

