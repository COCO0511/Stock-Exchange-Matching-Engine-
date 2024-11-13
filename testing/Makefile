TARGETS=client client2 client3 test_functionality test_scalability

all: $(TARGETS)
clean:
	rm -f $(TARGETS)

client: client.cpp
	g++ -g -o $@ $<
client2: client2.cpp
	g++ -g -o $@ $<
client3: client3.cpp
	g++ -g -o $@ $<
test_functionality: test_functionality.cpp
	g++ -g -o $@ $<
test_scalability: test_scalability.cpp
	g++ -g -o  $@ $< -pthread


