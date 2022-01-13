#------------------------------------------------
# GCC LINUX make file for REORDER project.
#
#
#------------------------------------------------



#----- make NDEBUG=1 for nodebugging -----#
ifeq ($(NDEBUG), 1)
CL = gcc -O2 -c -DNDEBUG
LINK = gcc -O2 -DNDEBUG
OPT = -mmmx -msse2


#----- DEBUG case is below -----#
else   
CL = gcc -g -c -Wall -DDEBUG
LINK = gcc -g -Wall -DDEBUG
OPT = 
endif



#----- default for make -----#
all : reordr


#----- project is here -----#
reordr : reordr.c reordr.h block.o block.h mgir03.o mgir03.h
	$(CL) $(OPT) reordr.c
	$(LINK) -o reordr reordr.o block.o mgir03.o -lm

block.o : block.c block.h 
	$(CL) $(OPT) block.c 

mgir03.o : mgir03.c block.h 
	$(CL) $(OPT) mgir03.c 


#----- compression and cleaning of files -----#
zoo :
	zoo ahP: reordr.zoo *

gzip :
	tar cvf reordr.tar *
	gzip -9 reordr.tar

bzip :
	tar cvf reordr.tar *
	bzip2 -9 reordr.tar

clean :
	for f in *.o;   do rm -f $$f; done
	for f in *.bak; do rm -f $$f; done	
	for f in *.zoo; do rm -f $$f; done
	for f in *.gz;  do rm -f $$f; done
	for f in *.out; do rm -f $$f; done
	for f in *.log; do rm -f $$f; done
	for f in *.dvi; do rm -f $$f; done
	for f in core;  do rm -f $$f; done
	for f in *.tar; do rm -f $$f; done
	for f in *.tgz; do rm -f $$f; done
	for f in *.bz2; do rm -f $$f; done
	for f in *~;    do rm -f $$f; done


cleaner :
	make clean
	for f in reordr;           do rm -f $$f; done
	for f in itoatest;         do rm -f $$f; done


