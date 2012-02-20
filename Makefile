#
# Created by gmakemake (Ubuntu Sep  7 2011) on Sun Feb 19 17:56:46 2012
#

#
# Definitions
#

.SUFFIXES:
.SUFFIXES:	.a .o .c .C .cpp .s .S
.c.o:
		$(COMPILE.c) $<
.C.o:
		$(COMPILE.cc) $<
.cpp.o:
		$(COMPILE.cc) $<
.S.s:
		$(CPP) -o $*.s $<
.s.o:
		$(COMPILE.cc) $<
.c.a:
		$(COMPILE.c) -o $% $<
		$(AR) $(ARFLAGS) $@ $%
		$(RM) $%
.C.a:
		$(COMPILE.cc) -o $% $<
		$(AR) $(ARFLAGS) $@ $%
		$(RM) $%
.cpp.a:
		$(COMPILE.cc) -o $% $<
		$(AR) $(ARFLAGS) $@ $%
		$(RM) $%

CC =		gcc
CXX =		g++

RM = rm -f
AR = ar
LINK.c = $(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS)
LINK.cc = $(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS)
COMPILE.c = $(CC) $(CFLAGS) $(CPPFLAGS) -c
COMPILE.cc = $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c
CPP = $(CPP) $(CPPFLAGS)
########## Default flags (redefine these with a header.mak file if desired)
CXXFLAGS =	-ggdb
CFLAGS =	-ggdb
CLIBFLAGS =	-lm
CCLIBFLAGS =	
########## End of default flags


CPP_FILES =	
C_FILES =	exec.c fsinit.c fsops.c fsread.c fswrite.c history.c os1shell.c signalHandlers.c
PS_FILES =	
S_FILES =	
H_FILES =	exec.h fs.h fsinit.h fsops.h fsread.h fswrite.h history.h os1shell.h signalHandlers.h
SOURCEFILES =	$(H_FILES) $(CPP_FILES) $(C_FILES) $(S_FILES)
.PRECIOUS:	$(SOURCEFILES)
OBJFILES =	exec.o fsinit.o fsops.o fsread.o fswrite.o history.o signalHandlers.o 

#
# Main targets
#

all:	os1shell 

os1shell:	os1shell.o $(OBJFILES)
	$(CC) $(CFLAGS) -o os1shell os1shell.o $(OBJFILES) $(CLIBFLAGS)

#
# Dependencies
#

exec.o:	os1shell.h
fsinit.o:	fs.h fsinit.h
fsops.o:	
fsread.o:	fs.h
fswrite.o:	fs.h fsread.h fswrite.h
history.o:	history.h
os1shell.o:	exec.h fs.h fsinit.h fsops.h history.h os1shell.h signalHandlers.h
signalHandlers.o:	history.h os1shell.h signalHandlers.h

#
# Housekeeping
#

Archive:	archive.tgz

archive.tgz:	$(SOURCEFILES) Makefile
	tar cf - $(SOURCEFILES) Makefile | gzip > archive.tgz

clean:
	-/bin/rm $(OBJFILES) os1shell.o core 2> /dev/null

realclean:        clean
	-/bin/rm -rf os1shell 
