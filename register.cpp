#include "register.h"
#include <algorithm>

Register::Register()
{
    bool tmpValue[24] = {0,0,0,0,0,0,0,0,
             0,0,0,0,0,0,0,0,
             0,0,0,0,0,0,0,0};
    std::copy(tmpValue, tmpValue + 24, Value);
    State = RegIdle;
}

void Register::Write(bool Source[24])
{
    std::copy(Source, Source + 24, Value);
    State = RegWritten;
}

void Register::Read(bool *Destination)
{
    std::copy(Value, Value + 24, Destination);
    State = RegRead;
}

int Register::GetState()
{
    return State;
}

RegState Register::GetStateEnum()
{
    return State;
}

void Register::SetStateIdle()
{
    State = RegIdle;
}

void Register::GetValue(bool *Destination)
{
    std::copy(Value, Value + 24, Destination);
}
