CPP = g++
CPPFLAGS = -g -Wall
LDLIBS = -lcrypto -lpthread -lrt
OBJS = Encoder.o Channel.o Shared_memory.o
TARGET = p1.ex encoder1.ex channel.ex encoder2.ex p2.ex

all: $(TARGET)

%.ex: %.o $(OBJS)
	$(CPP) $(CPPFLAGS) $< $(OBJS) -o $@ $(LDLIBS)

Encoder: Encoder.cc
	$(CPP) $(CPPFLAGS) -c Encoder.cc

Channel: Channel.cc
	$(CPP) $(CPPFLAGS) -c Channel.cc

Shared_memory: Shared_memory.cc
	$(CPP) $(CPPFLAGS) -c Shared_memory.cc

encoder1: encoder1.cc global.h
	$(CPP) $(CPPFLAGS) -c encoder1.cc

channel: channel.cc
	$(CPP) $(CPPFLAGS) -c channel.cc

encoder2: encoder2.cc
	$(CPP) $(CPPFLAGS) -c encoder2.cc

clean:
	rm -f *.ex $(TARGET)
