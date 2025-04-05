CC=gcc
# -DYYDEBUG=1
CFLAGS=-Wall -g3
# -Werror
LD=$(CC)
LIBS=m
LDFLAGS=$(CFLAGS)
BISONFLAGS=--header
# BISONFLAGS=--header -Wcounterexamples --debug
FLEXFLAGS=
# FLEXFLAGS=--debug
BINARY=toy
GENERATED_SOURCES=lex.yy.c parser.tab.c
GENERATED_HEADERS=parser.tab.h
SOURCES=$(sort $(wildcard *.c) $(GENERATED_SOURCES))
OBJECTS=$(SOURCES:.c=.o)
HEADERS=$(sort $(wildcard *.h) $(GENERATED_HEADERS))

.PHONY: all
all: test

$(BINARY): $(OBJECTS)
	$(LD) -o "$@" $^ $(addprefix -l,$(LIBS))

%.o: %.c $(HEADERS)
	$(CC) -c $(CFLAGS) -o "$@" $<

lex.yy.c: lexer.l
	flex $(FLEXFLAGS) $<

parser.tab.c parser.tab.h: parser.y
	bison $(BISONFLAGS) $<

.PHONY: clean
clean:
	rm -f $(BINARY) $(OBJECTS) $(GENERATED_SOURCES) $(GENERATED_HEADERS)

.PHONY: test
test: tests

.PHONY: tests
# 2>&1 | less
tests: $(BINARY)
	@for test_file in tests/*.toy ; do \
		if ./$(BINARY) $$test_file >/dev/null 2>&1 ; then \
			echo PASS: $$test_file ; \
		else \
			echo FAIL: $$test_file ; \
		fi ; \
	done
