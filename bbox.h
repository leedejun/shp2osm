#ifndef __BBOX_H__
#define __BBOX_H__
#include <algorithm>

struct bbox
{
    double minX;
    double minY;
    double maxX;
    double maxY;
    bbox()
        : minX(0.0), minY(0.0), maxX(0.0), maxY(0.0) {}

    bbox(double minx, double miny, double maxx, double maxy)
        : minX(minx), minY(miny), maxX(maxx), maxY(maxy) {}

    // 合并两个bbox对象
    bbox merge(const bbox& other) const {
        return bbox(
            std::min(minX, other.minX), // 取两个minX中的较小值
            std::min(minY, other.minY), // 取两个minY中的较小值
            std::max(maxX, other.maxX), // 取两个maxX中的最大值
            std::max(maxY, other.maxY)  // 取两个maxY中的最大值
        );
    }

};

#endif