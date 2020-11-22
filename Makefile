CPP = g++
CPPFLAGS = -g -Wall
LDLIBS = -lcrypto -pthread
OBJS = encoder.o channel.o shared_memory.o
TARGET = main.ex p1.ex p2.ex destroy_memory.ex

all: $(TARGET)

%.ex: %.o $(OBJS)
	$(CPP) $(CPPFLAGS) $< $(OBJS) -o $@ $(LDLIBS)

main: main.cc
	$(CPP) $(CPPFLAGS) -c main.cc

encoder: encoder.cc
	$(CPP) $(CPPFLAGS) -c encoder.cc

channel: channel.cc
	$(CPP) $(CPPFLAGS) -c channel.cc

clean:
	rm -f *.ex $(TARGET)
