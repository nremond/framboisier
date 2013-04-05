# What to call the final executable
TARGET = like-raspberries

# Which object files that the executable consists of
OBJS= rest-client.o like.o nfc-utils.o led.o

# What compiler to use
CC = gcc

# Compiler flags, -g for debug, -c to make an object file
CFLAGS = -c -g -std=c99

# This should point to a directory that holds libcurl, if it isn't
# in the system's standard lib dir
# We also set a -L to include the directory where we have the openssl
# libraries
LDFLAGS = -L/home/dast/lib -L/usr/local/ssl/lib

# We need -lssl and -lcrypto when using libcurl with SSL support
LIBS = -lcurl -lssl -lcrypto -lnfc

# Link the target with all objects and libraries
$(TARGET) : $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LDFLAGS) $(LIBS)

# clear out all suffixes
.SUFFIXES:
# list only those we use
.SUFFIXES: .o .c

# define a suffix rule for .c -> .o
.c.o :
	$(CC) $(CFLAGS) $<



clean :
	rm -f $(TARGET) $(OBJS) *~