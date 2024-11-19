EXE = rs_server
CC = gcc 
LD = gcc 
CFLAGS = -fPIC -Wall
INCS = 
LINKFLAGS =
LIB_OBJS = main.o websocket_server.o protocol.o protocol_restream.o goertzel.o
all: $(EXE)
$(EXE): $(LIB_OBJS)
	@$(LD) $(LINKFLAGS) -o $(EXE) $(LIB_OBJS) -lc -lm -lwebsockets -lpthread
%.o: %.c
	@echo "Compiling" $<
	@$(CC) $(CFLAGS) $(INCS) -c $<
clean: 
	rm *.o
	rm $(EXE)
