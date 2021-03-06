#include "controlbus.h"
#include "memory.h"


ControlBus::ControlBus()
{

}

ControlBus::ControlBus(class Memory *MemPointer)
{
    Mem = MemPointer;
}

void ControlBus::SetRead()
{
    State = CntrlRead;
    Mem->SetRead();
}

void ControlBus::SetWrite()
{
    State = CntrlWrite;
    Mem->SetWrite();
}

void ControlBus::SetIdle()
{
    State = CntrlIdle;
    Mem->SetIdle();
}

int ControlBus::GetState()
{
    return State;
}

CntrlBusState ControlBus::GetStateEnum()
{
    return State;
}

