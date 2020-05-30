#ifndef BINARYUTILITIES_H
#define BINARYUTILITIES_H



class BinaryUtilities
{
public:
    BinaryUtilities();

    //Converts Binary to Decimal (2s compliment)
    static int BinToDec(bool Binary[24], int NoBits);
    static bool DecToBin24Bit(int Decimal, bool Destination[24]);

    //Converts Binary to Decimal (unsigned)
    static int BinToDecUnsigned(bool Binary[24], int NoBits);

    //Check if two binary values are equal
    static bool BinaryEqual(bool *ValueOne, bool *ValueTwo, int NoBits);

    //Adds two 24-Bit Binary numbers
    static void BinaryAdd24Bit(bool *ValueOne, bool *ValueTwo, bool *ResultDest);

    //Gives the negative of a twos compliment 24-bit binary number
    static void BinaryNegative24Bit(bool *InValue, bool *ResultDest);

    //Right shifts a 24-bit binary number by ShiftAmount, where ShiftAmount > 0
    static void BinaryRightShift24Bit(bool *InValue, int ShiftAmount, bool *ResultDest);

    //Left shifts a 24-bit binary number by ShiftAmount, where ShiftAmount > 0
    static void BinaryLeftShift24Bit(bool *InValue, int ShiftAmount, bool *ResultDest);

    //Bitwise Not of a 24Bit binary number
    static void BinaryNot24Bit(bool *InValue, bool *SourceDest);

    //Bitwise And of two 24 Bits Binary numbers
    static void BinaryAnd24Bit(bool *ValueOne, bool *ValueTwo, bool *ResultDest);

    //Bitwise Or of two 24 Bits Binary numbers
    static void BinaryOr24Bit(bool *ValueOne, bool *ValueTwo, bool *ResultDest);

    //Bitwise XOr of two 24 Bits Binary numbers
    static void BinaryEor24Bit(bool *ValueOne, bool *ValueTwo, bool *ResultDest);

    static class QString ToString(bool *Binary, int NoBits);
    static class QString ToString24BitSpaced(bool *Binary);
    static class QString ToStringHex24Bits(bool *Binary);

    //Decimal string to binary
    static bool StringDecToBin(class QString Str, bool Destination[24]);
    static bool StringHexToBin(class QString Str, bool Destination[24]);
    static bool StringBinToBin(class QString Str, bool Destination[24]);
};

#endif // BINARYUTILITIES_H
