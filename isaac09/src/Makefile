SRCDIR = .
LIBDIR = ../util

CC          = g++
CFLAGS      =  -m32 -O9
#CFLAGS      =  -m32 -pg -O9
#CFLAGS      =  -m32 -g -O0

OBJS=interval.o interval_stabbing.o isaac_interval_stabbing.o compact_interval_stabbing.o mbr.o mbr_range.o hash.o table_offset.o static_bitsequence.o static_bitsequence_brw32.o static_bitsequence_rrr02.o darray.o bp.o bpcore.o balanced_parentheses.o bp_rp.o bp_hb.o bp_rmm.o tree_ff.o

all:  clean main cleanO

clean: 
	rm -f *.o *~  

cleanO: 
	rm -f *.o

main: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o main $(SRCDIR)/main.cpp

mbr.o:
	$(CC) $(CFLAGS) -c $(SRCDIR)/mbr.cpp

interval.o:
	$(CC) $(CFLAGS) -c $(SRCDIR)/interval.cpp

mbr_range.o:
	$(CC) $(CFLAGS) -c $(SRCDIR)/mbr_range.cpp

isaac_interval_stabbing.o:
	$(CC) $(CFLAGS) -c $(SRCDIR)/isaac_interval_stabbing.cpp

compact_interval_stabbing.o:
	$(CC) $(CFLAGS) -c $(SRCDIR)/compact_interval_stabbing.cpp

interval_stabbing.o:
	$(CC) $(CFLAGS) -c $(SRCDIR)/interval_stabbing.cpp

hash.o:
	$(CC) $(CFLAGS) -c hash.cpp

static_bitsequence.o:
	$(CC) $(CFLAGS) -c static_bitsequence.cpp

table_offset.o:
	  $(CC) $(CFLAGS) -c table_offset.cpp

static_bitsequence_rrr02.o:
	$(CC) $(CFLAGS) -c static_bitsequence_rrr02.cpp

static_bitsequence_brw32.o:
	$(CC) $(CFLAGS) -c static_bitsequence_brw32.cpp

bpcore.o:
	    $(CC) $(CFLAGS) -c bpcore.cpp

bp.o:
	  $(CC) $(CFLAGS) -c bp.cpp

bp_hb.o:
	  $(CC) $(CFLAGS) -c bp_hb.cpp

bp_rp.o:
	$(CC) $(CFLAGS) -c bp_rp.cpp

bp_rmm.o:
	$(CC) $(CFLAGS) -c bp_rmm.cpp

balanced_parentheses.o:
	$(CC) $(CFLAGS) -c balanced_parentheses.cpp

tree_ff.o:
	    $(CC) $(CFLAGS) -c tree_ff.cpp

darray.o:
	  $(CC) $(CFLAGS) -c darray.cpp
