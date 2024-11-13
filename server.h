#ifndef __SERVER_H__
#define __SERVER_H__

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <arpa/inet.h>
#include <vector>
#include "client.h"
#include "exception.h"
#include "parse.h"




using namespace std;

class Server {
private:
  int status;
  int socket_fd;
  struct addrinfo host_info;
  struct addrinfo* host_info_list;
  const char* hostname = NULL;
  const char* port;
public:
  Server(const char* _port) {
    port = _port;
  }
  int createServer();
  int acceptClient(std::string* ip);
  //void run();
  static void* handleRequest(void* userInfo);
  ~Server() {
    freeaddrinfo(host_info_list);
    close(socket_fd);
  };
};

#endif


