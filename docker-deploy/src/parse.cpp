#include "parse.h"

void parseXML(string req, string & response) {
    ///////////////
    //std::cout << "recv string: " << std::endl;
    //std::cout << req << std::endl;
    size_t firstline = req.find('\n', 0);
    req = req.substr(firstline + 1);
    XMLDocument * xml = new XMLDocument();
    xml->Parse(req.c_str());
    XMLElement * root = xml->RootElement();
    if (strcmp(root->Name(), "create") == 0) {
        parseCreate(xml, response);
    }
    else if (strcmp(root->Name(), "transactions") == 0) {
        parseTrans(xml, response);
    }
    else {
        throw(MyException("Error: Invalid request!"));
    }
    delete xml;
}

int getXmlSize(string req) {
    size_t firstline = req.find('\n', 0);
    return stoi(req.substr(0, firstline));
}

/*  
    <create>
        <account id="ACCOUNT_ID" balance="BALANCE"/> #0 or more
         <symbol sym="SYM"> #0 or more
            <account id="ACCOUNT_ID">NUM</account> #1 or more
         </symbol>
    </create>   
*/
void parseCreate(XMLDocument * xml, string & response) {
    connection *C = connect_db();
    Request* request = new Request();
    XMLElement * root = xml->RootElement();
    if (strcmp(root->Name(), "create") == 0) {
        XMLElement * child = root->FirstChildElement();
        while(child) {
            if(strcmp(child->Name(), "account") == 0) {
                int id = child->FindAttribute("id")->IntValue();
                float balance = child->FindAttribute("balance")->FloatValue();
                Account * account = new Account(id, balance);
                ///////////////
                // std::cout << "recv account: " << std::endl;
                // std::cout << id << std::endl;
                // std::cout << balance << std::endl;
                request->subrequest.push_back(account);
            }
            else if (strcmp(child->Name(), "symbol") == 0) {
                string sym = child->FindAttribute("sym")->Value();
                XMLElement * seller = child->FirstChildElement();
                while(seller) {
                    int id = seller->FindAttribute("id")->IntValue();
                    int num = atoi(seller->GetText());
                    Symbol * symbol = new Symbol(sym, id, num);
                    request->subrequest.push_back(symbol);
                    seller = seller->NextSiblingElement();
                }
            }
            child = child->NextSiblingElement();
        }
        request->execute(C);
        ///////////////
        //request->display();
        response = request->convertResToStr();
        ///////////////
        //std::cout << "response:" << std::endl;
        //std::cout << response << std::endl;
    }
    delete request;
    disconnect_db(C);
    delete C;
}

/*  
    <transactions id="ACCOUNT_ID"> #contains 1 or more of the below children
        <order sym="SYM" amount="AMT" limit="LMT"/> 
        <query id="TRANS_ID"> 
        <cancel id="TRANS_ID">
    </transactions>   
*/
void parseTrans(XMLDocument * xml, string & response) {
    connection *C = connect_db();
    Request* request = new Request();
    XMLElement * root = xml->RootElement();
    if (strcmp(root->Name(), "transactions") == 0) {
        int account_id = root->FindAttribute("id")->IntValue();
        XMLElement * child = root->FirstChildElement();
        while(child) {
            if(strcmp(child->Name(), "order") == 0) {
                string sym = child->FindAttribute("sym")->Value();
                int amount = child->FindAttribute("amount")->IntValue();
                float limit = child->FindAttribute("limit")->FloatValue();
                Order * order = new Order(account_id, sym, amount, limit);
                request->subrequest.push_back(order);
            }
            else if (strcmp(child->Name(), "query") == 0) {
                int trans_id = child->FindAttribute("id")->IntValue();
                Query * query = new Query(account_id, trans_id);
                request->subrequest.push_back(query);
            }
            else if (strcmp(child->Name(), "cancel") == 0) {
                int trans_id = child->FindAttribute("id")->IntValue();
                Cancel * cancel = new Cancel(account_id, trans_id);
                request->subrequest.push_back(cancel);
            }
            child = child->NextSiblingElement();
        }
        request->execute(C);
        response = request->convertResToStr();
    }
    delete request;
    disconnect_db(C);
    delete C;
}
