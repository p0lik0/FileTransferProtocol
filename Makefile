.PHONY: all clean

CC = gcc
CFLAGS = -Wall -Werror
LIBS = -lpthread

SERVERDIR=./server
SERVERMAITREDIR=./server_maitre
CLIENTDIR=./client

INCLDIRS = -Iserver/structures
INCLDIRSM = -Iserver/structures
INCLDIRC = -Iclient/structures

OBJSS = $(SERVERDIR)/structures/csapp.o $(SERVERDIR)/service.o
OBJSM = $(SERVERMAITREDIR)/structures/csapp.o 
OBJSC = $(CLIENTDIR)/structures/csapp.o $(CLIENTDIR)/gestion_client.o

PROGS = $(SERVERDIR)/serverp $(CLIENTDIR)/client $(SERVERMAITREDIR)/server_maitre 

all: $(PROGS)

# ---- SERVER_MAITRE ----
$(SERVERMAITREDIR)/server_maitre.o : $(SERVERMAITREDIR)/server_maitre.c
	$(CC) $(CFLAGS) $(INCLDIRSM) -c -o $@ $<

$(SERVERMAITREDIR)/server_maitre: $(SERVERMAITREDIR)/server_maitre.o $(OBJSM)
	$(CC) -o $@ $^ $(LIBS)

$(SERVERMAITREDIR)/%.o: $(SERVERMAITREDIR)/%.c 
	$(CC) $(CFLAGS) $(INCLDIRSM) -c -o $@ $<

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
	rm -f $(PROGS) $(SERVERDIR)/*.o $(SERVERDIR)/structures/*.o $(CLIENTDIR)/*.o $(CLIENTDIR)/structures/*.o $(SERVERMAITREDIR)/*.o $(SERVERMAITREDIR)/structures/*.o