#include "Database.h"

//#include <string>
#include <math.h>



void DropTable(connection *C){
    work W(*C);
    std::string sql = "DROP TABLE IF EXISTS ACCOUNT CASCADE;"
          "DROP TABLE IF EXISTS POSITION CASCADE;" 
          "DROP TABLE IF EXISTS OPEN_ORDER CASCADE;" 
          "DROP TABLE IF EXISTS EXEC CASCADE;"
          "DROP TABLE IF EXISTS CANCEL CASCADE;"; 
    W.exec(sql);
    W.commit();
}

void CreateTableA(connection *C){
    work W(*C);
    std::string sql_account = "CREATE TABLE ACCOUNT("
        "ACCOUNT_ID INT PRIMARY KEY NOT NULL,"
        "BALANCE    REAL NOT NULL);";
    W.exec(sql_account);
    W.commit();
}

void CreateTableP(connection *C){
    work W(*C);
    std::string sql_position = "CREATE TABLE POSITION("
        "POSITION_ID    SERIAL,"
        "ACCOUNT_ID     INT     REFERENCES ACCOUNT(ACCOUNT_ID)  NOT NULL,"
        "SYM            VARCHAR NOT NULL,"
        "AMOUNT         INT     NOT NULL,"
        "PRIMARY KEY (account_ID, SYM));";

    W.exec(sql_position);
    W.commit();
}

void CreateTableO(connection *C){
    work W(*C);
    std::string sql_order = "CREATE TABLE OPEN_ORDER("
        "ORDER_ID   SERIAL  PRIMARY KEY,"
        "ACCOUNT_ID INT     REFERENCES ACCOUNT(ACCOUNT_ID)  NOT NULL,"
        "SYM        VARCHAR NOT NULL,"
        "AMOUNT     INT     NOT NULL,"
        "PRICE      REAL    NOT NULL,"
        "TIME       BIGINT  NOT NULL);";
    W.exec(sql_order);
    W.commit();
}

void CreateTableE(connection *C){
    work W(*C);
    std::string sql_exec = "CREATE TABLE EXEC("
        "EXEC_ID    SERIAL  PRIMARY KEY,"
        "BUYER_ID   INT     NOT NULL,"
        "SELLER_ID  INT     NOT NULL,"
        "BUY_ID     INT     NOT NULL,"
        "SELL_ID    INT     NOT NULL,"
        "SYM        VARCHAR NOT NULL,"
        "AMOUNT     INT     NOT NULL,"
        "PRICE      REAL   NOT NULL,"
        "TIME       BIGINT  NOT NULL);";

    W.exec(sql_exec);
    W.commit();
}

void CreateTableC(connection *C){
    work W(*C);
    std::string sql_cancel = "CREATE TABLE CANCEL("
        "CANCEL_ID  SERIAL  PRIMARY KEY,"
        "ACCOUNT_ID INT     NOT NULL,"
        "TRANS_ID   INT     NOT NULL,"
        "SYM        VARCHAR NOT NULL,"
        "AMOUNT     INT     NOT NULL,"
        "TIME       BIGINT  NOT NULL);";

    W.exec(sql_cancel);
    W.commit();
}

int CreateAccount(connection *C, int account_id, float balance){
    work W(*C);
    try {
        //insert new account; [write]
        std::string sql = "INSERT INTO ACCOUNT(ACCOUNT_ID, BALANCE) VALUES(" + to_string(account_id) + ", " + to_string(balance) + ") ON CONFLICT (ACCOUNT_ID) DO NOTHING;";
        W.exec(sql);
        W.commit();
        return 1;
    }catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        W.abort();
        return 0;
    }
}

