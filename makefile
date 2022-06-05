CC=gcc
CXX=g++
FLAGS=-pthread -fPIC
HEADERS=$(wildcard *.hpp)

all: main1 Guard AOclient Singelton PollClient Reactor PollServer

main1: main1.o
	$(CC) $(FLAGS) main1.o -o main1

main1.o: $(HEADERS)  main1.c 
	$(CC) $(FLAGS) -c main1.c

AOclient: AOclient.o
	$(CXX) $(FLAGS) AOclient.o -o AOclient

AOclient.o: $(HEADERS)  AOclient.cpp 
	$(CXX) $(FLAGS) -c AOclient.cpp 

Guard:
	$(CXX) guard.cpp -o Guard $(FLAGS)

Singelton:
	$(CXX) singelton.cpp -o Singelton $(FLAGS)

Reactor: $(HEADERS) PollServer.o
	$(CXX) $<  -o Reactor $(FLAGS)

PollServer.o: $(HEADERS)  PollServer.cpp 
	$(CXX) $(FLAGS) -c PollServer.cpp 

PollServer: PollServer.o Reactor.o
	$(CXX) $(FLAGS) PollServer.o Reactor.o -o PollServer

PollClient.o: $(HEADERS)  PollClient.cpp 
	$(CXX) $(FLAGS) -c PollClient.cpp 

PollClient: PollClient.o
	$(CXX) $(FLAGS) PollClient.o  -o PollClient

clean: 
	rm -f *.o main1 Guard Reactor AOclient PollClient PollServer Singelton a.out
 