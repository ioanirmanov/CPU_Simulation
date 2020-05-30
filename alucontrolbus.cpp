#include "alucontrolbus.h"
#include <algorithm>
#include "arithlogunit.h"



AluControlBus::AluControlBus()
{

}

AluControlBus::AluControlBus(class ArithLogUnit *ACoUnitPtr)
{
    //Set pointers
    ACoUnit = ACoUnitPtr;
}

void AluControlBus::WriteInstruction(bool *Val)
{
    //Copy value
    std::copy(Val, Val + 4, Value);

    State = ACoBusWrite;

    //Write value to ALU
    ACoUnit->WriteInstruction(Value);
}

void AluControlBus::SetIdle()
{
    State = ACoBusIdle;
}

int AluControlBus::GetState()
{
    return State;
}

ACoBusState AluControlBus::GetStateEnum()
{
    return State;
}

void AluControlBus::GetValue(bool *Destination)
{
    std::copy(Value, Value + 4, Destination);
}