int CreatePosition(connection *C, int account_id, std::string symbol, int amount){
    //check if position is exist [read], yes->update [write]; no->insert [write]
    //rw->concurrency
    work W(*C);
    try{        
        //lock  FOR UPDATE
        result R = W.exec("SELECT AMOUNT FROM POSITION WHERE ACCOUNT_ID = " + to_string(account_id) + " AND SYM = '" + symbol + "' FOR UPDATE;");
        auto row = R.begin();

        std::string sql = "INSERT INTO position (account_ID, SYM, amount) "
                          "VALUES (" + W.quote(account_id) + ", " + W.quote(symbol) + ", " + W.quote(amount) + ") "
                          "ON CONFLICT (account_ID, SYM) DO UPDATE "
                          "SET amount = position.amount + EXCLUDED.amount;";
        W.exec(sql);
        W.commit();
        return 1;
        /*if(row != R.end()){
            //position exist
            std::string sql = "UPDATE POSITION SET AMOUNT = AMOUNT + " + to_string(amount) + " WHERE ACCOUNT_ID = " + to_string(account_id) + " AND SYM = '" + symbol + "';";
            W.exec(sql);
            W.commit();
            return 1;
        }else{
            //not exist
            std::string sql = "INSERT INTO POSITION(ACCOUNT_ID, SYM, AMOUNT) VALUES(" + to_string(account_id) + ", '" + symbol + "', " + to_string(amount) + ");";
            W.exec(sql);
            W.commit();
            return 1;
        }*/
    }catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        W.abort();
        return 0;
    }
}

int CreateOrder(connection *C, int account_id, std::string symbol, int amount, float price) {
  //insert [write]; update [write]; select [read] ->database concurrency
  //insert open order, then based on buyer or seller update account or position finally return trans_id(order_id)
  int trans_id = -1;
  work W(*C);
  try {
    
    time_t curr = time(NULL);
    std::string sql =
        "INSERT INTO OPEN_ORDER (ACCOUNT_ID, SYM, AMOUNT, PRICE, TIME) VALUES (" +
        to_string(account_id) + ", '" + symbol + "', " + to_string(amount) + ", " +
        to_string(price) + ", " + to_string(curr) + ");";

    W.exec(sql);
    
    if(amount < 0){
        //seller
        //LOCK
        std::string lock_sql = "SELECT AMOUNT FROM POSITION WHERE ACCOUNT_ID = " + to_string(account_id) + " AND SYM = '" + symbol + "' FOR UPDATE;";
        W.exec(lock_sql);
        std::string sql2 = "UPDATE POSITION SET AMOUNT = AMOUNT + " + to_string(amount) + " WHERE ACCOUNT_ID = " + to_string(account_id) + ";";
        W.exec(sql2);
    }else if (amount > 0)
    {
        //buyer
        //LOCK
        std::string lock_sql = "SELECT BALANCE FROM ACCOUNT WHERE ACCOUNT_ID = " + to_string(account_id) + " FOR UPDATE;";
        W.exec(lock_sql);
        std::string sql2 = "UPDATE ACCOUNT SET BALANCE = BALANCE - " + to_string(price * amount) + " WHERE ACCOUNT_ID = " + to_string(account_id) + ";";
        W.exec(sql2);
    }
    
    
    W.commit();
    //get the trans_id
    nontransaction N(*C);
    std::string sql3 = "SELECT ORDER_ID FROM OPEN_ORDER WHERE TIME = " + to_string(curr) + ";";

    result R = N.exec(sql3);
    for (auto row = R.begin(); row != R.end(); ++row) {
      trans_id = row[0].as<int>();
    }
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    W.abort();
  }
  return trans_id;
}

