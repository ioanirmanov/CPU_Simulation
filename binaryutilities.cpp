#include "binaryutilities.h"
#include <QtMath>
#include <vector>
#include <QString>
#include <QDebug>

BinaryUtilities::BinaryUtilities()
{

}

int BinaryUtilities::BinToDec(bool *Binary, int NoBits)
{
    int result = 0;

    //Subract value of first digit, as two compliment
    result -= ((Binary[0]) * qPow(2, NoBits - 1));

    //Loop through bits
    for(int i = 1; i < NoBits; i++)
    {
        //Add the value of each digit of the binary number to the result
        result += (Binary[i])*qPow(2, NoBits - 1 - i);
    }
    return result;
}

bool BinaryUtilities::DecToBin24Bit(int Decimal, bool *Destination)
{

    bool Result[24] = {0,0,0,0,
                      0,0,0,0,
                      0,0,0,0,
                      0,0,0,0,
                      0,0,0,0,
                      0,0,0,0};
    int WorkingNum = Decimal;

    //If negative
    if(Decimal < 0)
        WorkingNum = -Decimal;
    else WorkingNum = Decimal;
    if(WorkingNum > pow(2,23) - 1) return false;

    while(WorkingNum > 0)
    {
        //Work out largest binary digit lower than the current value and make it 1 in the binary
        //representation
        int digit = floor(log2(WorkingNum));
        Result[23 - digit] = 1;
        WorkingNum = WorkingNum - pow(2, digit);
    }

    //if negative turn positive calculated before into negative
    if(Decimal < 0) BinaryNegative24Bit(Result, Result);

    std::copy(Result, Result + 24, Destination);

    return true;
}

int BinaryUtilities::BinToDecUnsigned(bool *Binary, int NoBits)
{
    int result = 0;

    //Loop through bits
    for(int i = 0; i < NoBits; i++)
    {
        //Add the value of each digit of the binary number to the result
        result += (Binary[i])*qPow(2, NoBits - 1 - i);
    }
    return result;
}

bool BinaryUtilities::BinaryEqual(bool *ValueOne, bool *ValueTwo, int NoBits)
{
    bool Equal = true;
    for(int i = 0; i < NoBits; i++)
    {
        if(ValueOne[i] != ValueTwo[i]) Equal = false;
    }
    return Equal;
}

void BinaryUtilities::BinaryAdd24Bit(bool *ValueOne, bool *ValueTwo, bool *ResultDest)
{

    bool ValOne[24];
    bool ValTwo[24];
    bool Result[24];

    //Copy values
    std::copy(ValueOne, ValueOne + 24, ValOne);
    std::copy(ValueTwo, ValueTwo + 24, ValTwo);

    bool carry = false;

    //Loop through bits starting with least significant bit
    for(int i = 23 ; i >= 0; i--)
    {
        //XOr bits, then XOr that reuslt with carry
        Result[i] = ValOne[i] ^ ValTwo[i];
        Result[i] = Result[i] ^ carry;
        //If at least two of the bits are 1 carry is 1
        carry = (ValOne[i] && ValTwo[i]) || (carry && ValOne[i]) || (carry && ValTwo[i]);
    }

    //Copy result to desination
    std::copy(Result, Result + 24, ResultDest);
}

void BinaryUtilities::BinaryNegative24Bit(bool *InValue, bool *ResultDest)
{
    bool Result[24];
    std::copy(InValue, InValue + 24, Result);
    //Inverse Bits
    for(int i = 0; i < 24; i++)
    {
        Result[i] = !Result[i];
    }
    bool tmpBool[24] = {0,0,0,0,0,0,0,0,
                        0,0,0,0,0,0,0,0,
                        0,0,0,0,0,0,0,1};
    //Add One
    BinaryAdd24Bit(Result, tmpBool, Result);

    //Copy result to desination
    std::copy(Result, Result + 24, ResultDest);
}

void BinaryUtilities::BinaryRightShift24Bit(bool *InValue, int ShiftAmount, bool *ResultDest)
{
    if(ShiftAmount > 0)
    {
        bool ValueCopy[24];
        std::copy(InValue, InValue + 24, ValueCopy);
        bool WorkingValue[24]= {0,0,0,0,0,0,0,0
                                ,0,0,0,0,0,0,0,0
                                ,0,0,0,0,0,0,0,0};;
        //Loops through bits
        for(int i = 0; i + ShiftAmount < 24; i++)
        {
            //Shift bits
            WorkingValue[i+ShiftAmount] = ValueCopy[i];
        }

        //Copy result to desination
        std::copy(WorkingValue, WorkingValue + 24, ResultDest);
    }
}

