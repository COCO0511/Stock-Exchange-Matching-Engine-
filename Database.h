#include <iostream>
//#include <fstream>
#include <pqxx/pqxx>
#include "struct.h"
#include <vector>

using namespace pqxx;

void DropTable(connection *C);
void CreateTableA(connection *C);
void CreateTableP(connection *C);
void CreateTableO(connection *C);
void CreateTableE(connection *C);
void CreateTableC(connection *C);
int CreateAccount(connection *C, int account_id, float balance);//addAccount
int CreatePosition(connection *C, int account_id, std::string symbol, int amount);
int CreateOrder(connection *C, int account_id, std::string symbol, int amount, float price);
int ExecOrder(connection *C, int trans_id, int account_id, std::string symbol, int amount, float limit);
void MatchOrder(pqxx::work& W, int trans_id, int account_id, std::string symbol, int amount, float limit, std::vector<float> &exec_price, std::vector<int> &exec_amount, int sell_id, int buy_id, int seller_id, int buyer_id, int curr, int final_amount, connection *C);
std::vector<query_s> QueryOrder(connection *C, int account_id, int trans_id);
std::vector<query_s> CancelOrder(connection *C, int account_id, int trans_id);
bool checkAccountID(connection *C, int account_id);//int 
bool checkBuyerMoney(connection *C, int account_id, int amount, float limit);
bool checkSellerShare(connection *C, int account_id, std::string symbol, int amount);
bool checkTransID(connection *C, int trans_ID);
bool checkIfOpen(connection *C, int trans_ID);
bool checkBelong(connection *C, int account_id, int trans_ID);
//check account_id valid, return true/false
    //check buyermoney(balance > amount * limit)(account_id, amount, limit), return bool
    //check sellershare(amount )(account_id, symbol, amount), return bool
    //checkTransID(trans_ID) if exist, return bool

    //match logic
    //match one order
    //amount in buyer more than seller(hold left)
    


//Account: connect to db
