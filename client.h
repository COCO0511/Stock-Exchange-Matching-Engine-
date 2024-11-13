#ifndef __CLIENT_H__
#define __CLIENT_H__
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <arpa/inet.h>

using namespace std;

class Client {
private:
  int status;
  int socket_fd;
  struct addrinfo host_info;
  struct addrinfo* host_info_list;
  const char* hostname;
  const char* port;
public:
  Client(const char* _hostname, const char* _port) {
    hostname = _hostname;
    port = _port;
  }
  int createClient();
  ~Client() {
    freeaddrinfo(host_info_list);
    close(socket_fd);
  };
};

class User {
private:
    int client_fd;
    int thread_id;
    std::string ip;
public:
    User(int client_fd, int thread_id, std::string ip) : client_fd(client_fd), thread_id(thread_id), ip(ip) {}
    int getClientFd() { return client_fd; }
    int getThreadId() { return thread_id; }
    std::string getIp() { return ip; }
};

#endif