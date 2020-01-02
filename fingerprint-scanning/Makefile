PORT = linux3

PORT = linux3
CFLAGS = -D__cplusplus -Wall -O -DLINUX3
CPP=g++
CC=gcc
INCPATH = ./
LIBS =  -L./lib/$(PORT)

OBJS=	main.o

SRCS=	main.cpp

FDU07_TARGET = $(PORT)/fingerprint-scanner

all : $(FDU07_TARGET)

$(FDU07_TARGET) : $(OBJS)
	$(CPP) -o $@ $(OBJS) $(mysql_config --cflags) $(LIBS) $(mysql_config --libs) -lsgfplib -lsgnfiq -lsgimage -lsgfpamx -lsgfdu07 -lusb -lpthread -lmysqlclient
