#include "instructionhelper.h"
#include "binaryutilities.h"
#include <QString>
#include <algorithm>
#include "indatabus.h"
#include "memorybus.h"
#include "alucontrolbus.h"
#include "controlbus.h"
#include "register.h"
#include "memory.h"

InstructionHelper::InstructionHelper()
{

}

QString InstructionHelper::BinaryToInstruction(bool *Instruction)
{
    bool Binary[24];
    std::copy(Instruction, Instruction + 24, Binary);

    QString result = "";

    //0 = Data, 1 = Arith, 2 = Branch, 15 = HALT
    int Type = BinaryUtilities::BinToDecUnsigned(Binary, 4);
    if(Type == 0)
    {
        QString InstruArr[5] = {"LDR", "STR", "LDI", "STI", "MOV"};
        int InstructionNum = BinaryUtilities::BinToDecUnsigned(Binary + 4, 4);
        if(InstructionNum >= 0 && InstructionNum < 5)
        {
            //Write instruction mnemonic
            result.append(InstruArr[InstructionNum]);
        }
        else return "";

        int SaveRegNum = BinaryUtilities::BinToDecUnsigned(Binary + 8, 4);
        //If in right range for register add register number
        if(SaveRegNum >= 0 && SaveRegNum < 16)
        {
            result.append(" R" + QString::number(SaveRegNum));
        }
        else return "";

        //0 = direct, 1 = immediate
        int AddressingMode = BinaryUtilities::BinToDecUnsigned(Binary + 12, 4);
        if(AddressingMode == 0)
        {
            result.append(" R");
        }
        else if(AddressingMode == 1)
        {
            result.append(" #");
        }
        else return "";

        int Operand = BinaryUtilities::BinToDecUnsigned(Binary + 16, 8);
        if(Operand >= 0)
        {
            if(AddressingMode == 0)
            {
                //If not in range for register
                if(Operand >= 16) return "";
            }
            result.append(QString::number(Operand));
        }
    }
    else if(Type == 1)
    {
        QString InstruArr[9] = {"ADD", "SUB", "AND", "ORR", "EOR", "MVN", "LSL", "LSR", "CMP"};
        int InstructionNum = BinaryUtilities::BinToDecUnsigned(Binary + 4, 4);
        if(InstructionNum >= 0 && InstructionNum < 9)
        {
            //Write instruction mnemonic
            result.append(InstruArr[InstructionNum]);
        }
        else return "";

        int SaveRegNum = BinaryUtilities::BinToDecUnsigned(Binary + 8, 4);
        //If in right range for register add register number
        if(SaveRegNum >= 0 && SaveRegNum < 16)
        {
            //If not compare instruction
            //No save register for compare
            if(InstructionNum != 8)
            {
                result.append(" R" + QString::number(SaveRegNum));
            }
        }
        else return "";

        int RegNum = BinaryUtilities::BinToDecUnsigned(Binary + 16, 4);
        //If in right range for register add register number
        //If not a not instruction
        if(RegNum >= 0 && RegNum < 16 && InstructionNum != 5)
        {
            result.append(" R" + QString::number(RegNum));
        }

        int AddressingMode = BinaryUtilities::BinToDecUnsigned(Binary + 12, 4);
        //0 = direct, 1 = immediate
        if(AddressingMode == 0)
        {
            result.append(" R");
        }
        else if(AddressingMode == 1)
        {
            result.append(" #");
        }
        else return "";

        int Operand = BinaryUtilities::BinToDecUnsigned(Binary + 20, 4);
        //If valid operand
        if(Operand >= 0)
        {
            if(AddressingMode == 0)
            {
                if(Operand >= 16) return "";
            }
            result.append(QString::number(Operand));
        }
    }
    else if(Type == 2)
    {
        int InstructionNum = BinaryUtilities::BinToDecUnsigned(Binary + 4, 4);
        //Make sure opcide is right
        if(InstructionNum == 0)
        {
            result.append("BRN");
        }
        else return "";

        //Add branch condition
        int BranchCond = BinaryUtilities::BinToDecUnsigned(Binary + 8, 4);
        if(BranchCond <= 4 && BranchCond >= 0)
        {
            if(BranchCond == 1) result.append(" EQ");
            else if(BranchCond == 2) result.append(" GT");
            else if(BranchCond == 3) result.append(" LT");
            else if(BranchCond == 4) result.append(" NE");
        }
        else return "";

        //Add address in hex
        int AddressDec = BinaryUtilities::BinToDecUnsigned(Binary + 16, 8);
        QString AddressHex = QString::number(AddressDec, 16).toUpper();
        if(AddressDec >= 0) result.append(" 0x" + AddressHex);
    }
    else if(Type == 15)
    {
        result = "HALT";
    }
    else return "";


    return result;
}

