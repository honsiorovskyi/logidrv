BINARY  = logidrv
SOURCES = main.c logitech.c
HEADERS = logitech.h

PREFIX  = /usr/local
BINPATH = $(PREFIX)/bin

all: $(BINARY)

run: $(BINARY)
	./$(BINARY)

install: all
	install -m755 $(BINARY) $(BINPATH)

clean:
	rm -f $(BINARY)

$(BINARY): *.c *.h
	clang \
		-framework Carbon \
		-framework coreFoundation \
		-framework IOKit \
		-o $@ \
		$(SOURCES)


