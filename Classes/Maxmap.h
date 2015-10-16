// (C) 2015 Turnro.com

#ifndef __Turnroll__Maxmap__
#define __Turnroll__Maxmap__

#include "RollNode.h"
#include <unordered_map>
#include <memory>
#include <limits>

class Maxmap
{
public:
    void load(const std::string& maxmap, bool infiniteMode = false);
    int getSize();
    int getEndIndex();
    std::unique_ptr<RollMinMap>& getCookedMinMap(int index);
    bool checkIndex(int index);
protected:
    // .maxmap内的minmap名称列表
    std::vector<std::string> _maxmapRawList;
    void loadMaxmapRawList(const std::string& maxmap);

    // 所有需要的minmap的raw数据
    std::unordered_map<std::string, std::shared_ptr<RollMinMap> > _rawMinmaps;
    void loadRawMinmaps();

    // 烘焙过的minmap数据
    std::vector<std::unique_ptr<RollMinMap> > _cookedMinmaps;
    void cookMinmapsAll();
    void cookMinmapAppend(const std::string& minmapName);

    bool _infinite = false;
    int _endIndex = std::numeric_limits<int>::max();
};

#endif /* defined(__Turnroll__Maxmap__) */
