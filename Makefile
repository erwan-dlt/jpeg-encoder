CC      ?= gcc
CFLAGS  ?= -O2
# -Wno-unused-variable : les tables de include/qtables.h sont déclarées static
# dans un en-tête inclus par plusieurs fichiers, d'où des avertissements inutiles.
CFLAGS  += -Wall -Wextra -Wno-unused-variable -std=c99 -Iinclude -MMD -MP

BIN     = ppm2jpeg
SRC     = $(wildcard src/*.c)
OBJ     = $(patsubst src/%.c,obj/%.o,$(SRC))

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) -o $@

obj/%.o: src/%.c | obj
	$(CC) $(CFLAGS) -c $< -o $@

obj:
	mkdir -p obj

# Régénère src/tableaux_huffman.c à partir des tables de include/htables.h
tables: tools/genere_huffman.c src/htables.c
	$(CC) -O2 -Wall -Wextra -std=c99 tools/genere_huffman.c src/htables.c -o genere_huffman
	./genere_huffman > src/tableaux_huffman.c
	rm -f genere_huffman

# Compilation sans optimisation, avec symboles de débogage : make debug
debug: CFLAGS += -O0 -g
debug: clean all

# Encode les images de test et vérifie les fichiers JPEG produits
test: all
	./tests/run_tests.sh

clean:
	rm -rf $(BIN) obj out

.PHONY: all debug test tables clean

-include $(OBJ:.o=.d)
