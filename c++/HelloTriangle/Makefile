CFLAGS = -std=c++17 -O3
ifdef DBG
CFLAGS += -g
endif

LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi
INCLUDE = -I./
SRC = $(wildcard *.cpp)

HelloTriangle: 
	g++ $(CFLAGS) $(INCLUDE) -o HelloTriangle $(SRC) $(LDFLAGS)

.PHONY: test clean

test: HelloTriangle
	./HelloTriangle

clean:
	rm -f HelloTriangle
