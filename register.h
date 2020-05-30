#ifndef REGISTER_H
#define REGISTER_H

enum RegState : int
{
    RegIdle = 0,
    RegRead = 1,
    RegWritten = 2
};

class Register
{
public:
    Register();
    void Write(bool Source[24]);
    void Read(bool Destination[24]);
    int GetState();
    RegState GetStateEnum();
    void SetStateIdle();

    //functions for UI
    void GetValue(bool Destination[24]);
private:
    bool Value[24];
    RegState State;
};

#endif // REGISTER_H
