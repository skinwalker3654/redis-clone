CC = gcc
TARGET = redis
SRC_DIR = source
SOURCES = $(wildcard $(SRC_DIR)/*.c)

all: $(TARGET)

$(TARGET): $(SOURCES)
	@echo "compiling redis-clone ..."
	$(CC) $(SOURCES) -o $(TARGET) -g

run: $(TARGET)
	@echo "executing redis-clon ..."
	./$(TARGET)

clean:
	@echo "removing executables ..."
	rm -f $(TARGET)

.PHONY: all run clean
