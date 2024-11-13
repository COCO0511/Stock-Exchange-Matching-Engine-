#include "request.h"

void Account::execute(connection *C, XMLDocument & response) {
    try {
        bool exist = checkAccountID(C, id);
        if (exist) {
            string info = "The account_id already exists! Request(create account) reject!";
            replyError(C, response, info);
            return;
        }
        CreateAccount(C, id, balance);
    } catch (const std::exception &e){
        replyError(C, response, e.what());
        return;
    }
    replySuccess(C, response);
}

void Account::replySuccess(connection *C, XMLDocument & response) {
    /* <created id="ACCOUNT_ID"/> */
    XMLElement * root = response.RootElement();
    XMLElement * create = response.NewElement("created");
    create->SetAttribute("id", id);
    root->InsertEndChild(create);
}

void Account::replyError(connection *C, XMLDocument & response, string info) {
    /* <error id="ACCOUNT_ID">Msg</error> */
    XMLElement * root = response.RootElement();
    XMLElement * error = response.NewElement("error");
    error->SetAttribute("id", id);
    XMLText * msg = response.NewText(info.c_str());
    error->InsertFirstChild(msg);
    root->InsertEndChild(error);
}

void Symbol::execute(connection *C, XMLDocument & response) {
    try {
        bool exist = checkAccountID(C, id);
        if (!exist) {
            string info = "The account_id doesn't exist! Request(create symbol) reject!";
            replyError(C, response, info);
            return;
        }
        CreatePosition(C, id, sym, num);  // account's sym exists yes/not  -> both valid
    } catch (const std::exception &e){
        replyError(C, response, e.what());
        return;
    }
    replySuccess(C, response);
}

void Symbol::replySuccess(connection *C, XMLDocument & response) {
    /* <created sym="SYM" id="ACCOUNT_ID"/> */
    XMLElement * root = response.RootElement();
    XMLElement * create = response.NewElement("created");
    create->SetAttribute("sym", sym.c_str());
    create->SetAttribute("id", id);
    root->InsertEndChild(create);
}

void Symbol::replyError(connection *C, XMLDocument & response, string info) {
    /* <error sym="SYM" id="ACCOUNT_ID">Msg</error> */
    XMLElement * root = response.RootElement();
    XMLElement * error = response.NewElement("error");
    error->SetAttribute("sym", sym.c_str());
    error->SetAttribute("id", id);
    XMLText * msg = response.NewText(info.c_str());
    error->InsertFirstChild(msg);
    root->InsertEndChild(error);
}

void Order::execute(connection *C, XMLDocument & response) {
    try {
        // account_id invalid->error  not exist
        bool valid1 = checkAccountID(C, account_id);
        if (!valid1) {
            string info1 = "The account_id doesn't exist! Request(open) reject!";
            replyError(C, response, info1);
            return;
        }
        if (amount > 0) { // buyer
            bool valid2 = checkBuyerMoney(C, account_id, amount, limit);
            if (!valid2) {
                string info2 = "The account_id doesn't have enough money! Request(open) reject!";
                replyError(C, response, info2);
                return;
            }
        }
        if (amount < 0) { // seller
            bool valid3 = checkSellerShare(C, account_id, sym, amount);
            if (!valid3) {
                string info3 = "The account_id doesn't have enough share! Request(open) reject!";
                replyError(C, response, info3);
                return;
            }
        }
        trans_id = CreateOrder(C, account_id, sym, amount, limit);  
        ExecOrder(C, trans_id, account_id, sym, amount, limit);// insert in table +/-, may match-> +/-(execute)
        replySuccess(C, response);
    } catch (const std::exception &e){
        replyError(C, response, e.what());
        return;
    }
}

void Order::replySuccess(connection *C, XMLDocument & response) {
    /* <opened sym="SYM" amount="AMT" limit="LMT" id="TRANS_ID"/> */
    XMLElement * root = response.RootElement();
    XMLElement * create = response.NewElement("opened");
    create->SetAttribute("sym", sym.c_str());
    create->SetAttribute("amount", amount);
    create->SetAttribute("limit", limit);
    create->SetAttribute("id", trans_id);
    root->InsertEndChild(create);
}

void Order::replyError(connection *C, XMLDocument & response, string info) {
    /* <error sym="SYM" amount="AMT" limit="LMT">Message</error> */
    XMLElement * root = response.RootElement();
    XMLElement * error = response.NewElement("error");
    error->SetAttribute("sym", sym.c_str());
    error->SetAttribute("amount", amount);
    error->SetAttribute("limit", limit);
    XMLText * msg = response.NewText(info.c_str());
    error->InsertFirstChild(msg);
    root->InsertEndChild(error);
}

