CC = gcc
AOF_FILE = AOF
TARGET = redis
SRC_DIR = source
SOURCES = $(wildcard $(SRC_DIR)/*.c)


$(TARGET): $(SOURCES)
	@echo "compiling redis-clone ..."
	$(CC) $(SOURCES) -o $(TARGET)

run: $(TARGET)
	@echo "executing redis-clone ..."
	./$(TARGET)

clean:
	@echo "removing executables and data..."
	rm $(TARGET) $(AOF_FILE)

.PHONY: run clean
