#include "server.h"
#define MAX_SIZE 65536


int Server::createServer() {
  memset(&host_info, 0, sizeof(host_info));

  host_info.ai_family = AF_UNSPEC;
  host_info.ai_socktype = SOCK_STREAM;
  host_info.ai_flags = AI_PASSIVE;

  status = getaddrinfo(hostname, port, &host_info, &host_info_list);
  if (status != 0) {
    cerr << "Error: cannot get address info for host --server" << endl;
    return -1;
  }

  socket_fd = socket(host_info_list->ai_family,
    host_info_list->ai_socktype,
    host_info_list->ai_protocol);
  if (socket_fd == -1) {
    cerr << "Error: cannot create socket" << endl;
    return -1;
  }

  int yes = 1;
  status = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
  status = bind(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
  if (status == -1) {
    cerr << "Error: cannot bind socket" << endl;
    return -1;
  }

  status = listen(socket_fd, 100);
  if (status == -1) {
    cerr << "Error: cannot listen on socket" << endl;
    return -1;
  }
  return socket_fd;
}

int Server::acceptClient(std::string* ip) {
  struct sockaddr_storage socket_addr;
  socklen_t socket_addr_len = sizeof(socket_addr);
  int client_connection_fd;
  client_connection_fd = accept(socket_fd, (struct sockaddr*)&socket_addr, &socket_addr_len);
  if (client_connection_fd == -1) {
    cerr << "Error: cannot accept connection on socket" << endl;
    return -1;
  }
  struct sockaddr_in* addr = (struct sockaddr_in*)&socket_addr;
  struct in_addr addrin = addr->sin_addr;
  char* client_ip = inet_ntoa(addrin);
  *ip = client_ip;
  return client_connection_fd;
}

/*void Server::run() {
  int state = createServer();
    if (state < 0) {
      std::cerr << "Error: Failed to create server!" << std::endl;
      return;
    }
    
    int thread_id = 0;
    int user_fd;
    std::string ip;

    while (true) {
      user_fd = acceptClient(&ip);
      if (user_fd < 0) {
        std::cerr << "Error: Failed to accept client connection!" << std::endl;
      }

      pthread_t thread;

      pthread_mutex_lock(&mutex);
      User* user = new User(user_fd, thread_id, ip);
      thread_id++;
      pthread_mutex_unlock(&mutex);

      pthread_create(&thread, NULL, handleRequest, user);
    }
}*/

void* Server::handleRequest(void* userInfo) {
    User* user = (User*)userInfo;
    vector<char> message(MAX_SIZE, 0);
    string response;
    int status = recv(user->getClientFd(), message.data(), message.size(), 0);
    if (status <= 0) {
      std::cerr << "ERROR: Invalid receive!" << std::endl;
      return NULL;
    }
    std::string req = message.data();
    try {
      parseXML(req, response);
      /////////////////////
      /*std::cout << "res in handleRequest" << std::endl;
      std::cout << response << std::endl;*/
      int i = send(user->getClientFd(), response.c_str(), response.size(), 0);
      ////////////
      /*std::cout << "send status" << std::endl;
      std::cout << i << std::endl;*/
    } catch (MyException& e) {
        std::cerr << e.what() << std::endl;
        return NULL;
    }
    close(user->getClientFd());
    delete user;
    return NULL;
}

