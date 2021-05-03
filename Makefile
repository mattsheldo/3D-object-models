## Compiler
CC=g++
## Linker
LD=$(CC)
## Flags
CPPFLAGS = -Wall -g -DLINUX
LFLAGS = -lglut -L/usr/lib -L/usr/X11R6/lib -lXi -lXmu -lGL -lGLU -lm

TARGETS = $(PROGFILES:.cpp=)

PROGFILES = \
        3Dmodel.cpp \
        $(NULL)

targets default: $(TARGETS)

$(PROGFILES:.cpp=): 3Dmodel.o
	$(CC) -o 3Dmodel 3Dmodel.o ${LFLAGS}

depend :
	makedepend ${PROGFILES}
# DO NOT DELETE