void BinaryUtilities::BinaryLeftShift24Bit(bool *InValue, int ShiftAmount, bool *ResultDest)
{
    if(ShiftAmount > 0)
    {
        bool ValueCopy[24];
        std::copy(InValue, InValue + 24, ValueCopy);
        bool WorkingValue[24] = {0,0,0,0,0,0,0,0
                                ,0,0,0,0,0,0,0,0
                                ,0,0,0,0,0,0,0,0};
        for(int i = 23; i - ShiftAmount >= 0; i--)
        {
            WorkingValue[i-ShiftAmount] = ValueCopy[i];
        }

        //Copy result to desination
        std::copy(WorkingValue, WorkingValue + 24, ResultDest);
    }
}

void BinaryUtilities::BinaryNot24Bit(bool *InValue, bool *SourceDest)
{
    bool InVal[24];
    std::copy(InValue, InValue + 24, InVal);
    for(int i = 0; i < 24; i++)
    {
        InVal[i] = !InVal[i];
    }

    //Copy result to desination
    std::copy(InVal, InVal + 24, SourceDest);
}

void BinaryUtilities::BinaryAnd24Bit(bool *ValueOne, bool *ValueTwo, bool *ResultDest)
{
    bool ValOne[24];
    bool ValTwo[24];
    bool Result[24];

    //Copy values
    std::copy(ValueOne, ValueOne + 24,ValOne);
    std::copy(ValueTwo, ValueTwo + 24,ValTwo);

    //Loop through bits
    for(int i = 0; i < 24; i++)
    {
        //Logically and bits
        Result[i] = ValOne[i] && ValTwo[i];
    }

    //Copy result to desination
    std::copy(Result, Result + 24, ResultDest);
}

void BinaryUtilities::BinaryOr24Bit(bool *ValueOne, bool *ValueTwo, bool *ResultDest)
{
    bool ValOne[24];
    bool ValTwo[24];
    bool Result[24];

    //Copy values
    std::copy(ValueOne, ValueOne + 24,ValOne);
    std::copy(ValueTwo, ValueTwo + 24,ValTwo);

    //Loop through bits
    for(int i = 0; i < 24; i++)
    {
        //Logically Or bits
        Result[i] = ValOne[i] || ValTwo[i];
    }

    //Copy result to desination
    std::copy(Result, Result + 24, ResultDest);
}

void BinaryUtilities::BinaryEor24Bit(bool *ValueOne, bool *ValueTwo, bool *ResultDest)
{
    bool ValOne[24];
    bool ValTwo[24];
    bool Result[24];

    //Copy values
    std::copy(ValueOne, ValueOne + 24,ValOne);
    std::copy(ValueTwo, ValueTwo + 24,ValTwo);

    //Loop through bits
    for(int i = 0; i < 24; i++)
    {
        //Logically XOr bits
        Result[i] = ValOne[i] ^ ValTwo[i];
    }

    //Copy result to desination
    std::copy(Result, Result + 24, ResultDest);
}

QString BinaryUtilities::ToString(bool *Binary, int NoBits)
{
    QString result = "";
    //Loop through bits
    for(int i = 0; i < NoBits; i++)
    {
        //Add digit
        if(Binary[i]) result.append("1");
        else result.append("0");
    }
    return result;
}

QString BinaryUtilities::ToString24BitSpaced(bool *Binary)
{
    QString result = "";
    //Loop through bits
    for(int i = 0; i < 24; i++)
    {
        if(i == 4 || i == 8 || i == 12  || i == 16 || i == 20) result.append(" ");
        //Add digit
        if(Binary[i]) result.append("1");
        else result.append("0");
    }
    return result;
}

QString BinaryUtilities::ToStringHex24Bits(bool *Binary)
{
    QString result = "";

    static const char* const HexChars = "0123456789ABCDEF";

    result.append(HexChars[BinToDecUnsigned(Binary, 4)]);
    result.append(HexChars[BinToDecUnsigned(Binary + 4, 4)]);
    result.append(HexChars[BinToDecUnsigned(Binary + 8, 4)]);
    result.append(HexChars[BinToDecUnsigned(Binary + 12, 4)]);
    result.append(HexChars[BinToDecUnsigned(Binary + 16, 4)]);
    result.append(HexChars[BinToDecUnsigned(Binary + 20, 4)]);
    return result;
}

bool BinaryUtilities::StringDecToBin(class QString Str, bool *Destination)
{
    bool Parsed;
    int Dec = Str.toInt(&Parsed, 10);
    if(!Parsed) return false;
    return DecToBin24Bit(Dec, Destination);
}

bool BinaryUtilities::StringHexToBin(class QString Str, bool *Destination)
{
    bool Parsed;
    int Dec = Str.toInt(&Parsed, 16);
    if(!Parsed) return false;
    return DecToBin24Bit(Dec, Destination);
}

bool BinaryUtilities::StringBinToBin(class QString Str, bool *Destination)
{
    bool Parsed;
    int Dec = Str.toInt(&Parsed, 2);
    if(!Parsed) return false;
    return DecToBin24Bit(Dec, Destination);
}





