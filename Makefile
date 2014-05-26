# HOST - MACOS,LINUX

HOST 	= MACOS
TARGET	= ./test

# -------------------------------------------------------------------------------------------

SOURCES = common.cpp miniEngine.cpp main.cpp 
HEADERS = common.h miniEngine.h miniMath.h

# -------------------------------------------------------------------------------------------

CXX = g++
CXX_FLAGS = -c -Wall -Wextra -std=c++11 -O3 -m32 -DGL_GLEXT_PROTOTYPES -DHOST_$(HOST) 

LINUX_LINK_FLAGS = -m32 -lX11 -lGL
MACOS_LINK_FLAGS = -m32 -L/usr/X11R6/lib -lX11 -lGL


OBJECTS=$(SOURCES:.cpp=.o)

all: $(SOURCES) $(HEADERS) $(TARGET) Makefile
	rm -f $(OBJECTS)

$(TARGET): $(OBJECTS) $(HEADERS)  Makefile
	$(CXX) $(OBJECTS) $($(HOST)_LINK_FLAGS) -o $@
	
.cpp.o: $(SOURCES)  $(HEADERS) 
	$(CXX) $(CXX_FLAGS) -c -o $@ $<

clean:
	rm -f $(TARGET)
	rm -f $(OBJECTS)
	rm -f *.log
	
	
	