#ifndef __PARSE_H__
#define __PARSE_H__

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <cstring>
#include <string>
#include "tinyxml2.h"
#include "request.h"
#include "exception.h"
#include "sql.h"


using namespace tinyxml2;

void parseXML(string req, string & response);
int getXmlSize(string req);
void parseCreate(XMLDocument * xml, string & response);
void parseTrans(XMLDocument * xml, string & response);

#endif