int ExecOrder(connection *C, int trans_id, int account_id, std::string symbol, int amount, float limit){
    // "seller’s limit price ≤ execution price ≤ buyer’s limit price" 
    // could be either seller's price or buyer's price, depends on whose order is older.
    // check amount find it's sell or buy, then to find the match order can be executed
    // select [read] then modify; MatchOrder [write]
    std::vector<float> initial_price;
    std::vector<int> initial_amount;
    work W(*C);
    try{
        
        std::vector<float> &exec_price = initial_price;
        std::vector<int> &exec_amount = initial_amount;
        int sell_id;
        int buy_id;
        int final_amount;
        int seller_id;
        int buyer_id;
        time_t curr = time(NULL);
        if(amount < 0){//a negative amount means to sell
            //seller find buyer order
            //sql = select account_id, symbol from position, amount + share(amount) 
            //lock
            std::string sql = "SELECT ORDER_ID, ACCOUNT_ID, AMOUNT, PRICE FROM OPEN_ORDER WHERE AMOUNT > 0 AND PRICE >= " + to_string(limit) + " AND SYM ='" + symbol + "' AND ACCOUNT_ID != " + to_string(account_id) + " ORDER BY PRICE DESC, TIME ASC FOR UPDATE;";
            result R = W.exec(sql);//R? empty
            auto row = R.begin();
            W.commit();
            for(row ; row != R.end(); ++row){
                buy_id = row[0].as<int>();
                sell_id = trans_id;
                exec_price.push_back(row[3].as<float>());
                exec_amount.push_back(std::min(-amount, row[2].as<int>()));
                
                final_amount = amount + row[2].as<int>();
                buyer_id = row[1].as<int>();
                seller_id = account_id;
                
                // execute match order once
                //////////////
                // std::cout << "test in exec" << std::endl;
                // std::cout << "buy_id: " << buy_id <<"  sell_id: "<< sell_id << std::endl;
                // std::cout << "exec_price: " << exec_price.back() << "   exec_amount: " << exec_amount.back() << std::endl;
                work W2(*C);
                MatchOrder(W2, trans_id, account_id, symbol, amount, limit, exec_price, exec_amount, sell_id, buy_id, seller_id, buyer_id, curr, final_amount, C);
                if(final_amount >= 0)
                    break;
                else{
                    amount = final_amount;}
            }
        }else if(amount > 0){//buy
            //sql = select account_id from account, balance - (amount * limit)
            //lock
            std::string sql = "SELECT ORDER_ID, ACCOUNT_ID, AMOUNT, PRICE FROM OPEN_ORDER WHERE AMOUNT < 0 AND PRICE <= " + to_string(limit) + " AND SYM = '" + symbol + "' AND ACCOUNT_ID !=" + to_string(account_id)  + " ORDER BY PRICE DESC, TIME ASC FOR UPDATE;";
            result R = W.exec(sql);
            W.commit();
            for(auto row = R.begin(); row != R.end(); ++row){
                sell_id = row[0].as<int>();
                buy_id = trans_id;
                // can only select from early order, exec_price depends on select result
                exec_price.push_back(row[3].as<float>());      
                exec_amount.push_back(std::min(amount, -row[2].as<int>()));
                
                final_amount = amount + row[2].as<int>();
                seller_id = row[1].as<int>();
                buyer_id = account_id;
                //buyer's 
                //return balance: update balance = +(price-execprice)*amount from account
                work W2(*C);
                if(limit > exec_price.back()){
                    //LOCK
                    // std::string lock_amount = "SELECT BALANCE FROM ACCOUNT WHERE ACCOUNT_ID= " + to_string(buyer_id) + " FOR UPDATE;";
                    // W.exec(lock_amount);
                    
                    std::string update_refund = "UPDATE ACCOUNT SET BALANCE = BALANCE + " + to_string(limit - exec_price.back()) + " WHERE ACCOUNT_ID = " + to_string(buyer_id) + ";";
                    W2.exec(update_refund);
                }
                
                // execute match order once
                MatchOrder(W2, trans_id, account_id, symbol, amount, limit, exec_price, exec_amount, sell_id, buy_id, seller_id, buyer_id, curr, final_amount, C);
                
                if(final_amount <= 0)
                    break;
                else{
                    amount = final_amount;	}
            }
        }
        
    }catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        W.abort();
        return 0;
    }
    return 1;
}

