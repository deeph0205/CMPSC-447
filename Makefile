#
# File          : Makefile
# Description   : Build file for CMPSC447 project 1, applied cryptography
#                 


# Environment Setup
CC=clang
CFLAGS=-c $(INCLUDES) -g -Wall
LINK=clang -g
LDFLAGS=$(LIBDIRS)
AR=ar rc
RANLIB=ranlib

# Suffix rules
.c.o :
	${CC} ${CFLAGS} $< -o $@

#
# Setup builds

TARGETS=cmpsc447-p1
LIBS=

#
# Project Protections

p1 : $(TARGETS)

cmpsc447-p1 : cmpsc447-user.o
	$(LINK) $(LDFLAGS) cmpsc447-user.o $(LIBS) -o $@

clean:
	rm -f *.o *~ $(TARGETS)

BASENAME=p1
tar: 
	tar cvfz $(BASENAME).tgz -C ..\
	    $(BASENAME)/Makefile \
		$(BASENAME)/cmpsc447-user.c \
	    $(BASENAME)/cmpsc447-user.h \
	    $(BASENAME)/submit-out \
		$(BASENAME)/submit-out2 \
		$(BASENAME)/submit-out3


