SOURCES=$(wildcard src/*.cpp)
SOURCES+=$(wildcard vendor/*/*.cpp)
LIBS = sdl2
INCLUDES = $(shell pkg-config --libs $(LIBS))
CFLAGS = $(shell pkg-config --cflags $(LIBS))
TARGET = play

build:
	clang++ -std=c++17 \
	-ggdb \
	$(INCLUDES) \
	$(CFLAGS) \
	-Iinclude \
	-Ivendor \
	$(SOURCES) \
	-framework OpenGl \
	-o $(TARGET) \
	&& ./$(TARGET) \

clean:
	rm -f ./$(TARGET)