void MatchOrder(pqxx::work& W, int trans_id, int account_id, std::string symbol, int amount, float limit, std::vector<float> &exec_price, std::vector<int> &exec_amount, int sell_id, int buy_id, int seller_id, int buyer_id, int curr, int final_amount, connection *C){
    //insert EXEC, then modify open_order, then modify seller's and buyer's account or position
    //insert [write]; update[write], delete[write];
    //seller's account: select[read]; update[write]
    //buyer's position: select[read]; update/insert[write]
    //work W(*C);
    try{
        //insert EXEC table
        std::string sql = "INSERT INTO EXEC (BUYER_ID, SELLER_ID, BUY_ID, SELL_ID, SYM, PRICE, AMOUNT, TIME) VALUES (" + to_string(buyer_id) + ", " + to_string(seller_id) + ", " + to_string(buy_id) + ", " + to_string(sell_id) + 
            ", '" + symbol + "', " + to_string(exec_price.back()) +	", " + to_string(exec_amount.back()) + ", " + to_string(curr) + ");";        
        W.exec(sql);
        //W.commit();

        //modify OPEN_ORDER
        if (final_amount < 0) {
            //update sell order and delete buy order            
            //LOCK
            std::string lock_amount = "SELECT AMOUNT FROM OPEN_ORDER WHERE ORDER_ID= " + to_string(sell_id) + " FOR UPDATE;";
            W.exec(lock_amount);
            std::string update_sell_order = "UPDATE OPEN_ORDER SET AMOUNT=" + to_string(final_amount) + " WHERE ORDER_ID=" + to_string(sell_id) + ";";
            std::string delete_buy_order = "DELETE FROM OPEN_ORDER WHERE ORDER_ID = " + to_string(buy_id) + ";";
            
            W.exec(update_sell_order);
            W.exec(delete_buy_order);
            //W2.commit();
        } else if (final_amount > 0) {
            //delete sell order and update buy order
            //LOCK
            std::string lock_amount = "SELECT AMOUNT FROM OPEN_ORDER WHERE ORDER_ID= " + to_string(buy_id) + " FOR UPDATE;";
            W.exec(lock_amount);
            std::string delete_sell_order = "DELETE FROM OPEN_ORDER WHERE ORDER_ID=" + to_string(sell_id) + ";";
            std::string update_buy_order = "UPDATE OPEN_ORDER SET AMOUNT=" + to_string(final_amount) + " WHERE ORDER_ID=" + to_string(buy_id) + ";";
            
            W.exec(delete_sell_order);
            W.exec(update_buy_order);
            //W2.commit();
        } else {
            //erase both order
            std::string delete_sell_order = "DELETE FROM OPEN_ORDER WHERE ORDER_ID=" + to_string(sell_id) + ";";
            std::string delete_buy_order = "DELETE FROM OPEN_ORDER WHERE ORDER_ID=" + to_string(buy_id) + ";";
            W.exec(delete_sell_order);
            W.exec(delete_buy_order);
            //W2.commit();
        }

        //add to seller's balance        
        //lock
        std::string get_seller_balance = "SELECT BALANCE FROM ACCOUNT WHERE ACCOUNT_ID = " + to_string(seller_id) + " FOR UPDATE;";
        result R2 = W.exec(get_seller_balance);
        auto row2 = R2.begin();
        float seller_original_balance = row2[0].as<float>();
        float seller_updated_balance = seller_original_balance + exec_price.back() * exec_amount.back();
        std::string update_seller_acc =	"UPDATE ACCOUNT SET BALANCE=" + to_string(seller_updated_balance) + " WHERE ACCOUNT_ID=" + to_string(seller_id) + ";";
        W.exec(update_seller_acc);
        
        //add amount to buyer's position
        //lock
        std::string get_buyer_position = "SELECT AMOUNT FROM POSITION WHERE ACCOUNT_ID=" + to_string(buyer_id) + " AND SYM='" + symbol + "' FOR UPDATE;";
        result R = W.exec(get_buyer_position);
        
        auto row = R.begin();
        if(row != R.end()){ // update existing position
            int buyer_original_amount = row[0].as<int>();    
            int buyer_updated_amount = buyer_original_amount + exec_amount.back();

            std::string update_buyer_amt = "UPDATE POSITION SET AMOUNT=" + to_string(buyer_updated_amount) + " WHERE ACCOUNT_ID=" + to_string(buyer_id) + " AND SYM='" + symbol + "';";
            
            W.exec(update_buyer_amt);
            W.commit();
        }
        else{ // create new position
            std::string create_new_pos = "INSERT INTO POSITION (ACCOUNT_ID, SYM, AMOUNT) VALUES (" + to_string(buyer_id) + ", '" + symbol + "', " + to_string(exec_amount.back()) + ");";
            
            W.exec(create_new_pos);
            W.commit();
        }
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        W.abort();
    }
    //work W2(*C);
    /*try{    
        //modify OPEN_ORDER
        if (final_amount < 0) {
            //update sell order and delete buy order            
            //LOCK
            std::string lock_amount = "SELECT AMOUNT FROM OPEN_ORDER WHERE ORDER_ID= " + to_string(sell_id) + " FOR UPDATE;";
            W2.exec(lock_amount);
            std::string update_sell_order = "UPDATE OPEN_ORDER SET AMOUNT=" + to_string(final_amount) + " WHERE ORDER_ID=" + to_string(sell_id) + ";";
            std::string delete_buy_order = "DELETE FROM OPEN_ORDER WHERE ORDER_ID = " + to_string(buy_id) + ";";
            
            W2.exec(update_sell_order);
            W2.exec(delete_buy_order);
            W2.commit();
        } else if (final_amount > 0) {
            //delete sell order and update buy order
            //LOCK
            std::string lock_amount = "SELECT AMOUNT FROM OPEN_ORDER WHERE ORDER_ID= " + to_string(buy_id) + " FOR UPDATE;";
            W2.exec(lock_amount);
            std::string delete_sell_order = "DELETE FROM OPEN_ORDER WHERE ORDER_ID=" + to_string(sell_id) + ";";
            std::string update_buy_order = "UPDATE OPEN_ORDER SET AMOUNT=" + to_string(final_amount) + " WHERE ORDER_ID=" + to_string(buy_id) + ";";
            
            W2.exec(delete_sell_order);
            W2.exec(update_buy_order);
            W2.commit();
        } else {
            //erase both order
            std::string delete_sell_order = "DELETE FROM OPEN_ORDER WHERE ORDER_ID=" + to_string(sell_id) + ";";
            std::string delete_buy_order = "DELETE FROM OPEN_ORDER WHERE ORDER_ID=" + to_string(buy_id) + ";";
            W2.exec(delete_sell_order);
            W2.exec(delete_buy_order);
            W2.commit();
        }
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        W2.abort();
    }
    //work W3(*C);
    try{
        //add to seller's balance        
        //lock
        std::string get_seller_balance = "SELECT BALANCE FROM ACCOUNT WHERE ACCOUNT_ID = " + to_string(seller_id) + " FOR UPDATE;";
        result R2 = W3.exec(get_seller_balance);
        auto row2 = R2.begin();
        float seller_original_balance = row2[0].as<float>();
        float seller_updated_balance = seller_original_balance + exec_price.back() * exec_amount.back();
        std::string update_seller_acc =	"UPDATE ACCOUNT SET BALANCE=" + to_string(seller_updated_balance) + " WHERE ACCOUNT_ID=" + to_string(seller_id) + ";";
        W3.exec(update_seller_acc);
        
        //add amount to buyer's position
        //lock
        std::string get_buyer_position = "SELECT AMOUNT FROM POSITION WHERE ACCOUNT_ID=" + to_string(buyer_id) + " AND SYM='" + symbol + "' FOR UPDATE;";
        result R = W3.exec(get_buyer_position);
        
        auto row = R.begin();
        if(row != R.end()){ // update existing position
            int buyer_original_amount = row[0].as<int>();    
            int buyer_updated_amount = buyer_original_amount + exec_amount.back();

            std::string update_buyer_amt = "UPDATE POSITION SET AMOUNT=" + to_string(buyer_updated_amount) + " WHERE ACCOUNT_ID=" + to_string(buyer_id) + " AND SYM='" + symbol + "';";
            
            W3.exec(update_buyer_amt);
            W3.commit();
        }
        else{ // create new position
            std::string create_new_pos = "INSERT INTO POSITION (ACCOUNT_ID, SYM, AMOUNT) VALUES (" + to_string(buyer_id) + ", '" + symbol + "', " + to_string(exec_amount.back()) + ");";
            
            W3.exec(create_new_pos);
            W3.commit();
        }
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        W3.abort();
    }*/
    
}
//return vector struct (both have open and exec)
//all read
std::vector<query_s> QueryOrder(connection *C, int account_id, int trans_id){
    std::vector<query_s> query_results;
    try{
        //open
        //<open shares=.../>
        nontransaction N(*C);
        std::string open_sql = "SELECT AMOUNT FROM OPEN_ORDER WHERE ORDER_ID=" + to_string(trans_id) + ";";
        result R = N.exec(open_sql);
        for (auto row: R) {
            struct query_s s1 = {"open", row[0].as<int>(), 0, 0.0};
            query_results.push_back(s1);
        }
        
        //cancel 
        //<canceled shares=... time=...>
        std::string canceled_sql = "SELECT AMOUNT, TIME FROM CANCEL WHERE TRANS_ID=" + to_string(trans_id) + ";";

        result R2 = N.exec(canceled_sql);
        for (auto row: R2) {
            struct query_s s2 = {"canceled", row[0].as<int>(), row[1].as<long>(), 0.0};
            query_results.push_back(s2);
        }

        //exec
        //<executed shares=... price=... time=.../>
        std::string exec_sql = "SELECT AMOUNT, PRICE, TIME, BUYER_ID, SELLER_ID FROM EXEC WHERE BUY_ID=" + to_string(trans_id) + " OR SELL_ID=" + to_string(trans_id) + ";";
        result R3 = N.exec(exec_sql);
        for (auto row: R3) {
            struct query_s s3 = {"executed", row[0].as<int>(), row[2].as<long>(), row[1].as<float>()};
            query_results.push_back(s3);
        }
    }catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return query_results;
    }
    return query_results;
}

