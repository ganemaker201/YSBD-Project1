CC=gcc
CFLAGS=-g3 -I./include 
LDFLAGS=-L./lib -Wl,-rpath=./lib
LIBS=-lbf
OPTIMIZE=-O2

SRCDIR=./src
INCDIR=./include
LIBDIR=./lib
EXAMPLESDIR=./examples
BUILDDIR=./build

HP_MAIN_SRC=$(EXAMPLESDIR)/hp_main.c $(SRCDIR)/record.c $(SRCDIR)/hp_file.c
BF_MAIN_SRC=$(EXAMPLESDIR)/bf_main.c $(SRCDIR)/record.c

HP_MAIN=$(BUILDDIR)/hp_main
BF_MAIN=$(BUILDDIR)/bf_main



all: hp bf ht dbcheck

$(BUILDDIR):
	@mkdir -p $(BUILDDIR)

hp: $(BUILDDIR)
	@echo "Compiling hp_main ..."
	$(CC) $(CFLAGS) $(LDFLAGS) $(HP_MAIN_SRC) $(LIBS) -o $(HP_MAIN) $(OPTIMIZE) -w 

bf: $(BUILDDIR)
	@echo "Compiling bf_main ..."
	$(CC) $(CFLAGS) $(LDFLAGS) $(BF_MAIN_SRC) $(LIBS) -o $(BF_MAIN) $(OPTIMIZE) -w

clean:
	rm -f $(HP_MAIN) $(BF_MAIN) data.db block_example.db
	@rm -rf $(BUILDDIR)

.PHONY: all hp bf clean