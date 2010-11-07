TARGET = SpatialTest
SRCS = Core.cpp BruteForce.cpp SortAndSweep.cpp UniformGrid.cpp HierarchicalGrid.cpp Octree.cpp LooseOctree.cpp Kdtree.cpp SphereObject.cpp
OBJS = $(SRCS:.cpp=.o)

UNAME := $(shell uname)

CXX = g++
CFLAGS += -Wall

ifeq ($(UNAME), Darwin)
  CLINKFLAGS += -framework OpenGL -framework GLUT -framework ApplicationServices
else
  ifeq ($(findstring CYGWIN, $(UNAME)), CYGWIN)
    CFLAGS += -D_WIN32 -DLGUT_DISABLE_ATEXIT_HACK
    CLINKFLAGS += -lglut32 -lopengl32
    EXE = .exe
  else
	CLINKFLAGS += -L/usr/X11R6/lib64 -L/usr/X11R6/lib
	CLINKFLAGS += -lglut -lGLU -lGL -lXi -lXmu -lX11 -lm -lpthread
  endif
endif

all: $(TARGET:=$(EXE))

$(TARGET): $(OBJS)
	$(CXX) $(CGLAGS) -o $@ $^ $(CLINKFLAGS)

clean:
	$(RM) $(TARGET:=$(EXE)) $(OBJS)

clobber:
	$(RM) *.bak *.o *~

run:
	./$(TARGET:=$(EXE))

debug:
	@echo "Target: "
	@echo $(TARGET:=$(EXE))
	@echo
	@echo "Sources: "
	@echo $(SRCS)
	@echo
	@echo "Include paths: "
	@echo $(CFLAGS)
	@echo
	@echo "Link libs: "
	@echo $(CLINKFLAGS)

.PHONY: all run clean clobber