std::vector<query_s> CancelOrder(connection *C, int account_id, int trans_id) {
    std::vector<query_s> query_results;
    work W(*C);
    try {    
        std::string sql = "SELECT AMOUNT, SYM, PRICE, ACCOUNT_ID FROM OPEN_ORDER WHERE ORDER_ID=" + to_string(trans_id) + ";";
        result R = W.exec(sql);
        
        for (auto row : R) {
            // std::cout << "result in R:" << std::endl;
            // std::cout << row[0].as<int>() << std::endl;
            //std::cout << row[1].as<int>() << std::endl;
            //std::cout << row[5].as<long>() << std::endl;
            time_t curr_time = time(nullptr);
            // Add to cancel list
            struct query_s s1 = {"canceled", row[0].as<int>(), static_cast<long>(curr_time), 0.0};
            query_results.push_back(s1);

            //insert canceled order into CANCEL
            std::string sql2 = "INSERT INTO CANCEL (ACCOUNT_ID, TRANS_ID, SYM, AMOUNT, TIME) VALUES ('" +
                to_string(row[3].as<int>()) + "', '" + to_string(trans_id) + "', '" + row[1].as<std::string>() + "', '" + to_string(row[0].as<int>()) + "', '" + to_string(curr_time) + "');";
            W.exec(sql2);
            if(row[0].as<int>() < 0){
                //sell amount
                //LOCK
                std::string lock_amount = "SELECT AMOUNT FROM POSITION WHERE ACCOUNT_ID = " + to_string(row[3].as<int>()) + " FOR UPDATE;";
                W.exec(lock_amount);

                std::string return_seller = "UPDATE POSITION SET AMOUNT = AMOUNT - " + to_string(row[0].as<int>()) + " WHERE ACCOUNT_ID = " + to_string(row[3].as<int>()) + ";";
                W.exec(return_seller);
            }else{
                //LOCK
                std::string lock_balance = "SELECT BALANCE FROM ACCOUNT WHERE ACCOUNT_ID = " + to_string(row[3].as<int>()) + " FOR UPDATE;";
                W.exec(lock_balance);

                std::string return_buyer = "UPDATE ACCOUNT SET BALANCE =  BALANCE + " + to_string(row[2].as<float>() * row[0].as<int>()) + " WHERE ACCOUNT_ID = " + to_string(row[3].as<int>()) + ";";
                W.exec(return_buyer);
            }

            //remove from OPEN_ORDER
            std::string delete_sql = "DELETE FROM OPEN_ORDER WHERE ORDER_ID=" + to_string(trans_id) + ";";
            W.exec(delete_sql);
        }
        W.commit();

        nontransaction N(*C);
        std::string exec_sql = "SELECT AMOUNT, PRICE, TIME FROM EXEC WHERE BUY_ID='" + to_string(trans_id) + "' OR SELL_ID='" + to_string(trans_id) + "';";
        result R2 = N.exec(exec_sql);
        for (auto row : R2) {
            // Add to executed list
            struct query_s s2 = {"executed", row[0].as<int>(), row[2].as<long>(), row[1].as<float>()};
            query_results.push_back(s2);
        }

    } catch (const std::exception &e) {
        std::cerr << "Database cancel failed: " << e.what() << std::endl;
        W.abort();
        // Optionally handle exception, e.g., return an empty vector or log the error
    }

    return query_results;
}

