#ifndef __SQL_H__
#define __SQL_H__

#include <iostream>
#include <pqxx/pqxx>

using namespace std;
using namespace pqxx;

connection * connect_db();
void disconnect_db(connection *C);

#endif