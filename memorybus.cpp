#include "memorybus.h"
#include "register.h"
#include <algorithm>

MemoryBus::MemoryBus()
{

}

MemoryBus::MemoryBus(class Memory *MemPointer, class Register *RegPointer)
{
    Mem = MemPointer;
    Reg = RegPointer;
}

MemoryBus::~MemoryBus()
{

}

void MemoryBus::WriteToMemory()
{
    Reg->Read(Value);
    State = BusWriteMemory;
}

int MemoryBus::GetState()
{
    return State;
}

MemBusState MemoryBus::GetStateEnum()
{
    return State;
}

void MemoryBus::GetValue(bool *Destination)
{
    std::copy(Value, Value + 24, Destination);
}

void MemoryBus::SetIdle()
{
    State = BusIdle;
}

