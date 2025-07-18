
CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c99 -lm
SRCDIR = src
SOURCES = $(SRCDIR)/main.c $(SRCDIR)/decoder.c $(SRCDIR)/wav_io.c $(SRCDIR)/ftae_io.c $(SRCDIR)/sinewave_queue.c
TARGET = dfta_decode

.PHONY: all clean install test help

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(SOURCES) -o $(TARGET) $(CFLAGS)

clean:
	rm -f $(TARGET)

install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

test: $(TARGET)
	@echo "Build successful! Test with:"
	@echo "./$(TARGET) compressed.ftae restored.wav"

help:
	@echo "D-FTA Decoder Makefile"
	@echo "======================"
	@echo "Available targets:"
	@echo "  all      - Build the dfta_decode executable"
	@echo "  clean    - Remove built files"
	@echo "  install  - Install to /usr/local/bin"
	@echo "  test     - Show test commands"
	@echo "  help     - Show this help"
	@echo ""
	@echo "Usage examples:"
	@echo "  make"
	@echo "  make clean && make"
	@echo "  ./dfta_decode input.ftae output.wav"
