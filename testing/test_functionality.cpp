#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <sstream>
#include <vector>

#define MAX_SIZE 65536

using namespace std;

void testCreate(vector<string> & request_list) {
  //create: account 1, 100  symbol 1, SYM, 100
  stringstream ss;
  ss<<"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<create>"
  "<account id=\""<<1<<"\" balance=\""<<100<<"\"/>"
  "<symbol sym=\"SYM\"><account id=\""<<1<<"\">100"
  "</account></symbol></create>";
  string request = ss.str();
  size_t size = request.length();
  request = to_string(size) + '\n' + request;
  request_list.push_back(request);
}

void testOrder(vector<string> & request_list) {
  // transaction: order 1, SYM, -5, 80
  stringstream ss;
  ss<<"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<transactions id=\"1\">"
  "<order sym=\"SYM\" amount=\"-5\" limit=\"80\"/>"
  "</transactions>";
  string request = ss.str();
  size_t size = request.length();
  request = to_string(size) + '\n' + request;
  request_list.push_back(request);
}

void testQuery(vector<string> & request_list) {
  // transaction: query 1, 1
  stringstream ss;
  ss<<"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<transactions id=\"1\">"
  "<query id=\"1\"/>"
  "</transactions>";
  string request = ss.str();
  size_t size = request.length();
  request = to_string(size) + '\n' + request;
  request_list.push_back(request);
}

void testCancel(vector<string> & request_list) {
  // transaction: cancel 1, 1   
  stringstream ss;
  ss<<"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<transactions id=\"1\">"
  "<cancel id=\"1\"/>"
  "</transactions>";
  string request = ss.str();
  size_t size = request.length();
  request = to_string(size) + '\n' + request;
  request_list.push_back(request);
}

void testMatchOrder1(vector<string> & request_list) {
  // the same example in HW4 readme.pdf
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
  string request = ss.str();
  size_t size = request.length();
  request = to_string(size) + '\n' + request;
  request_list.push_back(request);
}

void testMatchOrder1_1(vector<string> & request_list) {
  // transaction: order 1, SYM, 3, 12.5
  stringstream ss;
  ss<<"<?xml version=\"1.0\" encoding=\"UTF-8\"?><transactions id=\"1\">"
  "<order sym=\"SYM\" amount=\"3\" limit=\"12.5\"/>"
  "</transactions>";
  string request = ss.str();
  size_t size = request.length();
  request = to_string(size) + '\n' + request;
  request_list.push_back(request);
}
void testMatchOrder1_2(vector<string> & request_list) {
  // transaction: order 2, SYM, -1, 13
  stringstream ss;
  ss<<"<?xml version=\"1.0\" encoding=\"UTF-8\"?><transactions id=\"2\">"
  "<order sym=\"SYM\" amount=\"-1\" limit=\"13\"/>"
  "</transactions>";
  string request = ss.str();
  size_t size = request.length();
  request = to_string(size) + '\n' + request;
  request_list.push_back(request);
}
void testMatchOrder1_3(vector<string> & request_list) {
  // transaction: order 3, SYM, 2, 12.7
  stringstream ss;
  ss<<"<?xml version=\"1.0\" encoding=\"UTF-8\"?><transactions id=\"3\">"
  "<order sym=\"SYM\" amount=\"2\" limit=\"12.7\"/>"
  "</transactions>";
  string request = ss.str();
  size_t size = request.length();
  request = to_string(size) + '\n' + request;
  request_list.push_back(request);
}
void testMatchOrder1_4(vector<string> & request_list) {
  // transaction: order 4, SYM, -5, 12.8
  stringstream ss;
  ss<<"<?xml version=\"1.0\" encoding=\"UTF-8\"?><transactions id=\"4\">"
  "<order sym=\"SYM\" amount=\"-5\" limit=\"12.8\"/>"
  "</transactions>";
  string request = ss.str();
  size_t size = request.length();
  request = to_string(size) + '\n' + request;
  request_list.push_back(request);
}
void testMatchOrder1_5(vector<string> & request_list) {
  // transaction: order 5, SYM, -2, 14
  stringstream ss;
  ss<<"<?xml version=\"1.0\" encoding=\"UTF-8\"?><transactions id=\"5\">"
  "<order sym=\"SYM\" amount=\"-2\" limit=\"14\"/>"
  "</transactions>";
  string request = ss.str();
  size_t size = request.length();
  request = to_string(size) + '\n' + request;
  request_list.push_back(request);
}
void testMatchOrder1_6(vector<string> & request_list) {
  // transaction: order 6, SYM, 4, 12.5
  stringstream ss;
  ss<<"<?xml version=\"1.0\" encoding=\"UTF-8\"?><transactions id=\"6\">"
  "<order sym=\"SYM\" amount=\"4\" limit=\"12.5\"/>"
  "</transactions>";
  string request = ss.str();
  size_t size = request.length();
  request = to_string(size) + '\n' + request;
  request_list.push_back(request);
}
void testMatchOrder1_7(vector<string> & request_list) {
  // transaction: order 7, SYM, -4, 12.4
  stringstream ss;
  ss<<"<?xml version=\"1.0\" encoding=\"UTF-8\"?><transactions id=\"7\">"
  "<order sym=\"SYM\" amount=\"-4\" limit=\"12.4\"/>"
  "</transactions>";
  string request = ss.str();
  size_t size = request.length();
  request = to_string(size) + '\n' + request;
  request_list.push_back(request);
}

