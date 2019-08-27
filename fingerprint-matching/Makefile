PORT = linux3

PORT = linux3
CFLAGS = -D__cplusplus -Wall -O
CPP=g++
CC=gcc
INCPATH = ./
LIBS =  -L./lib/$(PORT)

OBJS=	main.o

SRCS=	main.cpp

FDU07_TARGET = $(PORT)/fingerprint-matching

all : $(FDU07_TARGET)

$(FDU07_TARGET) : $(OBJS)
	$(CPP) -o $@ $(OBJS) $(LIBS) -lsgfplib -lsgnfiq -lsgimage -lsgfpamx -lsgfdu07 -lusb -lpthread