bool checkAccountID(connection *C, int account_id){
    //check if the account is exist, yes return true, no return false
    
    try{
        nontransaction N(*C);
        result R = N.exec("SELECT ACCOUNT_ID FROM ACCOUNT WHERE ACCOUNT_ID = " + to_string(account_id) + ";");
        auto row = R.begin();
        if(row != R.end()){
            return true;
        }else{
            return false;
        }
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        
    }
    return false;
}
//balance > amount * limit
bool checkBuyerMoney(connection *C, int account_id, int amount, float limit){
    try {
        nontransaction N(*C);
        std::string sql = "SELECT BALANCE FROM ACCOUNT WHERE ACCOUNT_ID = " + to_string(account_id) + ";";
        result R = N.exec(sql);
        auto row = R.begin();
        int balance = row[0].as<float>();
        if(balance >= (amount * limit)){
            return true;
        }else{
            return false;
        }
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        
    }
    return false;
    
}

bool checkSellerShare(connection *C, int account_id, std::string symbol, int amount){
    try {
        nontransaction N(*C);
        std::string sql = "SELECT AMOUNT FROM POSITION WHERE ACCOUNT_ID=" + to_string(account_id) + " AND SYM='" + symbol + "';";
        result R = N.exec(sql);
            
        auto row = R.begin();
        int seller_amount = row[0].as<int>();  
        if(seller_amount >= amount){
            return true;
        }else{
            return false;
        }
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        
    }
    return false;
      
}

