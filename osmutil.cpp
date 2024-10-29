#include "osmutil.h"
#include <algorithm>
#include <cctype>
#include <locale>
#include <regex>
#include <iostream>
#include "shapefil.h"
#include "quadtree.h"


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
        // tag.key = fieldName;
        // tag.value = value;
    }
    return tag;
}

bbox osmutil::getMergeBox(const std::vector<std::string>& shpFiles)
{
    bbox sumbox = bbox();
    double zero = 0.000000001;
    for (size_t i = 0; i < shpFiles.size(); i++)
    {
        double adfMinBound[4], adfMaxBound[4];
        std::string shpfile=shpFiles.at(i);
        int nShapeType, nWays;
        SHPHandle hWaySHP = SHPOpen(shpfile.c_str(), "rb");
        if (!hWaySHP) {
            std::cout << "Unable to open: " << shpfile << std::endl;
            continue;
        }

        SHPGetInfo(hWaySHP, &nWays, &nShapeType, adfMinBound, adfMaxBound);
        bbox box = bbox(adfMinBound[0] - zero, adfMinBound[1] - zero, adfMaxBound[0] + zero, adfMaxBound[1] + zero);
        if (i==0)
        {
            sumbox=box;
        }
        else{
            sumbox.merge(box);
        }
        SHPClose(hWaySHP);
    }

    return sumbox;
}

// int osmutil::shpfile2osmfile(char* inputfile, char* outputfile, std::FILE* fp_point, quadtree_t* tree, int wayStartId, int pointStartId)
// {
//     if (inputfile==nullptr || outputfile==nullptr 
//     || fp_point==nullptr || tree==nullptr 
//     || wayStartId<=0 || pointStartId<=0)
//     {
//         std::cout << "input param is error." << std::endl;
//         return 1;
//     }
    

//     SHPHandle hWaySHP;
//     DBFHandle hWayDBF;
//     int nShapeType, nWays, dbfCount, nWidth, nDecimals;
//     double adfMinBound[4], adfMaxBound[4];
//     char szTitle[50];

//     point_t* point = nullptr;
//     int point_num = pointStartId;
//     int* point_id = nullptr;
//     double zero = 0.000000001;

//     const char* tempfile = "temp";
//     hWaySHP = SHPOpen(inputfile, "rb");
//     if (!hWaySHP) {
//         std::cout << "Unable to open:" << inputfile << std::endl;
//         return 1;
//     }

//     hWayDBF = DBFOpen(inputfile, "rb");
//     if (!hWayDBF) {
//         std::cout << "DBFOpen(" << inputfile << ",\"r\") failed." << std::endl;
//         return 2;
//     }

//     SHPGetInfo(hWaySHP, &nWays, &nShapeType, adfMinBound, adfMaxBound);
//     dbfCount = DBFGetRecordCount(hWayDBF);
//     if (dbfCount != nWays) {
//         std::cout << "dbf number error " << std::endl;
//         return 1;
//     }

//     fp_way = std::fopen(tempfile, "w");

//     for (int i = 0; i < nWays; i++) {
//         int j;
//         SHPObject* psShape;
//         psShape = SHPReadObject(hWaySHP, i);
//         if (!psShape) {
//             std::fprintf(stderr, "Unable to read shape %d, terminating object reading.\n", i);
//             break;
//         }

//         std::fprintf(fp_way, "  <way id=\"%d\" version=\"1\" visible=\"true\" >\n", i + wayStartId);
//         for (j = 0; j < psShape->nVertices; j++) {
//             point_id = new int;
//             *point_id = point_num;
//             point = quadtree_insert(tree, psShape->padfX[j], psShape->padfY[j], point_id, false);
//             if (!point) {
//                 std::cout << "lat=\"" << psShape->padfY[j] << "\" lon=\"" << psShape->padfX[j] << "\" insert error " << std::endl;
//                 continue;
//             }
//             if (*(int*)point->key == point_num) {
//                 std::fprintf(fp_point, "  <node id=\"%d\"  version=\"1\" visible=\"true\" lat=\"%f\" lon=\"%f\"/>\n", *(int*)point->key, point->y, point->x);
//                 point_num++;
//             }
//             std::fprintf(fp_way, "    <nd ref=\"%d\"/>\n", *(int*)point->key);
//         }
//         SHPDestroyObject(psShape);

//         for (j = 0; j < DBFGetFieldCount(hWayDBF); j++) {
//             DBFFieldType eType;
//             eType = DBFGetFieldInfo(hWayDBF, j, szTitle, &nWidth, &nDecimals);
//             if (eType == FTString) {
//                 std::string fieldName = szTitle;
//                 std::string value = DBFReadStringAttribute(hWayDBF, i, j);
//                 osmtag tag = osmutil::Instance()->shpFieldValue2osmtag(fieldName, value);
//                 if (tag.key!="")
//                 {
//                     std::fprintf(fp_way, "    <tag k=\"%s\" v=\"%s\"/>\n", tag.key.c_str(), tag.value.c_str());
//                 }
//             } else if (eType == FTInteger) {
//                 std::fprintf(fp_way, "    <tag k=\"%s\" v=\"%d\"/>\n", szTitle, DBFReadIntegerAttribute(hWayDBF, i, j));
//             } else if (eType == FTDouble) {
//                 std::fprintf(fp_way, "    <tag k=\"%s\" v=\"%lf\"/>\n", szTitle, DBFReadDoubleAttribute(hWayDBF, i, j));
//             }
//         }
//         std::fprintf(fp_way, "  </way>\n");
//     }

//     SHPClose(hWaySHP);
//     DBFClose(hWayDBF);

//     std::fclose(fp_way);
//     fp_way = nullptr;

//     {
//         char c;
//         fp_way = std::fopen(tempfile, "r");
//         while ((c = std::fgetc(fp_way)) != EOF) {
//             std::fputc(c, fp_point);
//         }
//         std::fclose(fp_way);
//         fp_way = nullptr;
//         std::remove(tempfile);
//     }

//     return 0;
// }