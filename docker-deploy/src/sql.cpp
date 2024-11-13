#include "sql.h"

connection * connect_db(){
    connection *C;

    try{
        C = new connection("dbname=matching user=postgres password=passw0rd host=db");
        if (C->is_open()) {
            //cout << "Opened database successfully: " << C->dbname() << endl;
            return C;
        } else {
            cout << "Can't open database" << endl;
            return nullptr;
        }
    } catch (const std::exception &e){
        cerr << e.what() << std::endl;
        return nullptr;
    }
}

void disconnect_db(connection *C) {
    C->disconnect();
}