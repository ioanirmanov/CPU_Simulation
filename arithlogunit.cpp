#include "arithlogunit.h"
#include<algorithm>
#include "binaryutilities.h"
#include "register.h"
#include <QDebug>

ArithLogUnit::ArithLogUnit()
{

}

ArithLogUnit::ArithLogUnit(class Register *OpOneRegPtr, class Register *OpTwoRegPtr, class Register *ResultRegPtr, class Register *FlagRegPtr)
{
    //Set pinters
    OpOne = OpOneRegPtr;
    OpTwo = OpTwoRegPtr;
    ResultReg = ResultRegPtr;
    FlagReg = FlagRegPtr;
}

void ArithLogUnit::WriteInstruction(bool *value)
{
    //Copy written value, convert to decimal, execute operation corresponding to value
    std::copy(value, value + 4, InstructionValue);
    int InstructionIndex = BinaryUtilities::BinToDecUnsigned(InstructionValue, 4);
    ExecuteOperation(InstructionIndex);
    State = AluActive;
}

int ArithLogUnit::GetState()
{
    return State;
}

AluState ArithLogUnit::GetStateEnum()
{
    return State;
}

void ArithLogUnit::SetStateIdle()
{
    State = AluIdle;
}

void ArithLogUnit::ExecuteOperation(int Index)
{
    //If index is in right range for instruction
    if(Index >= 0 && Index < OperationNumber)
    {
        //Run function at array index
        (this->*FuncArray[Index])();
    }
}

void ArithLogUnit::Add()
{
   bool ValueOne[24];
   bool ValueTwo[24];
   bool Result[24];

   //Copy values from registers
   OpOne->Read(ValueOne);
   OpTwo->Read(ValueTwo);

   //Add numbers
   BinaryUtilities::BinaryAdd24Bit(ValueOne, ValueTwo, Result);

   //Write result to Result register
   ResultReg->Write(Result);
}

void ArithLogUnit::Subtract()
{
    bool ValueOne[24];
    bool ValueTwo[24];
    bool Result[24];

    //Copy values from registers
    OpOne->Read(ValueOne);
    OpTwo->Read(ValueTwo);

    //Get nagative of the second value
    BinaryUtilities::BinaryNegative24Bit(ValueTwo, ValueTwo);
    //Add the first value to the negative of the seocnd value
    BinaryUtilities::BinaryAdd24Bit(ValueOne, ValueTwo, Result);

    //Write result to Result register
    ResultReg->Write(Result);
}

void ArithLogUnit::And()
{
    bool ValueOne[24];
    bool ValueTwo[24];
    bool Result[24];

    //Copy values from registers
    OpOne->Read(ValueOne);
    OpTwo->Read(ValueTwo);

    //Logically AND numbers
    BinaryUtilities::BinaryAnd24Bit(ValueOne, ValueTwo, Result);

    //Write result to Result register
    ResultReg->Write(Result);
}

void ArithLogUnit::Or()
{
    bool ValueOne[24];
    bool ValueTwo[24];
    bool Result[24];

    //Copy values from registers
    OpOne->Read(ValueOne);
    OpTwo->Read(ValueTwo);

    //Logically OR numbers
    BinaryUtilities::BinaryOr24Bit(ValueOne, ValueTwo, Result);

    //Write result to Result register
    ResultReg->Write(Result);
}

void ArithLogUnit::ExOr()
{
    bool ValueOne[24];
    bool ValueTwo[24];
    bool Result[24];

    //Copy values from registers
    OpOne->Read(ValueOne);
    OpTwo->Read(ValueTwo);

    //Logically XOR numbers
    BinaryUtilities::BinaryEor24Bit(ValueOne, ValueTwo, Result);

    //Write result to Result register
    ResultReg->Write(Result);
}

void ArithLogUnit::Not()
{
    bool Value[24];
    bool Result[24];

    //Copy value from register
    OpTwo->Read(Value);

    //Logically NOT Value
    BinaryUtilities::BinaryNot24Bit(Value, Result);

    //Write result to Result Register
    ResultReg->Write(Result);
}

void ArithLogUnit::LeftShift()
{
    bool Value[24];
    bool ShiftAmount[24];
    bool Result[24];

    //Copy value from register
    OpOne->Read(Value);
    OpTwo->Read(ShiftAmount);

    //Get ShiftAmount into Decimal
    int ShiftDec = BinaryUtilities::BinToDec(ShiftAmount, 24);

    //Logically Left Shift value
    BinaryUtilities::BinaryLeftShift24Bit(Value, ShiftDec, Result);

    //Write result to Result Register
    ResultReg->Write(Result);
}

void ArithLogUnit::RightShift()
{

    bool Value[24];
    bool ShiftAmount[24];
    bool Result[24];

    //Copy value from register
    OpOne->Read(Value);
    OpTwo->Read(ShiftAmount);

    //Get ShiftAmount into Decimal
    int ShiftDec = BinaryUtilities::BinToDec(ShiftAmount, 24);

    //Logically Right Shift value
    BinaryUtilities::BinaryRightShift24Bit(Value, ShiftDec, Result);

    //Write result to Result Register
    ResultReg->Write(Result);
}

void ArithLogUnit::Compare()
{
    qDebug() << "Compare";

    bool ValueOne[24];
    bool ValueTwo[24];
    bool Result[24];

    //Copy values from registers
    OpOne->Read(ValueOne);
    OpTwo->Read(ValueTwo);

    //Get nagative of the second value
    BinaryUtilities::BinaryNegative24Bit(ValueTwo, ValueTwo);
    //Add the first value to the negative of the seocnd value
    BinaryUtilities::BinaryAdd24Bit(ValueOne, ValueTwo, Result);

    //Binary value to check if subratction result is zero
    bool tmpBool[24] = {0,0,0,0,0,0,0,0,
                        0,0,0,0,0,0,0,0,
                        0,0,0,0,0,0,0,0};
    //if result of subtraction is zero, both values are equal
    if(BinaryUtilities::BinaryEqual(Result, tmpBool, 24))
    {
        qDebug() << "Equal";
        //Value that will be written to flag register
        bool Flag[24] = {0,0,0,0,0,0,0,0,
                       0,0,0,0,0,0,0,0,
                       0,0,0,0,0,0,0,1};
        //Write to Flag register
        FlagReg->Write(Flag);
    }
    //if result of subtraction is negative, first value is les sthan second
    else if(Result[0])
    {
        qDebug() << "Less than";
        //Value that will be written to flag register
        bool Flag[24] = {0,0,0,0,0,0,0,0,
                       0,0,0,0,0,0,0,0,
                       0,0,0,0,0,0,1,1};
        //Write to Flag register
        FlagReg->Write(Flag);
    }
    //Result is positive and not zero, first value is greater than second
    else
    {
        qDebug() << "More than";
        //Value that will be written to flag register
        bool Flag[24] = {0,0,0,0,0,0,0,0,
                       0,0,0,0,0,0,0,0,
                       0,0,0,0,0,0,1,0};
        //Write to Flag register
        FlagReg->Write(Flag);
    }
}