QString InstructionHelper::InstructionDescription(bool *Instruction)
{
    bool Binary[24];
    std::copy(Instruction, Instruction + 24, Binary);

    QString result = "";

    //0 = Data, 1 = Arith, 2 = Branch, 15 = HALT
    int Type = BinaryUtilities::BinToDecUnsigned(Binary, 4);
    if(Type == 0)
    {
        //QString InstruArr[5] = {"Loading", "Storing", "LDI", "STI", "MOV"};
        int InstructionNum = BinaryUtilities::BinToDecUnsigned(Binary + 4, 4);
        int SaveRegNum = BinaryUtilities::BinToDecUnsigned(Binary + 8, 4);
        int AddressingMode = BinaryUtilities::BinToDecUnsigned(Binary + 12, 4);
        int Operand = BinaryUtilities::BinToDecUnsigned(Binary + 16, 8);
        if(InstructionNum == 0)
        {
            //Direct
            if(AddressingMode == 0)
            {
                result = QString("Load value in the memory location stored in register %1 into register %2")
                        .arg(Operand).arg(SaveRegNum);
            }
            //Immediate
            else if (AddressingMode == 1)
            {
                result = QString("Load value in the memory location %1 into register %2")
                        .arg(Operand).arg(SaveRegNum);
            }
            else return "";
        }
        else if(InstructionNum == 1)
        {
            //Direct
            if(AddressingMode == 0)
            {
                result = QString("Store value from register %1 into the memory location stored in register %2")
                        .arg(SaveRegNum).arg(Operand);
            }
            //mmediate
            else if (AddressingMode == 1)
            {
                result = QString("Store value from register %1 into the memory location %2")
                        .arg(SaveRegNum).arg(Operand);
            }
            else return "";
        }
        else if(InstructionNum == 2)
        {
            //Direct
            if(AddressingMode == 0)
            {
                result = QString("Indirectly load value pointed to by the value in the memory location stored in register %1 into register %2")
                        .arg(Operand).arg(SaveRegNum);
            }
            //Immediate
            else if (AddressingMode == 1)
            {
                result = QString("Indirectly load value pointed to by the value in the memory location %1 into register %2")
                        .arg(Operand).arg(SaveRegNum);
            }
            else return "";
        }
        else if(InstructionNum == 3)
        {
            //Direct
            if(AddressingMode == 0)
            {
                result = QString("Indirectly store value from register %1 into the memory location pointed to by the value in the memory location stored in register %2")
                        .arg(SaveRegNum).arg(Operand);
            }
            //mmediate
            else if (AddressingMode == 1)
            {
                result = QString("Indirectly store value from register %1 into the memory location pointed to by the value in the memory location %2")
                        .arg(SaveRegNum).arg(Operand);
            }
            else return "";
        }
        else if(InstructionNum == 4)
        {
            //Direct
            if(AddressingMode == 0)
            {
                result = QString("Move the value stored in register %1 to register %2")
                        .arg(Operand).arg(SaveRegNum);
            }
            //Immediate
            else if (AddressingMode == 1)
            {
                result = QString("Move the value %1 to register %2")
                        .arg(Operand).arg(SaveRegNum);
            }
            else return "";
        }
        else return "";
    }
    else if(Type == 1)
    {
        //QString InstruArr[9] = {"ADD", "SUB", "AND", "ORR", "EOR", "MVN", "LSL", "LSR", "CMP"};
        int InstructionNum = BinaryUtilities::BinToDecUnsigned(Binary + 4, 4);
        int SaveRegNum = BinaryUtilities::BinToDecUnsigned(Binary + 8, 4);
        int RegNum = BinaryUtilities::BinToDecUnsigned(Binary + 16, 4);
        int AddressingMode = BinaryUtilities::BinToDecUnsigned(Binary + 12, 4);
        int Operand = BinaryUtilities::BinToDecUnsigned(Binary + 20, 4);

        switch(InstructionNum)
        {
        case 0:
            //Direct
            if(AddressingMode == 0)
            {
                result = QString("Add the value in register %1 to the value in register %2 and store the result in register %3")
                        .arg(Operand).arg(RegNum).arg(SaveRegNum);
            }
            //Immediate
            else if (AddressingMode == 1)
            {
                result = QString("Add the value %1 to the value in register %2 and store the result in register %3")
                        .arg(Operand).arg(RegNum).arg(SaveRegNum);
            }
            else return "";
            break;
        case 1:
            //Direct
            if(AddressingMode == 0)
            {
                result = QString("Subtract the value in register %1 from the value in register %2 and store the result in register %3")
                        .arg(Operand).arg(RegNum).arg(SaveRegNum);
            }
            //Immediate
            else if (AddressingMode == 1)
            {
                result = QString("Subtract the value %1 from the value in register %2 and store the result in register %3")
                        .arg(Operand).arg(RegNum).arg(SaveRegNum);
            }
            else return "";
            break;
        case 2:
            //Direct
            if(AddressingMode == 0)
            {
                result = QString("Logically AND the value in register %1 with the value in register %2 and store the result in register %3")
                        .arg(Operand).arg(RegNum).arg(SaveRegNum);
            }
            //Immediate
            else if (AddressingMode == 1)
            {
                result = QString("Logically AND the value %1 with the value in register %2 and store the result in register %3")
                        .arg(Operand).arg(RegNum).arg(SaveRegNum);
            }
            else return "";
            break;
        case 3:
            //Direct
            if(AddressingMode == 0)
            {
                result = QString("Logically OR the value in register %1 with the value in register %2 and store the result in register %3")
                        .arg(Operand).arg(RegNum).arg(SaveRegNum);
            }
            //Immediate
            else if (AddressingMode == 1)
            {
                result = QString("Logically OR the value %1 with the value in register %2 and store the result in register %3")
                        .arg(Operand).arg(RegNum).arg(SaveRegNum);
            }
            else return "";
            break;
        case 4:
            //Direct
            if(AddressingMode == 0)
            {
                result = QString("Logically XOR the value in register %1 with the value in register %2 and store the result in  register %3")
                        .arg(Operand).arg(RegNum).arg(SaveRegNum);
            }
            //Immediate
            else if (AddressingMode == 1)
            {
                result = QString("Logically XOR the value %1 with the value in register %2 and store the result in register %3")
                        .arg(Operand).arg(RegNum).arg(SaveRegNum);
            }
            else return "";
            break;
        case 5:
            //Direct
            if(AddressingMode == 0)
            {
                result = QString("Logically NOT the value in register %1 and store the result in register %3")
                        .arg(Operand).arg(SaveRegNum);
            }
            //Immediate
            else if (AddressingMode == 1)
            {
                result = QString("Logically NOT the value %1 and store the result in register %3")
                        .arg(Operand).arg(SaveRegNum);
            }
            else return "";
            break;
        case 6:
            //Direct
            if(AddressingMode == 0)
            {
                result = QString("Logically left shift the value in register %1 by the value in register %2 and store the result in register %3")
                        .arg(RegNum).arg(Operand).arg(SaveRegNum);
            }
            //Immediate
            else if (AddressingMode == 1)
            {
                result = QString("Logically left shift the value in register %1 by %2 and store the result in register %3")
                        .arg(RegNum).arg(Operand).arg(SaveRegNum);
            }
            else return "";
            break;
        case 7:
            //Direct
            if(AddressingMode == 0)
            {
                result = QString("Logically right shift the value in register %1 by the value in register %2 and store the result in register %3")
                        .arg(RegNum).arg(Operand).arg(SaveRegNum);
            }
            //Immediate
            else if (AddressingMode == 1)
            {
                result = QString("Logically right shift the value in register %1 by %2 and store the result in register %3")
                        .arg(RegNum).arg(Operand).arg(SaveRegNum);
            }
            else return "";
            break;
        case 8:
            //Direct
            if(AddressingMode == 0)
            {
                result = QString("Compare the value in register %1 with the value in register %2, store the result in the FLAG register")
                        .arg(RegNum).arg(Operand);
            }
            //Immediate
            else if (AddressingMode == 1)
            {
                result = QString("Compare the value in register %1 with the value %2, store the result in the FLAG register")
                        .arg(RegNum).arg(Operand);
            }
            else return "";
            break;
        default:
            return "";
        }

    }
    else if(Type == 2)
    {
        int InstructionNum = BinaryUtilities::BinToDecUnsigned(Binary + 4, 4);
        if(InstructionNum != 0) return "";
        int AddressDec = BinaryUtilities::BinToDecUnsigned(Binary + 16, 8);
        QString AddressHex = QString::number(AddressDec, 16).toUpper();

        int BranchCond = BinaryUtilities::BinToDecUnsigned(Binary + 8, 4);
        if(BranchCond == 0)
        {
            result = QString("Branch to memory address 0x%1")
                    .arg(AddressDec);
        }
        //EQ
        else if(BranchCond == 1)
        {
            result = QString("Branch to memory address 0x%1 if the value is equal to the operand")
                    .arg(AddressDec);
        }
        //GT
        else if(BranchCond == 2)
        {
            result = QString("Branch to memory address 0x%1 if the value is greater than the operand")
                    .arg(AddressDec);
        }
        //LT
        else if(BranchCond == 3)
        {
            result = QString("Branch to memory address 0x%1 if the value is less than to the operand")
                    .arg(AddressDec);
        }
        //NE
        else if(BranchCond == 4)
        {
            result = QString("Branch to memory address 0x%1 if the value is not equal to the operand")
                    .arg(AddressDec);
        }
        else return "";

    }
    else if(Type == 15)
    {
        result = "Halt the program";
    }
    else return "";

    return result;
}

