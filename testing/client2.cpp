#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <sstream>
#include <vector>

#define MAX_SIZE 65536

using namespace std;

int main(int argc, char *argv[])
{
  int status;
  int socket_fd;
  struct addrinfo host_info;
  struct addrinfo *host_info_list;
  const char *hostname = argv[1];
  const char *port     = "12345";
  
  if (argc < 2) {
      cout << "Syntax: client <hostname>\n" << endl;
      return 1;
  }

  memset(&host_info, 0, sizeof(host_info));
  host_info.ai_family   = AF_UNSPEC;
  host_info.ai_socktype = SOCK_STREAM;

  status = getaddrinfo(hostname, port, &host_info, &host_info_list);
  if (status != 0) {
    cerr << "Error: cannot get address info for host" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return -1;
  } //if

  socket_fd = socket(host_info_list->ai_family, 
		     host_info_list->ai_socktype, 
		     host_info_list->ai_protocol);
  if (socket_fd == -1) {
    cerr << "Error: cannot create socket" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return -1;
  } //if
  
  cout << "Connecting to " << hostname << " on port " << port << "..." << endl;
  
  status = connect(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
  if (status == -1) {
    cerr << "Error: cannot connect to socket" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return -1;
  } //if


  //stringstream ss;
  // create: account 1, 100  symbol 1, SYM, 100
  //ss<<"<?xml version=\"1.0\" encoding=\"UTF-8\"?><create><account id=\""<<1<<"\" balance=\""<<100<<"\"/><symbol sym=\"SYM\"><account id=\""<<1<<"\">100</account></symbol></create>";
  
  // create: account 1-7, 100  symbol 1, SYM, 100  <create><account id=\"1\" balance=\"100\"/>  <account id=\"1\">100</account>
  stringstream ss;
  ss<<"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"  
  "<create><account id=\"1\" balance=\"100\"/>"  
  "<account id=\"2\" balance=\"100\"/>"  
  "<account id=\"3\" balance=\"100\"/>"  
  "<account id=\"4\" balance=\"100\"/>"  
  "<account id=\"5\" balance=\"100\"/>"  
  "<account id=\"6\" balance=\"100\"/>"  
  "<account id=\"7\" balance=\"100\"/>"  
  "<symbol sym=\"SYM\"><account id=\"2\">5</account>"  
  "<account id=\"4\">10</account>"  
  "<account id=\"5\">8</account>"  
  "<account id=\"7\">10</account>"  
  "</symbol></create>";
  
  // transaction: order 1, SYM, -1, 50
  //ss<<"<?xml version=\"1.0\" encoding=\"UTF-8\"?><transactions id=\"1\"><order sym=\"SYM\" amount=\"-1\" limit=\"50\"/></transactions>";
  
  // transaction: query 1, 1
  //ss<<"<?xml version=\"1.0\" encoding=\"UTF-8\"?><transactions id=\"1\"><query id=\"1\"/></transactions>";
  
  // transaction: cancel 1, 1
  //ss<<"<?xml version=\"1.0\" encoding=\"UTF-8\"?><transactions id=\"1\"><cancel id=\"1\"/></transactions>";
  string request = ss.str();
  size_t size = request.length();
  request = to_string(size) + '\n' + request;
  //const char *message = "hi there!";
  send(socket_fd, request.c_str(), request.size() + 1, 0);

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

  return 0;
}
