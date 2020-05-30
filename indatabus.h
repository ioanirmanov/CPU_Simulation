#ifndef INDATABUS_H
#define INDATABUS_H

enum InDataState : int
{
    Idle = 0,
    Write = 1,
};


class InDataBus
{
public:
    InDataBus();
    InDataBus(class Register *RegistersPtr);

    int GetState();
    InDataState GetStateEnum();

    void GetValue(bool Destination[24]);

    int GetSource();
    int GetDestination();

    //Write full value from register to register
    void Write(int Source, int Destination);
    //Write a part of the value from one register to another
    void WritePart(int Source, int StartIndex, int NoBits, int Destination);

    void SetIdle();

private:
    class Register *Registers;

    //Stores value written by bus
    bool Value[24];
    int Src;
    int Dest;

    InDataState State;
};

#endif // INDATABUS_H