QString InstructionHelper::StepDescription(InDataBus *InBus, class Memory *Mem, MemoryBus *AddBus, MemoryBus *DatBus, AluControlBus *ACoBus, ControlBus *CoBus, Register *Registers, bool *Instruction)
{
    QString result = "";

    bool Branch[8] = {0,0,1,0,0,0,0,0};
    if(InBus->GetState() == 1)
    {
        bool Value[24];
        InBus->GetValue(Value);
        QString ValueHex = BinaryUtilities::ToStringHex24Bits(Value);
        int Source = InBus->GetSource();
        int Dest = InBus->GetDestination();

        QString RegArr[16] = {"R0", "R1", "R2", "R3", "R4", "R5", "R6", "R7",
                              "PC", "CIR", "MAR", "MBR", "OP1", "OP2", "RESULT", "FLAG"};
        if(Source == 16)
        {
            result = QString("Internal data bus writing the value 0x%1 from the Control Unit to register %2.")
                    .arg(ValueHex).arg(RegArr[Dest]);
        }
        else if(Dest == 16)
        {
            result = QString("Internal data bus writing the value 0x%1 from register %2 to the Control Unit.")
                    .arg(ValueHex).arg(RegArr[Source]);
        }
        else
        {
            result = QString("Internal data bus writing the value 0x%1 from register %2 to register %3.")
                    .arg(ValueHex).arg(RegArr[Source]).arg(RegArr[Dest]);
        }
    }
    else if(AddBus->GetState() == 1)
    {
        bool Value[24];
        AddBus->GetValue(Value);
        QString ValueHex = BinaryUtilities::ToStringHex24Bits(Value);

        //If in read state
        if(Mem->GetState() == 1)
        {
            bool MemValue[24];
            Mem->GetValue(MemValue);
            QString MemValueHex = BinaryUtilities::ToStringHex24Bits(MemValue);
            result = QString("Memory address bus writing the address 0x%1 from the MAR to memory, memory fetching value 0x%2 from location 0x%1.")
                    .arg(ValueHex).arg(MemValueHex);
        }
        else
        {
            result = QString("Memory address bus writing the address 0x%1 from the MAR to memory.").arg(ValueHex);
        }
    }
    else if(DatBus->GetState() != 0)
    {
        //To memory
        if(DatBus->GetState() == 1)
        {
            bool Value[24];
            DatBus->GetValue(Value);
            QString ValueHex = BinaryUtilities::ToStringHex24Bits(Value);

            //if memory in write mode
            if(Mem->GetState() == 2)
            {
                bool WriteAddress[24];
                Mem->GetAddress(WriteAddress);
                QString AddressHex = BinaryUtilities::ToStringHex24Bits(WriteAddress);

                result = QString("Memory data bus writing the value 0x%1 from the MBR to memory, memory writing value 0x%1 to location 0x%2")
                        .arg(ValueHex).arg(AddressHex);
            }
            else
            {
                result = QString("Memory data bus writing the value 0x%1 from the MBR to memory.").arg(ValueHex);
            }
        }
        //To register
        else if(DatBus->GetState() == 2)
        {
            bool Value[24];
            DatBus->GetValue(Value);
            QString ValueHex = BinaryUtilities::ToStringHex24Bits(Value);
            result = QString("Memory data bus writing the value 0x%1 from memory to the MBR.").arg(ValueHex);
        }
    }
    else if(ACoBus->GetState() == 1)
    {
        bool Value[4];
        ACoBus->GetValue(Value);
        int ValueDec = BinaryUtilities::BinToDecUnsigned(Value, 4);

        bool Op1Value[24];
        Registers[12].GetValue(Op1Value);
        QString Op1Hex = BinaryUtilities::ToStringHex24Bits(Op1Value);

        bool Op2Value[24];
        Registers[13].GetValue(Op2Value);
        QString Op2Hex = BinaryUtilities::ToStringHex24Bits(Op2Value);

        bool ResultValue[24];
        Registers[14].GetValue(ResultValue);
        QString ResultHex = BinaryUtilities::ToStringHex24Bits(ResultValue);

        bool FlagValue[24];
        Registers[15].GetValue(FlagValue);
        int FlagDec = BinaryUtilities::BinToDecUnsigned(FlagValue, 24);

        //Add
        if(ValueDec == 0)
        {
            result = QString("ALU control bus writing. ALU adding the values 0x%1 and 0x%2, result 0x%3 being written to the RESULT register.")
                    .arg(Op1Hex).arg(Op2Hex).arg(ResultHex);
        }
        //Subtract
        else if(ValueDec == 1)
        {
            result = QString("ALU control bus writing. ALU subracting the value 0x%1 from 0x%2, result 0x%3 being written to the RESULT register.")
                    .arg(Op2Hex).arg(Op1Hex).arg(ResultHex);
        }
        //AND
        else if(ValueDec == 2)
        {
            result = QString("ALU control bus writing. ALU AND-ing the values 0x%1 and 0x%2, result 0x%3 being written to the RESULT register.")
                    .arg(Op1Hex).arg(Op2Hex).arg(ResultHex);
        }
        //OR
        else if(ValueDec == 3)
        {
            result = QString("ALU control bus writing. ALU OR-ing the values 0x%1 and 0x%2, result 0x%3 being written to the RESULT register.")
                    .arg(Op1Hex).arg(Op2Hex).arg(ResultHex);
        }
        //XOR
        else if(ValueDec == 4)
        {
            result = QString("ALU control bus writing. ALU XOR-ing the values 0x%1 and 0x%2, result 0x%3 being written to the RESULT register.")
                    .arg(Op1Hex).arg(Op2Hex).arg(ResultHex);
        }
        //NOT
        else if(ValueDec == 5)
        {
            result = QString("ALU control bus writing. ALU Not-ing the value 0x%1, result 0x%2 being written to the RESULT register.")
                    .arg(Op2Hex).arg(ResultHex);
        }
        //Left shift
        else if(ValueDec == 6)
        {
            result = QString("ALU control bus writing. ALU logically left shifting the value 0x%1 by 0x%2, result 0x%3 being written to the RESULT register.")
                    .arg(Op1Hex).arg(Op2Hex).arg(ResultHex);
        }
        //Right shift
        else if(ValueDec == 7)
        {
            result = QString("ALU control bus writing. ALU logically right shifting the value 0x%1 by 0x%2, result 0x%3 being written to the RESULT register.")
                    .arg(Op1Hex).arg(Op2Hex).arg(ResultHex);
        }
        //Compare
        else if(ValueDec == 8)
        {
            result = QString("ALU control bus writing, comparing values 0x%1 and 0x%2, ")
                    .arg(Op1Hex).arg(Op2Hex);
            //Equal
            if(FlagDec == 1)
            {
                result.append("the values are equal, writing branch flag (1) to FLAG register");
            }
            //Larger
            else if(FlagDec == 2)
            {
                result.append("the first value is larger, writing branch flag (2)  to FLAG register.");
            }
            //Smaller
            else if(FlagDec == 3)
            {
               result.append("the second value is larger, writing branch flag (3)  to FLAG register.");
            }
        }
    }
    else if(CoBus->GetState() != 0)
    {
        //Read
        if(CoBus->GetState() == 1)
        {
            result = QString("Setting memory control bus to read, setting memory to read.");
        }
        //write
        else if(CoBus->GetState() == 2)
        {
            result = QString("Setting memory control bus to write, setting memory to write.");
        }
    }
    //Incrementing PC
    else if(Registers[8].GetState() == 2)
    {
        result = QString("Incrementing PC");
    }
    //Branch and nothing else happening
    else if(BinaryUtilities::BinaryEqual(Instruction, Branch, 8))
    {
        int BranchCond = BinaryUtilities::BinToDecUnsigned(Instruction + 8, 4);
        QString BranchCondStr;
        if(BranchCond == 1) BranchCondStr = "EQ";
        else if(BranchCond == 2) BranchCondStr = "GT";
        else if(BranchCond == 3) BranchCondStr =  "LT";
        else if(BranchCond == 4) BranchCondStr = "NE";

        int Flag;
        bool FlagBin[24];
        Registers[16].GetValue(FlagBin);
        Flag = BinaryUtilities::BinToDecUnsigned(FlagBin, 24);
        QString FlagStr;
        if(Flag == 1) FlagStr = "EQ";
        else if(Flag == 2) FlagStr= "GT";
        else if(Flag == 3) FlagStr =  "LT";

        bool BranchAddress[24] = {0,0,0,0,0,0,0,0
                                 ,0,0,0,0,0,0,0,0
                                 ,0,0,0,0,0,0,0,0};
        std::copy(Instruction + 16, Instruction + 24, BranchAddress + 16);
        QString BranchAddressStr = BinaryUtilities::ToStringHex24Bits(BranchAddress);

        //If unconditional branch
        if(BranchCond == 0)
        {
            result = "Branching";
        }
        //NE branch cond and not EQ flag
        else if(BranchCond  == 4 && Flag != 1)
        {
            result = QString("Compring flag (%1) and branching conditiong (NE), flag satisfies condition, branching to address 0x%2.")
                    .arg(FlagStr).arg(BranchAddressStr);
        }
        //Branch cond and flag equal
        else if(BranchCondStr.length() >= 1 && BranchCond == Flag)
        {
            result = QString("Comapring flag (%1) and branching conditiong (%2), flag satisfies condition, branching to address 0x%3.")
                    .arg(FlagStr).arg(BranchCondStr).arg(BranchAddressStr);
        }
        //Branch cond and flag not equal
        else if(BranchCondStr.length() >= 1 && FlagStr.length() >= 1 && BranchCond != Flag)
        {
            result = QString("Comparing flag (%1) and branching conditiong (%2), flag doesn't satisfy the condition, not branching.")
                    .arg(FlagStr).arg(BranchCondStr);
        }
    }
    return result;
}
