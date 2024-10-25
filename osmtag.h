#ifndef __OSMTAG_H__
#define __OSMTAG_H__
#include <string>

struct osmtag
{
    std::string key;
    std::string value;
    osmtag()
    {
        key="";
        value="";
    }
};

#endif
