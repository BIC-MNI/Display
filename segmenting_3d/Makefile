OPT = -O

MODULES = ../../Modules

include ../../C_dev/Makefile.include

INCLUDE = -I$(MODULES)/Include $(C_UTILS_INCLUDE)


###########################################################################

test: test.o segment.o
	$(CC) $(LFLAGS) test.o segment.o $(MNI_LIBS) -o $@

lint_test: test.ln  segment.ln
	lint -u $(LINTFLAGS) test.ln segment.ln

###########################################################################

batch_segment: batch_segment.o segment.o
	$(CC) $(LFLAGS) batch_segment.o segment.o $(MNI_LIBS) -o $@

lint_batch_segment: batch_segment.ln  segment.ln
	lint -u $(LINTFLAGS) batch_segment.ln segment.ln $(MNI_LINTLIBS)

###########################################################################
