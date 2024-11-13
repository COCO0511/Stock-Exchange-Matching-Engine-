#ifndef __REQUEST_H__
#define __REQUEST_H__

#include <cstdio>
#include <cstdlib>
#include <string>
#include <cstring>
#include <vector>
#include <iostream>
#include "tinyxml2.h"
#include "Database.h" 

using namespace tinyxml2;
using namespace std;

class Sub {
    public:
    virtual void execute(connection *C, XMLDocument & response) = 0;
    virtual void replySuccess(connection *C, XMLDocument & response) = 0;
    virtual void replyError(connection *C, XMLDocument & response, string info) = 0;
    virtual void display() = 0;
};

class Request {
    public:
    std::vector<Sub *> subrequest;
    XMLDocument response;

    public: 
    Request() {
        /* <?xml version="1.0" encoding="UTF-8"?> */
        tinyxml2::XMLDeclaration * declaration = response.NewDeclaration();
        response.InsertFirstChild(declaration);
        /* <results></results> */
        XMLElement * root = response.NewElement("results");
        response.InsertEndChild(root);
    }
    ~Request() {
        for (int i = 0; i < subrequest.size(); i++) {
            delete subrequest[i];
        }
    }
    void execute(connection *C) {
        for (int i = 0; i < subrequest.size(); i++) {
            subrequest[i]->execute(C, response);
        }
    }
    void display() {
        for (int i = 0; i < subrequest.size(); i++) {
            subrequest[i]->display();
        }
    }
    string convertResToStr() {
        XMLPrinter p;
        response.Print(&p);
        string ss = p.CStr();
        size_t size = ss.length();
        ss = to_string(size) + '\n' + ss;
        return ss;
    }
};



class Account : public Sub {
    private:
    int id;
    float balance;

    public:
    Account(int id, float balance) : id(id), balance(balance) {}
    virtual void execute(connection *C, XMLDocument & response);
    virtual void replySuccess(connection *C, XMLDocument & response);
    virtual void replyError(connection *C, XMLDocument & response, string info);
    virtual void display() {
        std::cout << "Account" << std::endl;
        std::cout << "id: " << id << std::endl;
        std::cout << "balance: " << balance << std::endl;
    }
};

class Symbol : public Sub {
    private:
    string sym;
    int id;
    int num;

    public:
    Symbol(string sym, int id, int num) : sym(sym), id(id), num(num) {}
    virtual void execute(connection *C, XMLDocument & response);
    virtual void replySuccess(connection *C, XMLDocument & response);
    virtual void replyError(connection *C, XMLDocument & response, string info);
    virtual void display() {
        std::cout << "Symbol" << std::endl;
        std::cout << "sym: " << sym << std::endl;
        std::cout << "id: " << id << std::endl;
        std::cout << "num: " << num << std::endl;
    }
};

class Order : public Sub {
    private:
    int account_id;
    string sym;
    int amount;
    float limit;
    int trans_id;

    public:
    Order(int account_id, string sym, int amount, float limit) : account_id(account_id), sym(sym), amount(amount), limit(limit) {}
    virtual void execute(connection *C, XMLDocument & response);
    virtual void replySuccess(connection *C, XMLDocument & response);
    virtual void replyError(connection *C, XMLDocument & response, string info);
    virtual void display() {
        std::cout << "Order" << std::endl;
        std::cout << "account_id: " << account_id << std::endl;
        std::cout << "sym: " << sym << std::endl;
        std::cout << "amount: " << amount << std::endl;
        std::cout << "limit: " << limit << std::endl;
        std::cout << "trans_id: " << trans_id << std::endl;
    }
};

class Query : public Sub {
    private:
    int account_id;
    int trans_id;
    string state; //open, canceled, executed
    int shares;
    float price;
    long time;

    public:
    Query(int account_id, int trans_id) : account_id(account_id), trans_id(trans_id) {}
    virtual void execute(connection *C, XMLDocument & response);
    virtual void replySuccess(connection *C, XMLDocument & response);
    virtual void replyError(connection *C, XMLDocument & response, string info);
    virtual void display() {
        std::cout << "Query" << std::endl;
        std::cout << "account_id: " << account_id << std::endl;
        std::cout << "trans_id: " << trans_id << std::endl;
        std::cout << "state: " << state << std::endl;
        std::cout << "shares: " << shares << std::endl;
        std::cout << "price: " << price << std::endl;
        std::cout << "time: " << time << std::endl;
    }
};

class Cancel : public Sub {
    private:
    int account_id;
    int trans_id;
    string state; //open, canceled, executed
    int shares;
    float price;
    long time;

    public:
    Cancel(int account_id, int trans_id) : account_id(account_id), trans_id(trans_id) {}
    virtual void execute(connection *C, XMLDocument & response);
    virtual void replySuccess(connection *C, XMLDocument & response);
    virtual void replyError(connection *C, XMLDocument & response, string info);
    virtual void display() {
        std::cout << "Cancel" << std::endl;
        std::cout << "account_id: " << account_id << std::endl;
        std::cout << "trans_id: " << trans_id << std::endl;
        std::cout << "state: " << state << std::endl;
        std::cout << "shares: " << shares << std::endl;
        std::cout << "price: " << price << std::endl;
        std::cout << "time: " << time << std::endl;
    }
};

#endif