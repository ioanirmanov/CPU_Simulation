#ifndef ARITHLOGUNIT_H
#define ARITHLOGUNIT_H
#include<QMap>

enum AluState :int
{
    AluIdle = 0,
    AluActive = 1
};

class ArithLogUnit
{
public:
    ArithLogUnit();
    ArithLogUnit(class Register *OpOneRegPtr, class Register *OpTwoRegPtr, class Register *ResultRegPtr, class Register *FlagRegPtr);
    void WriteInstruction(bool value[4]);
    int GetState();
    AluState GetStateEnum();
    void SetStateIdle();
private:
    static const int OperationNumber = 9;
    //Array of pointers to operation functions
    void(ArithLogUnit::*FuncArray[OperationNumber])() = {&ArithLogUnit::Add, &ArithLogUnit::Subtract, &ArithLogUnit::And, &ArithLogUnit::Or, &ArithLogUnit::ExOr, &ArithLogUnit::Not, &ArithLogUnit::LeftShift, &ArithLogUnit::RightShift, &ArithLogUnit::Compare};

    //Function to execute operation
    void ExecuteOperation(int Index);

    //Operation functions
    void Add();
    void Subtract();
    void And();
    void Or();
    void ExOr();
    void Not();
    void LeftShift();
    void RightShift();
    void Compare();

    //Operand registers
    class Register *OpOne;
    class Register *OpTwo;

    //Result and flag register
    class Register *ResultReg;
    class Register *FlagReg;

    bool InstructionValue[4];
    bool OpOneValue[24];
    bool OpTwoValue[24];
    bool OutValue[24];

    AluState State;
};

#endif // ARITHLOGUNIT_H
