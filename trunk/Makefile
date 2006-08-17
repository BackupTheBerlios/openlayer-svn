# Makefile to build cbuild

CFLAGS=-O2 -W -Wall

ifneq ($(MINGDIR)$(DJDIR),)
	CC = gcc
	EXEEXT = .exe
else
	EXEEXT =
endif

CBUILD=cbuild$(EXEEXT)

all: $(CBUILD) exec

.PHONY: exec clean help verbose install uninstall loadpng glyphkeeper

$(CBUILD): cbuild.c
	@$(CC) $(CFLAGS) -o $@ $<

exec: $(CBUILD)
	@./$(CBUILD)
	@./$(CBUILD) rmcbuild

clean: ${CBUILD}
	@./${CBUILD} clean

help: $(CBUILD)
	@./$(CBUILD) makehelp
	@./$(CBUILD) rmcbuild

install: ${CBUILD}
	@./${CBUILD} install
	@./$(CBUILD) rmcbuild

uninstall: ${CBUILD}
	@./${CBUILD} uninstall
	@./$(CBUILD) rmcbuild

loadpng: ${CBUILD}
	@./${CBUILD} loadpng
	@./$(CBUILD) rmcbuild

glyphkeeper: ${CBUILD}
	@./${CBUILD} glyphkeeper
	@./$(CBUILD) rmcbuild

verbose: $(CBUILD)
	@./$(CBUILD) verbose
	@./$(CBUILD) rmcbuild