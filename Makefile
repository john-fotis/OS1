CPP = g++
CPPFLAGS = -g -Wall
LDLIBS = -lcrypto -pthread
OBJS = Encoder.o Channel.o Shared_memory.o
TARGET = p1.ex encoder1.ex channel.ex encoder2.ex p2.ex

all: $(TARGET)

%.ex: %.o $(OBJS)
	$(CPP) $(CPPFLAGS) $< $(OBJS) -o $@ $(LDLIBS)

encoder: encoder.cc
	$(CPP) $(CPPFLAGS) -c Encoder.cc

channel: channel.cc
	$(CPP) $(CPPFLAGS) -c Channel.cc

Shared_memory: Shared_memory.cc
	$(CPP) $(CPPFLAGS) -c Shared_memory.cc

clean:
	rm -f *.ex $(TARGET)
