BIN_FILES  = servidor servidor_rpc 

CC = gcc

CPPFLAGS = -I$(INSTALL_PATH)/include -Wall

LDFLAGS = -L$(INSTALL_PATH)/lib/
LDLIBS = -lpthread -lrt

all: $(BIN_FILES)
.PHONY : all

servidor_rpc: rpc_server.o lista.o rpc_svc.o rpc_xdr.o
	gcc $^ -o servidor_rpc

servidor: servidor.o lista.o lines.o rpc_xdr.o rpc_clnt.o
	gcc $^ -o servidor -lpthread -lrt
	javac client.java



%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $<
	
	
clean:
	rm -f $(BIN_FILES) *.o 

.PHONY : clean
