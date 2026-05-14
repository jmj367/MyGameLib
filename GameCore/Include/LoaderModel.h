#include "Skeleton.h"
#include <string>

class LoaderModel
{
public:
    static bool LoadSkeleton(const std::string &fileName, Skeleton &skeleton);

private:
    static bool LoadSkeletonJson(class JsonObject* data, Skeleton &skeleton);
    static bool LoadSkeletonJson(const std::string &fileName, Skeleton &skeleton);
    static bool LoadSkeletonBinary(const std::string &fileName, Skeleton &skeleton);
};