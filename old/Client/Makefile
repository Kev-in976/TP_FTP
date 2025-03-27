.PHONY: all clean

# Désactiver les règles implicites
.SUFFIXES:

# Compilateur et options
CC = gcc
CFLAGS = -Wall
LDFLAGS =

# Répertoires
CLIENT_DIR = Client
SERVEUR_DIR = Serveur

# Librairies nécessaires
LIBS += -lpthread

# Fichiers sources et exécutables
CLIENT_SRC = $(wildcard $(CLIENT_DIR)/*.c)
SERVEUR_SRC = $(wildcard $(SERVEUR_DIR)/*.c)
CLIENT_OBJS = $(CLIENT_SRC:.c=.o)
SERVEUR_OBJS = $(SERVEUR_SRC:.c=.o)

CLIENT_BIN = $(CLIENT_DIR)/client
SERVEUR_BIN = $(SERVEUR_DIR)/serveur

# Compilation de tous les programmes
all: $(CLIENT_BIN) $(SERVEUR_BIN)

# Compilation des fichiers objets
$(CLIENT_DIR)/%.o: $(CLIENT_DIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(SERVEUR_DIR)/%.o: $(SERVEUR_DIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Création des exécutables
$(CLIENT_BIN): $(CLIENT_OBJS)
	$(CC) -o $@ $^ $(LIBS)

$(SERVEUR_BIN): $(SERVEUR_OBJS)
	$(CC) -o $@ $^ $(LIBS)

# Nettoyage
clean:
	rm -f $(CLIENT_DIR)/*.o  $(CLIENT_BIN) 
	rm -f $(SERVEUR_DIR)/*.o $(SERVEUR_BIN)
