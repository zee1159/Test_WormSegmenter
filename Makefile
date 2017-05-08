CC = g++

CFLAGS = `pkg-config --cflags opencv`
LIBS = `pkg-config --libs opencv`

all:
	$(CC) $(CFLAGS) main.cpp $(LIBS) -g -o Test.so -I/usr/lib/jvm/java-1.8.0-openjdk/include -I/usr/lib/jvm/java-1.8.0-openjdk/include/linux

clean:
	rm -rf main main.o
