#ifndef MEMORYHELPER_H
#define MEMORYHELPER_H


class MemoryHelper
{
public:
    MemoryHelper();
    void SetLocation(bool Value[24], int Location);
    void GetLocations(bool Dest[256][24]);
    void GetLocation(int Location, bool Destination[24]);
private:
    bool Locations[256][24];
};

#endif // MEMORYHELPER_H
