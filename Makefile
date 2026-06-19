CC=gcc
CFLAGS= -c -g -Wall -Werror
LINK_FLAGS= -lsqlite3
BUILD_DIR= build
TARGET= $(BUILD_DIR)/main.out

SRC= $(shell find src/ -name "*.c")
OBJ= $(SRC:%.c=$(BUILD_DIR)/%.o)

$(TARGET):$(OBJ)
	@echo -e "\n\n\e[32;1m---------------------------\e[0m"
	@echo -e "\e[32;1mMaking $< files to $@ file\e[0m"
	$(CC) $^ -o $@ $(LINK_FLAGS)

$(BUILD_DIR)/%.o: %.c
	@echo -e "\n\n\e[32;1m---------------------------\e[0m"
	@echo -e "\e[32;1mMaking $< files to $@ file\e[0m"
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< -o $@ $(LINK_FLAGS)

clean:
	rm -r $(BUILD_DIR)/*

