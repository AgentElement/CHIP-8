CC=gcc
LIBS=-lGL -lGLU -lglfw3 -lX11 -lXxf86vm -lXrandr -lpthread -lXi -ldl -lXinerama -lXcursor -lm
IDIR=include
ODIR=obj
SRC=src

CFLAGS=-g -I$(IDIR)

_DEPS = glad/glad.c
DEPS = $(patsubst %, $(IDIR)/%, $(_DEPS))

_OBJ = display.o chip8.o main.o
OBJ = $(patsubst %, $(ODIR)/%, $(_OBJ))

$(ODIR)/%.o: $(SRC)/%.c
	$(CC) -c $^ -o $@ $(CFLAGS)

chip8test: $(OBJ)
	$(CC) $^ $(DEPS) -o $@ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o chip8test	