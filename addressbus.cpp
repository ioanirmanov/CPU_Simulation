#include "addressbus.h"
#include "memory.h"
#include <algorithm>
#include "register.h"

AddressBus::AddressBus()
 :MemoryBus()
{

}

AddressBus::AddressBus(class Memory *MemPointer, class Register *RegPointer)
    : MemoryBus(MemPointer, RegPointer)
{

}

void AddressBus::WriteToMemory()
{
    //Read value from register, change the state, write address to memory
    Reg->Read(Value);
    State = BusWriteMemory;
    Mem->WriteAddress(Value);
}
