#include "sql.h"
#include "Database.h"
#include "server.h"
#include "threadpool.h"

pthread_mutex_t cmutex = PTHREAD_MUTEX_INITIALIZER;

void start(const char* port) {
    Server server(port);
    int state = server.createServer();
    if (state < 0) {
      std::cerr << "Error: Failed to create server!" << std::endl;
      return;
    }
    
    int thread_id = 0;
    int user_fd;
    std::string ip;

    ThreadPool pool(10);

    while (true) {
      pthread_mutex_lock(&cmutex);
      user_fd = server.acceptClient(&ip);
      if (user_fd < 0) {
        std::cerr << "Error: Failed to accept client connection!" << std::endl;
      }

      pthread_t thread;

      User* user = new User(user_fd, thread_id, ip);
      thread_id++;
      pthread_mutex_unlock(&cmutex);

      //pthread_create(&thread, NULL, Server::handleRequest, user);
      pool.enqueue([](void * userInfo) {Server::handleRequest(userInfo);}, (void * )user);
    }
}

int main() {
    try {
        connection *C = connect_db();
        DropTable(C);
        CreateTableA(C);
        CreateTableP(C);
        CreateTableO(C);
        CreateTableE(C);
        CreateTableC(C);
        
        disconnect_db(C);
        delete C;

        const char* port = "12345";
        start(port);

        return 0;
        
    } catch (const std::exception &e){
        cerr << e.what() << std::endl;
        return 1;
    }
}