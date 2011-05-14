ALGOS := pd0_4_1
DELETEFILES := $(wildcard *.o *.d)
DEPFILES := $(patsubst %.c,%.d,$(wildcard *.c))
CFLAGS := -Wall

all: $(ALGOS)

.PHONY: all clean mrproper

pd0_4_1: qsort_pdclib_0.4.1.o testframe.o
	$(CC) $(CFLAGS) $^ -o $@

-include $(DEPFILES)

clean:
	@for file in $(DELETEFILES); do rm $$file; done

mrproper: clean
	@for file in $(ALGOS); do if [ -f $$file ]; then rm $$file; fi; done
	@if [ -f log.csv ]; then rm log.csv; fi

%.o: %.c Makefile
	$(CC) $(CFLAGS) -MMD -MP -MT "$*.d" -g -std=c99 -c $< -o $@
