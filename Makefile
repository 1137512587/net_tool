.RHNOY:clean
BIN	=main
CC	=gcc
CFLAGS	=-Wall -g
SUBDIR	=$(shell ls -d net_config)
SUBSRC	=$(shell find $(SUBDIR) -name '*.c')
SUBOBJ	=$(SUBSRC:%.c=%.o)
ROOTSRC	=$(shell ls *.c)
ROOTOBJ	=$(ROOTSRC:%.c=%.o)
INCDIR	=-I net_config/inc

$(BIN):$(ROOTOBJ) $(SUBOBJ)
	$(CC) $(CFLAGS) -o $(BIN) $(ROOTOBJ) $(SUBOBJ)

.c.o:
	$(CC) $(CFLAGS) $(INCDIR) -c $< -o $@

clean:
	rm -f $(BIN) $(ROOTOBJ) $(SUBOBJ)
