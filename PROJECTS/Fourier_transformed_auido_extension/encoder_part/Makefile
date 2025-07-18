
CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c99 -lm
SRCDIR = src
SOURCES = $(SRCDIR)/main.c $(SRCDIR)/encoder.c $(SRCDIR)/fft.c $(SRCDIR)/wav_io.c $(SRCDIR)/ftae_io.c $(SRCDIR)/sinewave_queue.c
TARGET = dfta_encode

.PHONY: all clean install

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(SOURCES) -o $(TARGET) $(CFLAGS)

clean:
	rm -f $(TARGET)

install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

test: $(TARGET)
	@echo "Build successful! Test with:"
	@echo "./$(TARGET) sample.wav output.ftae --compression-level medium"

help:
	@echo "Available targets:"
	@echo "  all      - Build the dfta_encode executable"
	@echo "  clean    - Remove built files"
	@echo "  install  - Install to /usr/local/bin"
	@echo "  test     - Show test commands"
	@echo "  help     - Show this help"
