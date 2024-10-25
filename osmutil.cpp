#include "osmutil.h"
#include <algorithm>
#include <cctype>
#include <locale>
#include <regex>
#include <iostream>


osmutil* osmutil:: m_instance=nullptr;

void osmutil::init() 
{
    //highway map
    m_highwayLevelMap["00"] = "motorway";//高速路
    m_highwayLevelMap["01"] = "trunk";//都市高速路00 01 02 10
    m_highwayLevelMap["02"] = "trunk";//国道
    m_highwayLevelMap["03"] = "primary";//省道 13
    m_highwayLevelMap["04"] = "secondary";//县道12
    m_highwayLevelMap["06"] = "tertiary";//乡镇村道
    m_highwayLevelMap["08"] = "service";////其他道路
    m_highwayLevelMap["09"] = "unclassified";//九级路
    m_highwayLevelMap["0a"] = "ferry_motorcar";//轮渡
    m_highwayLevelMap["0b"] = "pedestrian";//行人道路
    m_highwayLevelMap["0c"] = "ferry";//人渡
    m_highwayLevelMap["0d"] = "cycleway";//自行车专用道

    //direction map
    m_directionMap["0"] = "no";
    m_directionMap["1"] = "no";
    m_directionMap["2"] = "yes";
    m_directionMap["3"] = "-1";
}

osmutil::osmutil()
{
    init();
}

osmutil::~osmutil()
{

}

osmutil* osmutil::Instance()
{
    if (m_instance==nullptr)
    {
        static osmutil staticOsmutil;
        m_instance = &staticOsmutil;
    }
    return m_instance;
    
}

std::string osmutil::getHighway(const std::string& key) 
{
    auto it = m_highwayLevelMap.find(key);
    if (it != m_highwayLevelMap.end()) {
        return it->second;
    } 
    else 
    {
        return "";
    }
}

std::string osmutil::getDirection(const std::string& key)
{
    auto it = m_directionMap.find(key);
    if (it != m_directionMap.end()) {
        return it->second;
    } 
    else 
    {
        return "";
    }
}

bool osmutil::caseInsensitiveEqual(const std::string& str1, const std::string& str2) {
    std::regex e(str2, std::regex_constants::icase);
    return std::regex_match(str1, e);
}

std::vector<std::string> osmutil::SpliteString(const std::string &in,const std::string &delim)
{
    try
    {
        std::regex re(delim);
        return std::vector<std::string>(
            std::sregex_token_iterator(in.begin(),in.end(),re,-1),
            std::sregex_token_iterator());
    }
    catch(const std::exception &e)
    {
        std::cerr<<e.what()<<'\n';
        return std::vector<std::string>();
    }
}

osmtag osmutil::shpFieldValue2osmtag(const std::string& fieldName, const std::string& value)
{
    osmtag tag;
    if (caseInsensitiveEqual(fieldName,"kind"))
    {
        tag.key="highway";
        std::vector<std::string> kindList = SpliteString(value, "\\|");
        //std::cout << "kindList.size="<<kindList.size()<< std::endl;
        if (kindList.size())
        {
            std::string kindValue = kindList.at(0);
            if (kindValue.size()>=2)
            {
                std::string roadlevel = kindValue.substr(0,2);
                tag.value = getHighway(roadlevel);
            }
        }

        //std::cout << "tag.key="<<tag.key<<",tag.value ="<< tag.value << std::endl;
    }
    else if (caseInsensitiveEqual(fieldName,"direction"))
    {
        tag.key="oneway";
        //std::cout << "tag.key="<<tag.key<<",value ="<< value << std::endl;
        tag.value = getDirection(value);
        //std::cout << "tag.key="<<tag.key<<",tag.value ="<< tag.value << std::endl;
    }
    else if (caseInsensitiveEqual(fieldName,"name")
    || caseInsensitiveEqual(fieldName,"pathname"))
    {
        tag.key = "name";
        tag.value = value;
    }
    else
    {
        tag.key = fieldName;
        tag.value = value;
    }
    return tag;
}