#include "memory.h"
#include <algorithm>
#include "binaryutilities.h"
#include "databus.h"
#include "memoryhelper.h"
#include<QString>
#include <fstream>

Memory::Memory()
{
    //Set all locations to 0
    for(int a = 0; a < 256; a++)
    {
        for(int b = 0; b < 24; b++)
        {
            Locations[a][b] = 0;
        }
    }
}

void Memory::SetRead()
{
    State = MemRead;
    LocAccessed = false;
}

void Memory::SetWrite()
{
    State = MemWrite;
    LocAccessed = false;
}

void Memory::SetIdle()
{
    State = MemIdle;
    LocAccessed = false;
}

void Memory::WriteAddress(bool *InAddress)
{
    std::copy(InAddress, InAddress + 24, Address);
    int AddressDec = BinaryUtilities::BinToDec(Address, 24);

    if(State == MemRead)
    {
        ReadLocation(AddressDec, Value);
        MemDatBus->SetValue(Value);
        LocAccessed = true;
    }
    std::ofstream outfile;

}

void Memory::WriteData(bool *Data)
{
    int AddressDec = BinaryUtilities::BinToDec(Address, 24);
    std::copy(Data, Data + 24, Value);
    if(State == MemWrite)
    {
        WriteLocation(AddressDec, Value);
        LocAccessed = true;
    }
}

void Memory::ReadLocation(int Index, bool *Result)
{
    if(Index < 256 && Index >=0)
    {
        std::copy(Locations[Index], Locations[Index] + 24, Result);
    }
}

void Memory::WriteLocation(int Index, bool *InValue)
{
    if(Index < 256 && Index >=0)
    {
        std::copy(InValue, InValue + 24, Locations[Index]);
    }
}

void Memory::SetDataBusPtr(class DataBus *Ptr)
{
    MemDatBus = Ptr;
}

void Memory::SetContents(class MemoryHelper *MemHelp)
{
    MemHelp->GetLocations(Locations);
}

void Memory::GetValue(bool *Destination)
{
    std::copy(Value, Value + 24, Destination);
}

void Memory::GetAddress(bool *Destination)
{
    std::copy(Address, Address + 24, Destination);

}

int Memory::GetState()
{
    return State;
}

MemoryState Memory::GetStateEnum()
{
    return State;
}

bool Memory::GetLocAccessed()
{
    return LocAccessed;
}


