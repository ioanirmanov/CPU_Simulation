#ifndef ALUCONTROLBUS_H
#define ALUCONTROLBUS_H

#pragma once

enum ACoBusState : int
{
    ACoBusIdle = 0,
    ACoBusWrite = 1
};

class AluControlBus
{
public:
    AluControlBus();
    AluControlBus(class ArithLogUnit *ACoUnitPtr);
    void WriteInstruction(bool Val[4]);
    void SetIdle();

    int GetState();
    void GetValue(bool Destination[4]);
    ACoBusState GetStateEnum();
private:
    class ArithLogUnit *ACoUnit;
    //Stores value that has been written
    bool Value[4];
    ACoBusState State = ACoBusIdle;
};

#endif // ALUCONTROLBUS_H
