EXE = pointoverdrawbench
OBJS = \
       main.o positiongen.o solid.o stats.o \
       shaderloader/shaderloader.o \
       debuggl/debuggl.o
CFLAGS = -std=c99 -I../../../extra
LDLIBS = -lGL -lGLEW -lglut -lm

.PHONY: all clean

all: $(EXE)

clean:
	-rm -f $(OBJS) $(EXE)

$(EXE): $(OBJS)
	$(CC) $(LDFLAGS) -o $(EXE) $(OBJS) $(LOADLIBES) $(LDLIBS)
