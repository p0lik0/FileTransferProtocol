.PHONY: all clean

CC = gcc
CFLAGS = -Wall -Werror
LIBS = -lpthread

SERVERDIR=./server
CLIENTDIR=./client
INCLDIRS = -Iserver/structures
INCLDIRC = -Iclient/structures

OBJSS = $(SERVERDIR)/structures/csapp.o $(SERVERDIR)/service.o
OBJSC = $(CLIENTDIR)/structures/csapp.o $(CLIENTDIR)/gestion_client.o

PROGS = $(SERVERDIR)/serverp $(CLIENTDIR)/client 

all: $(PROGS)

# ---- SERVER ----
$(SERVERDIR)/serverp.o : $(SERVERDIR)/serverp.c
	$(CC) $(CFLAGS) $(INCLDIRS) -c -o $@ $<

$(SERVERDIR)/serverp: $(SERVERDIR)/serverp.o $(OBJSS)
	$(CC) -o $@ $^ $(LIBS)

$(SERVERDIR)/%.o: $(SERVERDIR)/%.c 
	$(CC) $(CFLAGS) $(INCLDIRS) -c -o $@ $<

# ---- CLIENT ----
$(CLIENTDIR)/client.o : $(CLIENTDIR)/client.c
	$(CC) $(CFLAGS) $(INCLDIRC) -c -o $@ $<

$(CLIENTDIR)/client: $(CLIENTDIR)/client.o $(OBJSC)
	$(CC) -o $@ $^ $(LIBS)

$(CLIENTDIR)/%.o: $(CLIENTDIR)/%.c 
	$(CC) $(CFLAGS) $(INCLDIRC) -c -o $@ $<

# ---- CLEAN ----
clean:
	rm -f $(PROGS) $(SERVERDIR)/*.o $(SERVERDIR)/structures/*.o $(CLIENTDIR)/*.o $(CLIENTDIR)/structures/*.o