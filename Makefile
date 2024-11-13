TARGETS=server

all: $(TARGETS)
clean:
	rm -f $(TARGETS)

server: main.cpp parse.cpp request.cpp server.cpp sql.cpp tinyxml2.cpp Database.cpp client.cpp 
	g++ -g -o $@ $^ -lpqxx -lpq -lpthread