bool checkTransID(connection *C, int trans_ID){
    
    try{
        nontransaction N(*C);
        result R = N.exec("SELECT TRANS_ID FROM CANCEL WHERE TRANS_ID = " + to_string(trans_ID) + ";");
        result R1 = N.exec("SELECT ORDER_ID FROM OPEN_ORDER WHERE ORDER_ID = " + to_string(trans_ID) + ";");
        result R2 = N.exec("SELECT BUY_ID FROM EXEC WHERE BUY_ID = " + to_string(trans_ID) + ";");
        result R3 = N.exec("SELECT SELL_ID FROM EXEC WHERE SELL_ID = " + to_string(trans_ID) + ";");
        auto row = R.begin();
        auto row1 = R1.begin();
        auto row2 = R2.begin();
        auto row3 = R3.begin();
        if(row != R.end() || row2 != R2.end() || row3 != R3.end() || row1 != R1.end()){
            return true;
        }else{
            return false;
        }
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        
    }
    return false;
}

bool checkIfOpen(connection *C, int trans_ID){
    try {
        nontransaction N(*C);
        std::string sql = "SELECT ORDER_ID FROM OPEN_ORDER WHERE ORDER_ID=" + to_string(trans_ID) +  ";";
        result R = N.exec(sql);
            
        auto row = R.begin(); 
        if(row != R.end()){
            return true;
        }else{
            return false;
        }
    }catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        
    }
    return false;
      
}

bool checkBelong(connection *C, int account_id, int trans_ID){
    
    try{
        nontransaction N(*C);
        result R = N.exec("SELECT TRANS_ID, ACCOUNT_ID FROM CANCEL WHERE TRANS_ID = " + to_string(trans_ID) + ";");
        result R1 = N.exec("SELECT ORDER_ID, ACCOUNT_ID FROM OPEN_ORDER WHERE ORDER_ID = " + to_string(trans_ID) + ";");
        result R2 = N.exec("SELECT BUY_ID, BUYER_ID FROM EXEC WHERE BUY_ID = " + to_string(trans_ID) + ";");
        result R3 = N.exec("SELECT SELL_ID, SELLER_ID FROM EXEC WHERE SELL_ID = " + to_string(trans_ID) + ";");
        auto row = R.begin();
        auto row1 = R1.begin();
        auto row2 = R2.begin();
        auto row3 = R3.begin();
        for (auto row: R) {
            //cancel
            if (row[1].as<int>() != account_id) {
                //std::cout << "in cancel: " << "row[1]= " << row[1].as<int>() << " account_id= "<<account_id << std::endl;
                return false;
            }
        }
        for (auto row1: R1) {
            //open
            if (row1[1].as<int>() != account_id) {
                //std::cout << "in open: " << "row[1]= " << row[1].as<int>() << " account_id= "<<account_id << std::endl;
                return false;
            }
        }
        for (auto row2: R2) {
            //buyer
            if (row2[1].as<int>() != account_id) {
                //std::cout << "in buyer: " << "row[1]= " << row[1].as<int>() << " account_id= "<<account_id << std::endl;
                return false;
            }
        }
        for (auto row3: R3) {
            //seller
            if (row3[1].as<int>() != account_id) {
                //std::cout << "in seller: " << "row[1]= " << row[1].as<int>() << " account_id= "<<account_id << std::endl;
                return false;
            }
        }
        return true;
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        
    }
    return true;
}
