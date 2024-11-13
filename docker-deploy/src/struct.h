#ifndef __STRUCT_H__
#define __STRUCT_H__
#include <string>

struct query_s
{
    std::string state;//open, cancel, exec
    int shares;
    long time;
    float price;
};

#endif

