#include "client.h"


int Client::createClient() {
  memset(&host_info, 0, sizeof(host_info));
  host_info.ai_family = AF_UNSPEC;
  host_info.ai_socktype = SOCK_STREAM;
  cerr << "before:  hostname:" << hostname << endl;
  cerr << "before:  port:" << port << endl;
  status = getaddrinfo(hostname, port, &host_info, &host_info_list);
  if (status != 0) {
    cerr << "Error: cannot get address info for host --client " << gai_strerror(status) << endl;
    cerr << "hostname:" << hostname << endl;
    cerr << "port:" << port << endl;
    return -1;
  }

  socket_fd = socket(host_info_list->ai_family,
    host_info_list->ai_socktype,
    host_info_list->ai_protocol);
  if (socket_fd == -1) {
    cerr << "Error: cannot create socket" << endl;
    return -1;
  }

  status = connect(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
  if (status == -1) {
    cerr << "Error: cannot connect to socket" << endl;
    return -1;
  }
  cout << "success connect!" << endl;
  return socket_fd;
}