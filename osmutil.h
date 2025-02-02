#ifndef __OSMUTIL_H__
#define __OSMUTIL_H__

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include "osmtag.h"
#include "bbox.h"

class osmutil {
public:
   static osmutil* Instance();
   ~osmutil();

   std::string getHighway(const std::string& key);
   std::string getDirection(const std::string& key);
   osmtag shpFieldValue2osmtag(const std::string& fieldName, const std::string& value);

   bbox getMergeBox(const std::vector<std::string>& shpFiles);
//    int shpfile2osmfile(char* inputfile, char* outputfile, std::FILE* fp_point, quadtree_t* tree, int wayStartId, int pointStartId);


protected:
    void init();
    osmutil();
    bool caseInsensitiveEqual(const std::string& str1, const std::string& str2);
    std::vector<std::string> SpliteString(const std::string &in,const std::string &delim);

    std::unordered_map<std::string, std::string> m_highwayLevelMap;
    std::unordered_map<std::string, std::string> m_directionMap;
    
    static osmutil* m_instance;
};

#endif