void testMatchOrder1_8(vector<string> & request_list) {
  // transaction: query 3, 4
  stringstream ss;
  ss<<"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<transactions id=\"3\">"
  "<query id=\"4\"/>"
  "</transactions>";
  string request = ss.str();
  size_t size = request.length();
  request = to_string(size) + '\n' + request;
  request_list.push_back(request);
}

void testMatchOrder1_9(vector<string> & request_list) {
  // transaction: query 7, 7
  stringstream ss;
  ss<<"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<transactions id=\"7\">"
  "<query id=\"7\"/>"
  "</transactions>";
  string request = ss.str();
  size_t size = request.length();
  request = to_string(size) + '\n' + request;
  request_list.push_back(request);
}

void testCreateError(vector<string> & request_list) {
  //create: account 1, 100 ---again  symbol 1 right; 2---not exist, SYM, 100
  stringstream ss;
  ss<<"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<create>"
  "<account id=\""<<1<<"\" balance=\""<<100<<"\"/>"
  "<symbol sym=\"SYM\">"
  "<account id=\""<<1<<"\">100</account>"
  "<account id=\""<<2<<"\">100</account>"
  "</symbol></create>";
  string request = ss.str();
  size_t size = request.length();
  request = to_string(size) + '\n' + request;
  request_list.push_back(request);
}

void testTransAccountError(vector<string> & request_list) {
  // transaction: account_id=99 not exist
  stringstream ss;
  ss<<"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<transactions id=\"99\">"
  "<order sym=\"SYM\" amount=\"-5\" limit=\"80\"/>"
  "<query id=\"1\"/>"
  "<cancel id=\"1\"/>"
  "</transactions>";
  string request = ss.str();
  size_t size = request.length();
  request = to_string(size) + '\n' + request;
  request_list.push_back(request);
}

void testTransIDError(vector<string> & request_list) {
  // transaction: id=1    query/cancel id 33 not exist
  stringstream ss;
  ss<<"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<transactions id=\"1\">"
  "<order sym=\"SYM\" amount=\"5\" limit=\"80\"/>"
  "<query id=\"33\"/>"
  "<cancel id=\"33\"/>"
  "</transactions>";
  string request = ss.str();
  size_t size = request.length();
  request = to_string(size) + '\n' + request;
  request_list.push_back(request);
}

void testNoneToCancelError(vector<string> & request_list) {
  // transaction: id=1    cancel_id = 1 cancel twice
  stringstream ss;
  ss<<"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<transactions id=\"1\">"
  "<cancel id=\"1\"/>"
  "</transactions>";
  string request = ss.str();
  size_t size = request.length();
  request = to_string(size) + '\n' + request;
  request_list.push_back(request);
}

int main(int argc, char *argv[])
{
  const char *hostname = argv[1];
  if (argc < 2) {
      cout << "Syntax: client <hostname>\n" << endl;
      return 1;
  }

  vector<string> request_list;
  // choose the test type
  // 1. basic test
  /*testCreate(request_list);
  testOrder(request_list);
  testQuery(request_list);
  testCancel(request_list);*/

  // 2. match test_1 [the same example in HW4 readme.pdf]
  testMatchOrder1(request_list);
  testMatchOrder1_1(request_list);
  testMatchOrder1_2(request_list);
  testMatchOrder1_3(request_list);
  testMatchOrder1_4(request_list);
  testMatchOrder1_5(request_list);
  testMatchOrder1_6(request_list);
  testMatchOrder1_7(request_list);
  testMatchOrder1_8(request_list);
  testMatchOrder1_9(request_list);

  // 3. match test_2 [change the order of the open_order, include refund]
  /*testMatchOrder1(request_list);
  testMatchOrder1_1(request_list);
  testMatchOrder1_2(request_list);
  testMatchOrder1_7(request_list);
  testMatchOrder1_3(request_list);
  testMatchOrder1_4(request_list);
  testMatchOrder1_5(request_list);
  testMatchOrder1_6(request_list);
  testMatchOrder1_8(request_list);
  testMatchOrder1_9(request_list);*/

  // 4. create_error test
  /*testCreate(request_list);
  testCreateError(request_list);*/

  // 5. trans_error test: account_id not exist
  //testTransAccountError(request_list);

  // 6. trans_error test: trans_id not exist; account_id doesn't have enough money
  // testCreate(request_list);
  // testTransIDError(request_list);

  // 7. cancel twice, no open_order to cancel
  // testCreate(request_list);
  // testOrder(request_list);
  // testNoneToCancelError(request_list);
  // testNoneToCancelError(request_list);
  


  for (int i = 0; i < request_list.size(); ++i) {
    int status;
    int socket_fd;
    struct addrinfo host_info;
    struct addrinfo *host_info_list;
    const char *port     = "12345";

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
    } 

    
    send(socket_fd, request_list[i].c_str(), request_list[i].size() + 1, 0);

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

  return 0;
}
