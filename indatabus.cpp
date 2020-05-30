#include "indatabus.h"
#include <algorithm>
#include "register.h"

InDataBus::InDataBus()
{

}

InDataBus::InDataBus(class Register *RegistersPtr)
{
    Registers = RegistersPtr;
}

int InDataBus::GetState()
{
    return State;
}

InDataState InDataBus::GetStateEnum()
{
    return State;
}

void InDataBus::GetValue(bool *Destination)
{
    std::copy(Value, Value + 24, Destination);
}

int InDataBus::GetSource()
{
    return Src;
}

int InDataBus::GetDestination()
{
    return Dest;
}

void InDataBus::Write(int Source, int Destination)
{
    State = InDataState::Write;
    //if source and destination are in the right range
    if(Source >= 0 && Source < 17)
    {
        if(Destination >= 0 && Destination < 17)
        {
            Registers[Source].Read(Value);
            Registers[Destination].Write(Value);
            Src = Source;
            Dest = Destination;
        }
    }  
}

void InDataBus::WritePart(int Source, int StartIndex, int NoBits, int Destination)
{
    State = InDataState::Write;
    //if source and destination are in the right range
    if(Source >= 0 && Source < 17)
    {
        if(Destination >= 0 && Destination < 17)
        {
            bool tmpValue[24];
            Registers[Source].Read(tmpValue);
            //If index is in right range
            if(StartIndex >= 0 && StartIndex < 24 && NoBits > 0)
            {
                //Work out end index
                int EndLocMod = StartIndex + NoBits;
                if(EndLocMod > 24) EndLocMod = 24;
                for(int i = 0; i < StartIndex; i++)
                {
                    Value[i] = 0;
                }
                std::copy(tmpValue + StartIndex, tmpValue + EndLocMod, Value + StartIndex);
                Registers[Destination].Write(Value);
                Src = Source;
                Dest = Destination;
            }
        }
    }
}

void InDataBus::SetIdle()
{
    State = Idle;
}