void Query::execute(connection *C, XMLDocument & response) {
    bool valid1 = checkAccountID(C, account_id);
    if (!valid1) {
        string info = "The account_id doesn't exist! Request(query) reject!";
        replyError(C, response, info);
        return;
    } 
    bool valid2 = checkTransID(C, trans_id);  // exist
    if (!valid2) {
        string info = "The trans_id doesn't exist! Request(query) reject!";
        replyError(C, response, info);
        return;
    } 
    bool valid3 = checkBelong(C, account_id, trans_id);
    if (!valid3) {
        string info = "The trans_id doesn't belong to this account_id! Request(query) reject!";
        replyError(C, response, info);
        return;
    }
    replySuccess(C, response);
}

void Query::replySuccess(connection *C, XMLDocument & response) {
    std::vector<query_s> res = QueryOrder(C, account_id, trans_id);
    /* 
    <status id="TRANS_ID">
        <open shares=.../>
        <canceled shares=... time=.../>
        <executed shares=... price=... time=.../>
    </status> 
    */
    XMLElement * root = response.RootElement();
    XMLElement * status = response.NewElement("status");
    status->SetAttribute("id", trans_id);
    for(int i = 0; i < res.size(); ++i) {
        state = res[i].state;
        shares = res[i].shares;
        if (state == "open") {
            XMLElement * open = response.NewElement("open");
            open->SetAttribute("shares", shares);
            status->InsertEndChild(open);
        }
        else if (state == "canceled") {
            time = res[i].time;
            XMLElement * canceled = response.NewElement("canceled");
            canceled->SetAttribute("shares", shares);
            canceled->SetAttribute("time", time);
            status->InsertEndChild(canceled);
        }
        else {
            // executed
            time = res[i].time;
            price = res[i].price;
            XMLElement * executed = response.NewElement("executed");
            executed->SetAttribute("shares", shares);
            executed->SetAttribute("price", price);
            executed->SetAttribute("time", time);
            status->InsertEndChild(executed);
        }
    }
    root->InsertEndChild(status);
}

void Query::replyError(connection *C, XMLDocument & response, string info) {
    /* <error id="TRANS_ID">Message</error> */
    XMLElement * root = response.RootElement();
    XMLElement * error = response.NewElement("error");
    error->SetAttribute("id", trans_id);
    XMLText * msg = response.NewText(info.c_str());
    error->InsertFirstChild(msg);
    root->InsertEndChild(error);
}

void Cancel::execute(connection *C, XMLDocument & response) {
    bool valid1 = checkAccountID(C, account_id);
    if (!valid1) {
        string info = "The account_id doesn't exist! Request(cancel) reject!";
        replyError(C, response, info);
        return;
    } 
    bool valid2 = checkTransID(C, trans_id);  // exist
    if (!valid2) {
        string info = "The trans_id doesn't exist! Request(cancel) reject!";
        replyError(C, response, info);
        return;
    } 
    bool valid3 = checkBelong(C, account_id, trans_id);
    if (!valid3) {
        string info = "The trans_id doesn't belong to this account_id! Request(cancel) reject!";
        replyError(C, response, info);
        return;
    }
    bool valid4 = checkIfOpen(C, trans_id);   // true-> could cancel
    if (!valid4) {
        string info = "There is no open order for this trans_id! Request(cancel) reject!";
        replyError(C, response, info);
        return;
    } 
    replySuccess(C, response);
}

void Cancel::replySuccess(connection *C, XMLDocument & response) {
    std::vector<query_s> res = CancelOrder(C, account_id, trans_id);
    /*
    <canceled id="TRANS_ID">
        <canceled shares=... time=.../>
        <executed shares=... price=... time=.../>
    </canceled>
    */
    XMLElement * root = response.RootElement();
    XMLElement * cancelTag = response.NewElement("canceled");
    cancelTag->SetAttribute("id", trans_id);
    for(int i = 0; i < res.size(); ++i) {
        state = res[i].state;
        shares = res[i].shares;
        time = res[i].time;
        if (state == "canceled") {
            XMLElement * canceled = response.NewElement("canceled");
            canceled->SetAttribute("shares", shares);
            canceled->SetAttribute("time", time);
            cancelTag->InsertEndChild(canceled);
        }
        else if (state == "executed") {
            // executed
            price = res[i].price;
            XMLElement * executed = response.NewElement("executed");
            executed->SetAttribute("shares", shares);
            executed->SetAttribute("price", price);
            executed->SetAttribute("time", time);
            cancelTag->InsertEndChild(executed);
        }
    }
    root->InsertEndChild(cancelTag);
}

void Cancel::replyError(connection *C, XMLDocument & response, string info) {
    /* <error id="TRANS_ID">Message</error> */
    XMLElement * root = response.RootElement();
    XMLElement * error = response.NewElement("error");
    error->SetAttribute("id", trans_id);
    XMLText * msg = response.NewText(info.c_str());
    error->InsertFirstChild(msg);
    root->InsertEndChild(error);
}

