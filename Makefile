## flatex

CC := gcc
CFLAGS := -g -Wall

SRCDIR := src
BIN := flatex

OBJDIR := obj
OBJ := $(addprefix $(OBJDIR)/, flatex.o)

TESTSDIR := tests
TESTS := $(wildcard $(TESTSDIR)/*.c)
TESTBINS := $(patsubst $(TESTSDIR)/%.c, $(TESTSDIR)/bin/%, $(TESTS))

PREFIX := /usr/local

### BIN #####

$(BIN): $(SRCDIR)/main.c $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(SRCDIR)/%.h | $(OBJDIR)
	$(CC) $(CFLAGS) -o $@ -c $<

$(OBJDIR):
	mkdir -p $@

### TESTS ###

$(TESTSDIR)/bin/%: $(TESTSDIR)/%.c $(OBJ) | $(TESTSDIR)/bin
	$(CC) $(CFLAGS) -lcriterion -o $@ $^

$(TESTSDIR)/bin:
	mkdir -p $@

test: $(BIN) $(TESTBINS)
	for test in $(TESTBINS) ; do ./$$test ; done

#############

release: CFLAGS := -Wall -O2
release: clean $(BIN)

install: release
	mv $(BIN) $(PREFIX)/bin
	chmod 755 $(PREFIX)/bin/$(BIN)

uninstall:
	rm $(PREFIX)/bin/$(BIN)

clean:
	rm -f $(BIN) $(OBJ)

#############

.PHONY: test clean release install uninstall
