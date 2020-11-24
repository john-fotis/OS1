CPP = g++
CPPFLAGS = -g -Wall
LDLIBS = -lcrypto -pthread
OBJS = Encoder.o Channel.o shared_memory.o
TARGET = p1.ex p2.ex

all: $(TARGET)

%.ex: %.o $(OBJS)
	$(CPP) $(CPPFLAGS) $< $(OBJS) -o $@ $(LDLIBS)

encoder: encoder.cc
	$(CPP) $(CPPFLAGS) -c Encoder.cc

channel: channel.cc
	$(CPP) $(CPPFLAGS) -c Channel.cc

clean:
	rm -f *.ex $(TARGET)
