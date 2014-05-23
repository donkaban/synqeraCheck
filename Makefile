# HOST - MACOS,LINUX

HOST 	= MACOS
TARGET	= ./test

# -------------------------------------------------------------------------------------------

SOURCES = main.cpp 
HEADERS =

# -------------------------------------------------------------------------------------------

LINUX_CXX = g++
MACOS_CXX = clang

CXX_FLAGS = -c -Wall -Wextra -pedantic -std=c++11 -O3 -m32 -DHOST_$(HOST) 

LINUX_LINK_FLAGS = -m32 -lX11
MACOS_LINK_FLAGS = -m32 -L/usr/X11R6/lib -lX11 


OBJECTS=$(SOURCES:.cpp=.o)

all: $(SOURCES) $(HEADERS) $(TARGET) Makefile
	rm -f $(OBJECTS)

$(TARGET): $(OBJECTS) $(HEADERS)  Makefile
	$(CXX) $(OBJECTS) $($(HOST)_LINK_FLAGS) -lGL -o $@
	
.cpp.o: $(SOURCES)  $(HEADERS) 
	$(CXX) $(CXX_FLAGS) -c -o $@ $<

clean:
	rm -f $(TARGET)
	rm -f $(OBJECTS)
	rm -f *.bin
	rm -f *.result*
	
	
	