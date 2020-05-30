#include "databus.h"
#include "register.h"
#include "memory.h"
#include <algorithm>

DataBus::DataBus()
 :MemoryBus ()
{

}

DataBus::DataBus(class Memory *MemPointer, class Register *RegPointer)
   : MemoryBus(MemPointer, RegPointer)
{

}

void DataBus::WriteToMemory()
{
    Reg->Read(Value);
    State = BusWriteMemory;
    Mem->WriteData(Value);
}

void DataBus::WriteToRegister()
{
    State = BusWriteRegister;
    Reg->Write(Value);
}

void DataBus::SetValue(bool *Val)
{
    std::copy(Val, Val + 24, Value);
}
