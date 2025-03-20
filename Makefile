CC=gcc
CFLAGS=-Wall -g3
# -Werror
LD=$(CC)
LDFLAGS=$(CFLAGS)
# BISONFLAGS=--header
BISONFLAGS=--header -Wcounterexamples --debug
# FLEXFLAGS=
FLEXFLAGS=--debug
BINARY=toy
GENERATED_SOURCES=lex.yy.c parser.tab.c
GENERATED_HEADERS=parser.tab.h
SOURCES=$(sort $(wildcard *.c) $(GENERATED_SOURCES))
OBJECTS=$(SOURCES:.c=.o)
HEADERS=$(sort $(wildcard *.h) $(GENERATED_HEADERS))

.PHONY: all
all: test

$(BINARY): $(OBJECTS)
	$(LD) -o "$@" $^

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
test: $(BINARY)
	./$(BINARY) sample-program.toy 2>&1 | less
