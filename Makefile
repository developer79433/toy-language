CC=gcc
CFLAGS=-Wall -g3
# -Werror
LD=$(CC)
LDFLAGS=$(CFLAGS)
BINARY=toy
GENERATED_SOURCES=lex.yy.c parser.tab.c
SOURCES=$(sort $(wildcard *.c) $(GENERATED_SOURCES))
OBJECTS=$(SOURCES:.c=.o)
HEADERS=$(wildcard *.h)

.PHONY: all
all: test

$(BINARY): $(OBJECTS)
	$(LD) -o "$@" $^

%.o: %.c $(HEADERS)
	$(CC) -c $(CFLAGS) -o "$@" $<

lex.yy.c: lexer.l
	flex --debug $<

parser.tab.c: parser.y
	bison -Wcounterexamples --debug $<

.PHONY: clean
clean:
	rm -f $(BINARY) $(OBJECTS)

.PHONY: test
test: $(BINARY)
	./$(BINARY) sample-program.toy 2>&1 | less
