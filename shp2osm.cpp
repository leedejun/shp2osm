#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include "shapefil.h"
#include "quadtree.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include "osmutil.h"

void key_free(void* key) {
    delete static_cast<int*>(key); // 确保类型匹配
}

// You must free the result if result is non-NULL.
char* str_replace(char* orig, const char* rep, const char* with) {
    char* result; // the return string
    char* ins;    // the next insert point
    char* tmp;    // varies
    int len_rep;  // length of rep
    int len_with; // length of with
    int len_front; // distance between rep and end of last rep
    int count;    // number of replacements

    if (!orig)
        return nullptr;
    if (!rep)
        rep = ""; // 使用const char*避免警告
    len_rep = strlen(rep);
    if (!with)
        with = ""; // 使用const char*避免警告
    len_with = strlen(with);

    ins = orig;
    for (count = 0; (tmp = strstr(ins, rep)); ++count) {
        ins = tmp + len_rep;
    }

    tmp = result = new char[strlen(orig) + (len_with - len_rep) * count + 1];

    if (!result)
        return nullptr;

    while (count--) {
        ins = strstr(orig, rep);
        len_front = ins - orig;
        strcpy(tmp, std::string(orig, len_front).c_str());
        tmp += len_front;
        strcpy(tmp, with);
        tmp += len_with;
        orig += len_front + len_rep; // move to next "end of rep"
    }
    strcpy(tmp, orig);
    return result;
}

// // You must free the result if result is non-NULL.
// char* str_replace(char* orig, char* rep, char* with) {
//     char* result; // the return string
//     char* ins;    // the next insert point
//     char* tmp;    // varies
//     int len_rep;  // length of rep
//     int len_with; // length of with
//     int len_front; // distance between rep and end of last rep
//     int count;    // number of replacements

//     if (!orig)
//         return nullptr;
//     if (!rep)
//         rep = "";
//     len_rep = strlen(rep);
//     if (!with)
//         with = "";
//     len_with = strlen(with);

//     ins = orig;
//     for (count = 0; tmp = strstr(ins, rep); ++count) {
//         ins = tmp + len_rep;
//     }

//     tmp = result = new char[strlen(orig) + (len_with - len_rep) * count + 1];

//     if (!result)
//         return nullptr;

//     while (count--) {
//         ins = strstr(orig, rep);
//         len_front = ins - orig;
//         strcpy(tmp, std::string(orig, len_front).c_str());
//         tmp += len_front;
//         strcpy(tmp, with);
//         tmp += len_with;
//         orig += len_front + len_rep; // move to next "end of rep"
//     }
//     strcpy(tmp, orig);
//     return result;
// }

// int main(int argc, char** argv) {
//     SHPHandle hWaySHP;
//     DBFHandle hWayDBF;
//     int nShapeType, nWays, dbfCount, nWidth, nDecimals;
//     double adfMinBound[4], adfMaxBound[4];
//     char szTitle[50];
//     int i = 0;
//     std::FILE* fp_point = nullptr;
//     std::FILE* fp_way = nullptr;

//     quadtree_t* tree = nullptr;
//     point_t* point = nullptr;
//     int point_num = 1;
//     int* point_id = nullptr;
//     double zero = 0.000000001;

//     char* inputfile = nullptr;
//     char* outputfile = nullptr;
//     const char* tempfile = "temp";

//     if (argc < 2) {
//         std::cout << "shp2osm: convert shapefile to osm" << std::endl;
//         std::cout << "Usage: " << std::endl;
//         std::cout << "shp2osm way_shp_file " << std::endl;
//         exit(1);
//     }
//     inputfile = argv[1];
//     outputfile = str_replace(inputfile, ".shp", ".osm");
//     if (!strstr(outputfile, ".osm")) {
//         std::strcat(outputfile, ".osm");
//     }

//     hWaySHP = SHPOpen(inputfile, "rb");
//     if (!hWaySHP) {
//         std::cout << "Unable to open:" << argv[1] << std::endl;
//         exit(1);
//     }

//     hWayDBF = DBFOpen(inputfile, "rb");
//     if (!hWayDBF) {
//         std::cout << "DBFOpen(" << argv[1] << ",\"r\") failed." << std::endl;
//         exit(2);
//     }

//     SHPGetInfo(hWaySHP, &nWays, &nShapeType, adfMinBound, adfMaxBound);
//     dbfCount = DBFGetRecordCount(hWayDBF);
//     if (dbfCount != nWays) {
//         std::cout << "dbf number error " << std::endl;
//         exit(1);
//     }

//     fp_way = std::fopen(tempfile, "w");
//     fp_point = std::fopen(outputfile, "w");
//     std::fprintf(fp_point, "<?xml version='1.0' encoding='UTF-8'?>\n");
//     std::fprintf(fp_point, "<osm version=\"0.6\" generator=\"shp2osm\">\n");

//     tree = quadtree_new(adfMinBound[0] - zero, adfMinBound[1] - zero, adfMaxBound[0] + zero, adfMaxBound[1] + zero);
//     tree->key_free = key_free;

//     for (i = 0; i < nWays; i++) {
//         int j;
//         SHPObject* psShape;
//         psShape = SHPReadObject(hWaySHP, i);
//         if (!psShape) {
//             std::fprintf(stderr, "Unable to read shape %d, terminating object reading.\n", i);
//             break;
//         }

//         std::fprintf(fp_way, "  <way id=\"%d\" version=\"1\" visible=\"true\" >\n", i + 1);
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

//     quadtree_free(tree);

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
//     std::fprintf(fp_point, "</osm>\n");

//     std::fclose(fp_point);
//     fp_point = nullptr;

//     delete[] outputfile;

