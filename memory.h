#ifndef MEMORY_H
#define MEMORY_H

#pragma once


enum MemoryState : int
{
    MemIdle = 0,
    MemRead = 1,
    MemWrite = 2
};


class Memory
{
    friend class MemoryWidget;
    friend class MemoryInfoWindow;
public:
    Memory();
    void SetRead();
    void SetWrite();
    void SetIdle();
    void WriteAddress(bool InAddress[24]);
    void WriteData(bool InData[24]);

    int GetState();
    MemoryState GetStateEnum();
    bool GetLocAccessed();
    void GetValue(bool Destination[24]);
    void GetAddress(bool Destination[24]);

    void SetDataBusPtr(class DataBus *Ptr);

    void SetContents(class MemoryHelper *MemHelp);

private:
    bool Locations[256][24];
    MemoryState State;
    bool Address[24];
    bool Value[24];
    class DataBus *MemDatBus;

    void WriteLocation(int Index, bool InValue[24]);
    void ReadLocation(int Index, bool *Result);

    bool LocAccessed = false;
    int AddressAccessed;
};

#endif // MEMORY_H
