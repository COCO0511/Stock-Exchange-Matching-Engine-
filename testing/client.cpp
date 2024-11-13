#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <sstream>
#include <vector>

#define MAX_SIZE 65536

using namespace std;

void req1(const char *hostname) {
  const char *port     = "12345";
  int status;
  int socket_fd;
  struct addrinfo host_info;
  struct addrinfo *host_info_list;
  memset(&host_info, 0, sizeof(host_info));
  host_info.ai_family   = AF_UNSPEC;
  host_info.ai_socktype = SOCK_STREAM;

  status = getaddrinfo(hostname, port, &host_info, &host_info_list);
  if (status != 0) {
    cerr << "Error: cannot get address info for host" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return ;
  } //if

  socket_fd = socket(host_info_list->ai_family, 
		     host_info_list->ai_socktype, 
		     host_info_list->ai_protocol);
  if (socket_fd == -1) {
    cerr << "Error: cannot create socket" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return ;
  } //if
  
  cout << "Connecting to " << hostname << " on port " << port << "..." << endl;
  
  status = connect(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
  if (status == -1) {
    cerr << "Error: cannot connect to socket" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return ;
  } //if

  stringstream ss;
  // transaction: order 1, SYM, -1, 50
  ss<<"<?xml version=\"1.0\" encoding=\"UTF-8\"?><transactions id=\"1\">"
  "<order sym=\"SYM\" amount=\"3\" limit=\"12.5\"/>"
  "</transactions>";
  
  string request = ss.str();
  size_t size = request.length();
  request = to_string(size) + '\n' + request;

  int m = send(socket_fd, request.c_str(), request.size() + 1, 0);
  cout << "client1 send status: " << endl;
  cout << m << endl;

  vector<char> response(MAX_SIZE, 0);
  int len = recv(socket_fd, response.data(), response.size(), 0);
  if(len <= 0) {
    cerr << "Error: no recv!" << endl;
  }
  string res = response.data();
  cout << "Response: " << endl;
  cout << res << endl;
  freeaddrinfo(host_info_list);
  close(socket_fd);
}

void req2(const char *hostname) {
  const char *port     = "12345";
  int status;
  int socket_fd;
  struct addrinfo host_info;
  struct addrinfo *host_info_list;
  memset(&host_info, 0, sizeof(host_info));
  host_info.ai_family   = AF_UNSPEC;
  host_info.ai_socktype = SOCK_STREAM;

  status = getaddrinfo(hostname, port, &host_info, &host_info_list);
  if (status != 0) {
    cerr << "Error: cannot get address info for host" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return ;
  } //if

  socket_fd = socket(host_info_list->ai_family, 
		     host_info_list->ai_socktype, 
		     host_info_list->ai_protocol);
  if (socket_fd == -1) {
    cerr << "Error: cannot create socket" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return ;
  } //if
  
  cout << "Connecting to " << hostname << " on port " << port << "..." << endl;
  
  status = connect(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
  if (status == -1) {
    cerr << "Error: cannot connect to socket" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return ;
  } //if

  stringstream ss;
  // transaction: order 1, SYM, -1, 50
  ss<<"<?xml version=\"1.0\" encoding=\"UTF-8\"?><transactions id=\"2\">"
  "<order sym=\"SYM\" amount=\"-1\" limit=\"13\"/>"
  "</transactions>";
  
  string request = ss.str();
  size_t size = request.length();
  request = to_string(size) + '\n' + request;

  int m = send(socket_fd, request.c_str(), request.size() + 1, 0);
  cout << "client1 send status: " << endl;
  cout << m << endl;

  vector<char> response(MAX_SIZE, 0);
  int len = recv(socket_fd, response.data(), response.size(), 0);
  if(len <= 0) {
    cerr << "Error: no recv!" << endl;
  }
  string res = response.data();
  cout << "Response: " << endl;
  cout << res << endl;
  freeaddrinfo(host_info_list);
  close(socket_fd);
}

void req3(const char *hostname) {
  const char *port     = "12345";
  int status;
  int socket_fd;
  struct addrinfo host_info;
  struct addrinfo *host_info_list;
  memset(&host_info, 0, sizeof(host_info));
  host_info.ai_family   = AF_UNSPEC;
  host_info.ai_socktype = SOCK_STREAM;

  status = getaddrinfo(hostname, port, &host_info, &host_info_list);
  if (status != 0) {
    cerr << "Error: cannot get address info for host" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return ;
  } //if

  socket_fd = socket(host_info_list->ai_family, 
		     host_info_list->ai_socktype, 
		     host_info_list->ai_protocol);
  if (socket_fd == -1) {
    cerr << "Error: cannot create socket" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return ;
  } //if
  
  cout << "Connecting to " << hostname << " on port " << port << "..." << endl;
  
  status = connect(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
  if (status == -1) {
    cerr << "Error: cannot connect to socket" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return ;
  } //if

  stringstream ss;
  // transaction: order 1, SYM, -1, 50
  ss<<"<?xml version=\"1.0\" encoding=\"UTF-8\"?><transactions id=\"3\">"
  "<order sym=\"SYM\" amount=\"2\" limit=\"12.7\"/>"
  "</transactions>";
  
  string request = ss.str();
  size_t size = request.length();
  request = to_string(size) + '\n' + request;

  int m = send(socket_fd, request.c_str(), request.size() + 1, 0);
  cout << "client1 send status: " << endl;
  cout << m << endl;

  vector<char> response(MAX_SIZE, 0);
  int len = recv(socket_fd, response.data(), response.size(), 0);
  if(len <= 0) {
    cerr << "Error: no recv!" << endl;
  }
  string res = response.data();
  cout << "Response: " << endl;
  cout << res << endl;
  freeaddrinfo(host_info_list);
  close(socket_fd);
}

void req4(const char *hostname) {
  const char *port     = "12345";
  int status;
  int socket_fd;
  struct addrinfo host_info;
  struct addrinfo *host_info_list;
  memset(&host_info, 0, sizeof(host_info));
  host_info.ai_family   = AF_UNSPEC;
  host_info.ai_socktype = SOCK_STREAM;

  status = getaddrinfo(hostname, port, &host_info, &host_info_list);
  if (status != 0) {
    cerr << "Error: cannot get address info for host" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return ;
  } //if

  socket_fd = socket(host_info_list->ai_family, 
		     host_info_list->ai_socktype, 
		     host_info_list->ai_protocol);
  if (socket_fd == -1) {
    cerr << "Error: cannot create socket" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return ;
  } //if
  
  cout << "Connecting to " << hostname << " on port " << port << "..." << endl;
  
  status = connect(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
  if (status == -1) {
    cerr << "Error: cannot connect to socket" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return ;
  } //if

  stringstream ss;
  // transaction: order 1, SYM, -1, 50
  ss<<"<?xml version=\"1.0\" encoding=\"UTF-8\"?><transactions id=\"4\">"
  "<order sym=\"SYM\" amount=\"-5\" limit=\"12.8\"/>"
  "</transactions>";
  
  string request = ss.str();
  size_t size = request.length();
  request = to_string(size) + '\n' + request;

  int m = send(socket_fd, request.c_str(), request.size() + 1, 0);
  cout << "client1 send status: " << endl;
  cout << m << endl;

  vector<char> response(MAX_SIZE, 0);
  int len = recv(socket_fd, response.data(), response.size(), 0);
  if(len <= 0) {
    cerr << "Error: no recv!" << endl;
  }
  string res = response.data();
  cout << "Response: " << endl;
  cout << res << endl;
  freeaddrinfo(host_info_list);
  close(socket_fd);
}

void req5(const char *hostname) {
  const char *port     = "12345";
  int status;
  int socket_fd;
  struct addrinfo host_info;
  struct addrinfo *host_info_list;
  memset(&host_info, 0, sizeof(host_info));
  host_info.ai_family   = AF_UNSPEC;
  host_info.ai_socktype = SOCK_STREAM;

  status = getaddrinfo(hostname, port, &host_info, &host_info_list);
  if (status != 0) {
    cerr << "Error: cannot get address info for host" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return ;
  } //if

  socket_fd = socket(host_info_list->ai_family, 
		     host_info_list->ai_socktype, 
		     host_info_list->ai_protocol);
  if (socket_fd == -1) {
    cerr << "Error: cannot create socket" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return ;
  } //if
  
  cout << "Connecting to " << hostname << " on port " << port << "..." << endl;
  
  status = connect(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
  if (status == -1) {
    cerr << "Error: cannot connect to socket" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return ;
  } //if

  stringstream ss;
  // transaction: order 1, SYM, -1, 50
  ss<<"<?xml version=\"1.0\" encoding=\"UTF-8\"?><transactions id=\"5\">"
  "<order sym=\"SYM\" amount=\"-2\" limit=\"14\"/>"
  "</transactions>";
  
  string request = ss.str();
  size_t size = request.length();
  request = to_string(size) + '\n' + request;

  int m = send(socket_fd, request.c_str(), request.size() + 1, 0);
  cout << "client1 send status: " << endl;
  cout << m << endl;

  vector<char> response(MAX_SIZE, 0);
  int len = recv(socket_fd, response.data(), response.size(), 0);
  if(len <= 0) {
    cerr << "Error: no recv!" << endl;
  }
  string res = response.data();
  cout << "Response: " << endl;
  cout << res << endl;
  freeaddrinfo(host_info_list);
  close(socket_fd);
}

void req6(const char *hostname) {
  const char *port     = "12345";
  int status;
  int socket_fd;
  struct addrinfo host_info;
  struct addrinfo *host_info_list;
  memset(&host_info, 0, sizeof(host_info));
  host_info.ai_family   = AF_UNSPEC;
  host_info.ai_socktype = SOCK_STREAM;

  status = getaddrinfo(hostname, port, &host_info, &host_info_list);
  if (status != 0) {
    cerr << "Error: cannot get address info for host" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return ;
  } //if

  socket_fd = socket(host_info_list->ai_family, 
		     host_info_list->ai_socktype, 
		     host_info_list->ai_protocol);
  if (socket_fd == -1) {
    cerr << "Error: cannot create socket" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return ;
  } //if
  
  cout << "Connecting to " << hostname << " on port " << port << "..." << endl;
  
  status = connect(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
  if (status == -1) {
    cerr << "Error: cannot connect to socket" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return ;
  } //if

  stringstream ss;
  // transaction: order 1, SYM, -1, 50
  ss<<"<?xml version=\"1.0\" encoding=\"UTF-8\"?><transactions id=\"6\">"
  "<order sym=\"SYM\" amount=\"4\" limit=\"12.5\"/>"
  "</transactions>";
  
  string request = ss.str();
  size_t size = request.length();
  request = to_string(size) + '\n' + request;

  int m = send(socket_fd, request.c_str(), request.size() + 1, 0);
  cout << "client1 send status: " << endl;
  cout << m << endl;

  vector<char> response(MAX_SIZE, 0);
  int len = recv(socket_fd, response.data(), response.size(), 0);
  if(len <= 0) {
    cerr << "Error: no recv!" << endl;
  }
  string res = response.data();
  cout << "Response: " << endl;
  cout << res << endl;
  freeaddrinfo(host_info_list);
  close(socket_fd);
}

void req7(const char *hostname) {
  const char *port     = "12345";
  int status;
  int socket_fd;
  struct addrinfo host_info;
  struct addrinfo *host_info_list;
  memset(&host_info, 0, sizeof(host_info));
  host_info.ai_family   = AF_UNSPEC;
  host_info.ai_socktype = SOCK_STREAM;

  status = getaddrinfo(hostname, port, &host_info, &host_info_list);
  if (status != 0) {
    cerr << "Error: cannot get address info for host" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return ;
  } //if

  socket_fd = socket(host_info_list->ai_family, 
		     host_info_list->ai_socktype, 
		     host_info_list->ai_protocol);
  if (socket_fd == -1) {
    cerr << "Error: cannot create socket" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return ;
  } //if
  
  cout << "Connecting to " << hostname << " on port " << port << "..." << endl;
  
  status = connect(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
  if (status == -1) {
    cerr << "Error: cannot connect to socket" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return ;
  } //if

  stringstream ss;
  // transaction: order 1, SYM, -1, 50
  ss<<"<?xml version=\"1.0\" encoding=\"UTF-8\"?><transactions id=\"7\">"
  "<order sym=\"SYM\" amount=\"-4\" limit=\"12.4\"/>"
  "</transactions>";
  
  string request = ss.str();
  size_t size = request.length();
  request = to_string(size) + '\n' + request;

  int m = send(socket_fd, request.c_str(), request.size() + 1, 0);
  cout << "client1 send status: " << endl;
  cout << m << endl;

  vector<char> response(MAX_SIZE, 0);
  int len = recv(socket_fd, response.data(), response.size(), 0);
  if(len <= 0) {
    cerr << "Error: no recv!" << endl;
  }
  string res = response.data();
  cout << "Response: " << endl;
  cout << res << endl;
  freeaddrinfo(host_info_list);
  close(socket_fd);
}



int main(int argc, char *argv[])
{
 
  const char *hostname = argv[1];
  
  
  if (argc < 2) {
      cout << "Syntax: client <hostname>\n" << endl;
      return 1;
  }


  

  req1(hostname);
  sleep(1.1);
  req2(hostname);
  sleep(1.1);
  req7(hostname);
  sleep(1.1);
  req3(hostname);
  sleep(1.1);
  req4(hostname);
  sleep(1.1);
  req5(hostname);
  sleep(1.1);
  req6(hostname);


  

  return 0;
}

