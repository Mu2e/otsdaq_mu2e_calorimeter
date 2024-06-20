# Mettere PROJ_FOLD=yes per pescare gli header dalla cartella
PROJ_FOLD ?= no

CC = gcc
CFLAGS = -Wall -Wextra -std=c11
CPPFLAGS = -DGNU_COMP -I. 

ifeq ($(PROJ_FOLD), yes)
CPPFLAGS += \
	-I../../LIB \
	-I../../MZB/MCU/Drivers \
	-I../../MZB/MCU/APD 
else
CPPFLAGS += -Ires
endif

BIN_DIR = bin
TARGET = prova
SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,$(BIN_DIR)/%.o,$(SRCS))

# SRCS_SO = API_I2C.c SBL_utils.c
# OBJS_SO = $(patsubst %.c,$(BIN_DIR)/%.so,$(SRCS_SO))
TARGET_SO = API_I2C_Shared.so


.PHONY: all clean

# all: $(TARGET) $(OBJS_SO)
all: $(TARGET) $(TARGET_SO)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(BIN_DIR)/%.o: %.c | $(BIN_DIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -fPIC -o $@ $<

$(TARGET_SO): $(OBJS)
	$(CC) -shared -o $@ $^


# $(BIN_DIR)/%.so: %.c | $(BIN_DIR)
# 	$(CC) $(CFLAGS) $(CPPFLAGS) -shared -o $@ -fPIC $<

bin:
	mkdir -p $(BIN_DIR)

clean:
	rm -f $(TARGET) $(OBJS) $(OBJS_SO) 

# sharedlib:	API_I2C.c | $(BIN_DIR)
# 	$(CC) $(CFLAGS) $(CPPFLAGS) -shared -o bin/API_I2C.so -fPIC API_I2C.c