#ifndef CONTROLBUS_H
#define CONTROLBUS_H



enum CntrlBusState : int
{
    CntrlIdle = 0,
    CntrlRead = 1,
    CntrlWrite = 2
};

class ControlBus
{
public:
    ControlBus();
    ControlBus(class Memory *MemPointer);
    
    void SetRead();
    void SetWrite();
    void SetIdle();
    
    int GetState();
    CntrlBusState GetStateEnum();
private:
    class Memory *Mem;
    CntrlBusState State = CntrlIdle;
};

#endif // CONTROLBUS_H