//     return 0;
// }

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "shp2osm: convert shapefile to osm" << std::endl;
        std::cout << "Usage: " << std::endl;
        std::cout << "shp2osm way_shp_file1 way_shp_file2 ..." << std::endl;
        exit(1);
    }

    // 创建一个vector来存储字符串参数
    std::vector<std::string> args;

    // 遍历argv数组，跳过第一个元素（程序的路径），将其余的元素添加到vector中
    for (int i = 1; i < argc; ++i) {
        args.push_back(argv[i]);
    }

    // bbox mergeBox = osmutil::Instance()->getMergeBox(args);
    bbox mergeBox = bbox(109.4956982421875011,20.0853388129340260,121.3786493598090317,28.2534320746527783);
    const char* tempfile = "temp";
    std::FILE* fp_way = std::fopen(tempfile, "w");
    if (!fp_way) {
        std::cout << "Unable to open temp file." << std::endl;
        exit(1);
    }
    std::FILE* fp_point = std::fopen("output.osm", "w"); // 单个输出文件
    if (!fp_point) {
        std::cout << "Unable to open output file." << std::endl;
        exit(1);
    }
    std::fprintf(fp_point, "<?xml version='1.0' encoding='UTF-8'?>\n");
    std::fprintf(fp_point, "<osm version=\"0.6\" generator=\"shp2osm\">\n");

    quadtree_t* tree = quadtree_new(mergeBox.minX, mergeBox.minY, mergeBox.maxX, mergeBox.maxY); // 创建四叉树
    tree->key_free = key_free;
    point_t* point = nullptr;
    int* point_id = nullptr;
    int globalNodeID = 1; // 用于跟踪全局节点ID
    int globalWayId = 1;   // 用于跟踪全局way ID

    for (int i = 1; i < argc; ++i) {
        int nShapeType, nWays, dbfCount, nWidth, nDecimals;
        SHPHandle hWaySHP = SHPOpen(argv[i], "rb");
        if (!hWaySHP) {
            std::cout << "Unable to open: " << argv[i] << std::endl;
            continue;
        }

        DBFHandle hWayDBF = DBFOpen(argv[i], "rb");
        if (!hWayDBF) {
            std::cout << "DBFOpen(" << argv[i] << ",\"r\") failed." << std::endl;
            SHPClose(hWaySHP);
            continue;
        }

        double adfMinBound[4], adfMaxBound[4];
        SHPGetInfo(hWaySHP, &nWays, &nShapeType, adfMinBound, adfMaxBound);
        dbfCount = DBFGetRecordCount(hWayDBF);
        if (dbfCount != nWays) {
            std::cout << "dbf number error " << std::endl;
            continue;
        }

        for (int j = 0; j < nWays; j++) {
            SHPObject* psShape = nullptr;
            psShape = SHPReadObject(hWaySHP, j);
            if (!psShape) {
                std::fprintf(stderr, "Unable to read shape %d, terminating object reading.\n", j);
                break;
            }

            std::fprintf(fp_way, "  <way id=\"%d\" version=\"1\" visible=\"true\" >\n", globalWayId);
            for (int k = 0; k < psShape->nVertices; k++) {
                point_id = new int;
                *point_id = globalNodeID;
                point = quadtree_insert(tree, psShape->padfX[k], psShape->padfY[k], point_id, false);
                if (!point) {
                    std::cout << "lat=\"" << psShape->padfY[k] << "\" lon=\"" << psShape->padfX[k] << "\" insert error " << std::endl;
                    continue;
                }
                if (*(int*)point->key == globalNodeID) {
                    std::fprintf(fp_point, "  <node id=\"%d\"  version=\"1\" visible=\"true\" lat=\"%f\" lon=\"%f\"/>\n", *(int*)point->key, point->y, point->x);
                    globalNodeID++;
                }
                std::fprintf(fp_way, "    <nd ref=\"%d\"/>\n", *(int*)point->key);
            }
            SHPDestroyObject(psShape);

            for (int k = 0; k < DBFGetFieldCount(hWayDBF); k++) {
                DBFFieldType eType;
                char szTitle[128];
                eType = DBFGetFieldInfo(hWayDBF, k, szTitle, &nWidth, &nDecimals);
                if (eType == FTString) {
                    std::string fieldName = szTitle;
                    std::string value = DBFReadStringAttribute(hWayDBF, j, k);
                    osmtag tag = osmutil::Instance()->shpFieldValue2osmtag(fieldName, value);
                    if (tag.key!="")
                    {
                        std::fprintf(fp_way, "    <tag k=\"%s\" v=\"%s\"/>\n", tag.key.c_str(), tag.value.c_str());
                    }
                } else if (eType == FTInteger) {
                    std::fprintf(fp_way, "    <tag k=\"%s\" v=\"%d\"/>\n", szTitle, DBFReadIntegerAttribute(hWayDBF, j, k));
                } else if (eType == FTDouble) {
                    std::fprintf(fp_way, "    <tag k=\"%s\" v=\"%lf\"/>\n", szTitle, DBFReadDoubleAttribute(hWayDBF, j, k));
                }
            }
            std::fprintf(fp_way, "  </way>\n");
            globalWayId++;
        }

        SHPClose(hWaySHP);
        DBFClose(hWayDBF);
    }

    std::fclose(fp_way);
    fp_way = nullptr;

    {
        char c;
        fp_way = std::fopen(tempfile, "r");
        while ((c = std::fgetc(fp_way)) != EOF) {
            std::fputc(c, fp_point);
        }
        std::fclose(fp_way);
        fp_way = nullptr;
        std::remove(tempfile);
    }

    quadtree_free(tree); // 释放四叉树资源
    std::fprintf(fp_point, "</osm>\n");
    std::fclose(fp_point);
    fp_point = nullptr;

    return 0;
}
