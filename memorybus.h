#ifndef MEMORYBUS_H
#define MEMORYBUS_H

#pragma once



enum MemBusState : int
{
  BusIdle = 0,
    BusWriteMemory = 1,
    BusWriteRegister = 2
};


class MemoryBus
{
public:
    MemoryBus();
    MemoryBus(class Memory *MemPointer, class Register *RegPointer);
    virtual ~MemoryBus();
    virtual void WriteToMemory();

    int GetState();
    MemBusState GetStateEnum();
    void GetValue(bool Destination[24]);
    void SetIdle();
protected:
    class Register *Reg;
    class Memory *Mem;
    MemBusState State = BusIdle;
    bool Value[24];
};

#endif // MEMORYBUS_H
