#include "assembler.h"
#include <QString>
#include <QStringList>
#include <algorithm>
#include "binaryutilities.h"
#include "memoryhelper.h"
#include <vector>

#include <QDebug>

Assembler::Assembler()
{

}

QMap<int, QString> Assembler::ReferencedLabels = QMap<int, QString>();

bool Assembler::Assemble(class QString Text, class MemoryHelper *MemHelper)
{
    //Function pointer array to functions that decode instructions
    bool (*InstructionFuncArray[InstructionNumber])(class QStringList, int, class MemoryHelper*) = {&Assembler::Load, &Assembler::Store, &Assembler::LoadIndirect, &Assembler::StoreIndirect,
                            &Assembler::Move,&Assembler::Add, &Assembler::Subtract, &Assembler::And,
                            &Assembler::Or, &Assembler::ExOr, &Assembler::Not, &Assembler::LeftShift,
                            &Assembler::RightShift,&Assembler::Compare, &Assembler::Branch, &Assembler::Halt};
    //Map to store labels key = label name, value = line number of label
    QMap<class QString, int> Labels;
    //Used to store labels branch instructions branch to value = line branch instruction is on, key = label name
    ReferencedLabels = QMap<int, QString>();
    ReferencedLabels.clear();

    //Map to map instruction mnemonic to index in function pointer array
    QMap<QString, int> InstructionString;
    InstructionString["LDR"] = 0;
    InstructionString["STR"] = 1;
    InstructionString["LDI"] = 2;
    InstructionString["STI"] = 3;
    InstructionString["MOV"] = 4;
    InstructionString["ADD"] = 5;
    InstructionString["SUB"] = 6;
    InstructionString["AND"] = 7;
    InstructionString["ORR"] = 8;
    InstructionString["EOR"] = 9;
    InstructionString["MVN"] = 10;
    InstructionString["LSL"] = 11;
    InstructionString["LSR"] = 12;
    InstructionString["CMP"] = 13;
    InstructionString["BRN"] = 14;
    InstructionString["HALT"] = 15;

    //Split text into lines
    QStringList Lines = Text.split("\n");

    //if the last line is empty
    while(Lines.size() > 0 && Lines[Lines.size()-1] == "") Lines.removeAt(Lines.size() - 1);

    //Remove lines that are commented out
    std::vector<int> CommentedLines;

    for(int i = 0; i < Lines.length(); i++)
    {
        QString Line = Lines.at(i);
        if(Line[0] == '/' && Line[1] == '/')
        {
            CommentedLines.push_back(i);
            qDebug() << "Line " << i << " is a comment";
        }
    }

    for(std::size_t i = 0; i < CommentedLines.size(); i++)
    {
        Lines.removeAt(CommentedLines[i] - i);
        qDebug() << "Line " << CommentedLines[i] << " removed";
    }

    if(Lines.length() > 256 || Lines.length() < 1) return false;

    //Go through lines and if they are valid, start with mnemonic or mnemonic after label pass them to correspondign fucntion to be decoded
    for(int i = 0; i < Lines.length(); i++)
    {
        QStringList Line = Lines.at(i).split(" ");
        if(InstructionString.contains(Line.at(0)))
        {
            int FuncIndex = InstructionString.value(Line.at(0));
            if(!InstructionFuncArray[FuncIndex](Line, i, MemHelper)) return false;
        }
        else if(Line.length() >= 2)
        {
            if(InstructionString.contains(Line.at(1)))
            {
                 Labels[Line.at(0)] = i;
                 Line.removeAt(0);
                 int FuncIndex = InstructionString.value(Line.at(0));
                 if(!InstructionFuncArray[FuncIndex](Line, i, MemHelper)) return false;

            }
            else return false;
        }
        else return false;
    }

    //Loop through referenced labels and if they are valid add memory location to branch to in branch instructions
    QMapIterator<int, QString> i(ReferencedLabels);
    while (i.hasNext()) {
        i.next();
        if(Labels.contains(i.value()))
        {
            bool Instruction[24];
            bool BinAddress[24];
            //Convert line number of label to binary
            BinaryUtilities::DecToBin24Bit(Labels.value(i.value()), BinAddress);
            //Copy branch instruction
            MemHelper->GetLocation(i.key(), Instruction);
            //Copy binary address into branch instruction
            std::copy(BinAddress + 16, BinAddress + 24, Instruction + 16);
            //Write new instruction back into memoryhelper
            MemHelper->SetLocation(Instruction, i.key());
        }
        else return false;
    }

}

bool Assembler::Load(class QStringList Line, int LineNo, class MemoryHelper *MemHelper)
{
    //In format LDR Rd <memort ref>

    bool Instruction[24]= {0,0,0,0,
                           0,0,0,0,
                           0,0,0,0,
                           0,0,0,0,
                           0,0,0,0,
                           0,0,0,0};

    bool OpCode[8] = {0,0,0,0,
                     0,0,0,0};

    std::copy(OpCode, OpCode + 8, Instruction);

    //if the line has the right number of words
    if(Line.length() == 3)
    {
        QString LineSection = Line.at(1);
        //Check first word is in the right format
        if(LineSection[0] == 'R')
        {
            LineSection.remove(0, 1);
            bool Parsed;
            int Number = LineSection.toInt(&Parsed, 10);
            //If value not in range for registers
            if(!Parsed || Number < 0 || Number > 15) return false;
            bool Bin[24];
            BinaryUtilities::DecToBin24Bit(Number, Bin);
            //Copy to save register part of instruction
            std::copy(Bin + 20, Bin + 24, Instruction + 8);

            LineSection = Line.at(2);
            //If direct addressing
            if(LineSection[0] == 'R')
            {
                //Set addressing mode
                Instruction[15] = 0;
                LineSection.remove(0, 1);
                Number = LineSection.toInt(&Parsed, 10);
                if(!Parsed || Number < 0 || Number > 15) return false;
            }
            //If immediate addressing
            else if(LineSection[0] == '#')
            {
                //Set addressing mode
                Instruction[15] = 1;
                LineSection.remove(0, 1);
                Number = LineSection.toInt(&Parsed, 10);
                if(!Parsed || Number < 0 || Number > 255) return false;
            }
            else return false;

            //Copy operand to the instruction
            BinaryUtilities::DecToBin24Bit(Number, Bin);
            std::copy(Bin + 16, Bin + 24, Instruction + 16);

            MemHelper->SetLocation(Instruction, LineNo);

            return true;
        }
        else return false;
    }
    else return false;
}

bool Assembler::Store(class QStringList Line, int LineNo, class MemoryHelper *MemHelper)
{
    //Instruction in format STR Rd <memory ref>

    bool Instruction[24]= {0,0,0,0,
                           0,0,0,0,
                           0,0,0,0,
                           0,0,0,0,
                           0,0,0,0,
                           0,0,0,0};

    bool OpCode[8] = {0,0,0,0,
                     0,0,0,1};

    std::copy(OpCode, OpCode + 8, Instruction);
    //if the line has the right number of words
    if(Line.length() == 3)
    {
        QString LineSection = Line.at(1);
        //Check first word is in the right format
        if(LineSection[0] == 'R')
        {
            LineSection.remove(0, 1);
            bool Parsed;
            int Number = LineSection.toInt(&Parsed, 10);
            if(!Parsed || Number < 0 || Number > 15) return false;
            bool Bin[24];
            BinaryUtilities::DecToBin24Bit(Number, Bin);
            std::copy(Bin + 20, Bin + 24, Instruction + 8);

            LineSection = Line.at(2);
            if(LineSection[0] == 'R')
            {
                Instruction[15] = 0;
                LineSection.remove(0, 1);
                Number = LineSection.toInt(&Parsed, 10);
                if(!Parsed || Number < 0 || Number > 15) return false;
            }
            else if(LineSection[0] == '#')
            {
                Instruction[15] = 1;
                LineSection.remove(0, 1);
                Number = LineSection.toInt(&Parsed, 10);
                if(!Parsed || Number < 0 || Number > 255) return false;
            }
            else return false;

            BinaryUtilities::DecToBin24Bit(Number, Bin);
            std::copy(Bin + 16, Bin + 24, Instruction + 16);

            MemHelper->SetLocation(Instruction, LineNo);

            return true;
        }
        else return false;
    }
    else return false;
}

bool Assembler::LoadIndirect(class QStringList Line, int LineNo, class MemoryHelper *MemHelper)
{
    //Instruction in format LDI Rd <memory ref>

    bool Instruction[24]= {0,0,0,0,
                           0,0,0,0,
                           0,0,0,0,
                           0,0,0,0,
                           0,0,0,0,
                           0,0,0,0};

    bool OpCode[8] = {0,0,0,0,
                     0,0,1,0};

    std::copy(OpCode, OpCode + 8, Instruction);

    //if the line has the right number of words
    if(Line.length() == 3)
    {
        QString LineSection = Line.at(1);
        //Check first word is in the right format
        if(LineSection[0] == 'R')
        {
            LineSection.remove(0, 1);
            bool Parsed;
            int Number = LineSection.toInt(&Parsed, 10);
            if(!Parsed || Number < 0 || Number > 15) return false;
            bool Bin[24];
            BinaryUtilities::DecToBin24Bit(Number, Bin);
            std::copy(Bin + 20, Bin + 24, Instruction + 8);

            LineSection = Line.at(2);
            //If direct addressing
            if(LineSection[0] == 'R')
            {
                Instruction[15] = 0;
                LineSection.remove(0, 1);
                Number = LineSection.toInt(&Parsed, 10);
                if(!Parsed || Number < 0 || Number > 15) return false;
            }
            else if(LineSection[0] == '#')
            {
                Instruction[15] = 1;
                LineSection.remove(0, 1);
                Number = LineSection.toInt(&Parsed, 10);
                if(!Parsed || Number < 0 || Number > 255) return false;
            }
            else return false;

            BinaryUtilities::DecToBin24Bit(Number, Bin);
            std::copy(Bin + 16, Bin + 24, Instruction + 16);

            MemHelper->SetLocation(Instruction, LineNo);

            return true;
        }
        else return false;
    }
    else return false;
}

bool Assembler::StoreIndirect(class QStringList Line, int LineNo, class MemoryHelper *MemHelper)
{
    //Instruction in format LDI Rd <memory ref>

    bool Instruction[24]= {0,0,0,0,
                           0,0,0,0,
                           0,0,0,0,
                           0,0,0,0,
                           0,0,0,0,
                           0,0,0,0};

    bool OpCode[8] = {0,0,0,0,
                     0,0,1,1};

    std::copy(OpCode, OpCode + 8, Instruction);

    //if the line has the right number of words
    if(Line.length() == 3)
    {
        QString LineSection = Line.at(1);

        //Check first word is in the right format
        if(LineSection[0] == 'R')
        {
            LineSection.remove(0, 1);
            bool Parsed;
            int Number = LineSection.toInt(&Parsed, 10);
            if(!Parsed || Number < 0 || Number > 15) return false;
            bool Bin[24];
            BinaryUtilities::DecToBin24Bit(Number, Bin);
            std::copy(Bin + 20, Bin + 24, Instruction + 8);

            LineSection = Line.at(2);
            //If direct addressing
            if(LineSection[0] == 'R')
            {
                Instruction[15] = 0;
                LineSection.remove(0, 1);
                Number = LineSection.toInt(&Parsed, 10);
                if(!Parsed || Number < 0 || Number > 15) return false;
            }
            else if(LineSection[0] == '#')
            {
                Instruction[15] = 1;
                LineSection.remove(0, 1);
                Number = LineSection.toInt(&Parsed, 10);
                if(!Parsed || Number < 0 || Number > 255) return false;
            }
            else return false;

            BinaryUtilities::DecToBin24Bit(Number, Bin);
            std::copy(Bin + 16, Bin + 24, Instruction + 16);

            MemHelper->SetLocation(Instruction, LineNo);

            return true;
        }
        else return false;
    }
    else return false;
}

bool Assembler::Move(class QStringList Line, int LineNo, class MemoryHelper *MemHelper)
{
    //Instruction in format MOV Rd <operand 2>

    bool Instruction[24]= {0,0,0,0,
                           0,0,0,0,
                           0,0,0,0,
                           0,0,0,0,
                           0,0,0,0,
                           0,0,0,0};

    bool OpCode[8] = {0,0,0,0,
                     0,1,0,0};

    std::copy(OpCode, OpCode + 8, Instruction);

    //if the line has the right number of words
    if(Line.length() == 3)
    {
        QString LineSection = Line.at(1);
        //Check first word is in the right format
        if(LineSection[0] == 'R')
        {
            LineSection.remove(0, 1);
            bool Parsed;
            int Number = LineSection.toInt(&Parsed, 10);
            if(!Parsed || Number < 0 || Number > 15) return false;
            bool Bin[24];
            BinaryUtilities::DecToBin24Bit(Number, Bin);
            std::copy(Bin + 20, Bin + 24, Instruction + 8);

            LineSection = Line.at(2);
            //If direct addressing
            if(LineSection[0] == 'R')
            {
                Instruction[15] = 0;
                LineSection.remove(0, 1);
                Number = LineSection.toInt(&Parsed, 10);
                if(!Parsed || Number < 0 || Number > 15) return false;
            }
            else if(LineSection[0] == '#')
            {
                Instruction[15] = 1;
                LineSection.remove(0, 1);
                Number = LineSection.toInt(&Parsed, 10);
                if(!Parsed || Number < 0 || Number > 255) return false;
            }
            else return false;

            BinaryUtilities::DecToBin24Bit(Number, Bin);
            std::copy(Bin + 16, Bin + 24, Instruction + 16);

            MemHelper->SetLocation(Instruction, LineNo);

            return true;
        }
        else return false;
    }
    else return false;
}

bool Assembler::Add(class QStringList Line, int LineNo, class MemoryHelper *MemHelper)
{
    //Instruction in format ADD Rd Rn <operand 2>

    bool Instruction[24]= {0,0,0,0,
                           0,0,0,0,
                           0,0,0,0,
                           0,0,0,0,
                           0,0,0,0,
                           0,0,0,0};

    bool OpCode[8] = {0,0,0,1,
                     0,0,0,0};

    std::copy(OpCode, OpCode + 8, Instruction);

    //if the line has the right number of words
    if(Line.length() == 4)
    {
        QString LineSection = Line.at(1);
        //Check first word is in the right format
        if(LineSection[0] == 'R')
        {
            LineSection.remove(0, 1);
            bool Parsed;
            int Number = LineSection.toInt(&Parsed, 10);
            if(!Parsed || Number < 0 || Number > 15) return false;
            bool Bin[24];
            BinaryUtilities::DecToBin24Bit(Number, Bin);
            std::copy(Bin + 20, Bin + 24, Instruction + 8);

            LineSection = Line.at(2);
            //Check if in the right format
            if(LineSection[0] == 'R')
            {
                LineSection.remove(0, 1);
                Number = LineSection.toInt(&Parsed, 10);
                if(!Parsed || Number < 0 || Number > 15) return false;
            }
            else return false;

            BinaryUtilities::DecToBin24Bit(Number, Bin);
            std::copy(Bin + 20, Bin + 24, Instruction + 16);

            LineSection = Line.at(3);
            //If direct addressing
            if(LineSection[0] == 'R')
            {
                Instruction[15] = 0;
                LineSection.remove(0, 1);
                Number = LineSection.toInt(&Parsed, 10);
                if(!Parsed || Number < 0 || Number > 15) return false;
            }
            else if(LineSection[0] == '#')
            {
                Instruction[15] = 1;
                LineSection.remove(0, 1);
                Number = LineSection.toInt(&Parsed, 10);
                if(!Parsed || Number < 0 || Number > 15) return false;
            }
            else return false;

            BinaryUtilities::DecToBin24Bit(Number, Bin);
            std::copy(Bin + 20, Bin + 24, Instruction + 20);

            MemHelper->SetLocation(Instruction, LineNo);

            return true;
        }
        else return false;
    }
    else return false;
}

bool Assembler::Subtract(class QStringList Line, int LineNo, class MemoryHelper *MemHelper)
{
    //Instruction in format SUB Rd Rn <operand 2>

    bool Instruction[24]= {0,0,0,0,
                           0,0,0,0,
                           0,0,0,0,
                           0,0,0,0,
                           0,0,0,0,
                           0,0,0,0};

    bool OpCode[8] = {0,0,0,1,
                     0,0,0,1};

    std::copy(OpCode, OpCode + 8, Instruction);

    //if the line has the right number of words
    if(Line.length() == 4)
    {
        QString LineSection = Line.at(1);
        //Check first word is in the right format
        if(LineSection[0] == 'R')
        {
            LineSection.remove(0, 1);
            bool Parsed;
            int Number = LineSection.toInt(&Parsed, 10);
            if(!Parsed || Number < 0 || Number > 15) return false;
            bool Bin[24];
            BinaryUtilities::DecToBin24Bit(Number, Bin);
            std::copy(Bin + 20, Bin + 24, Instruction + 8);

            LineSection = Line.at(2);
            if(LineSection[0] == 'R')
            {
                LineSection.remove(0, 1);
                Number = LineSection.toInt(&Parsed, 10);
                if(!Parsed || Number < 0 || Number > 15) return false;
            }
            else return false;

            BinaryUtilities::DecToBin24Bit(Number, Bin);
            std::copy(Bin + 20, Bin + 24, Instruction + 16);

            LineSection = Line.at(3);
            //If direct addressing
            if(LineSection[0] == 'R')
            {
                Instruction[15] = 0;
                LineSection.remove(0, 1);
                Number = LineSection.toInt(&Parsed, 10);
                if(!Parsed || Number < 0 || Number > 15) return false;
            }
            else if(LineSection[0] == '#')
            {
                Instruction[15] = 1;
                LineSection.remove(0, 1);
                Number = LineSection.toInt(&Parsed, 10);
                if(!Parsed || Number < 0 || Number > 15) return false;
            }
            else return false;

            BinaryUtilities::DecToBin24Bit(Number, Bin);
            std::copy(Bin + 20, Bin + 24, Instruction + 20);

            MemHelper->SetLocation(Instruction, LineNo);

            return true;
        }
        else return false;
    }
    else return false;
}


bool Assembler::And(class QStringList Line, int LineNo, class MemoryHelper *MemHelper)
{
    //Instruction in format AND Rd Rn <operand 2>

    bool Instruction[24]= {0,0,0,0,
                           0,0,0,0,
                           0,0,0,0,
                           0,0,0,0,
                           0,0,0,0,
                           0,0,0,0};

    bool OpCode[8] = {0,0,0,1,
                     0,0,1,0};

    std::copy(OpCode, OpCode + 8, Instruction);

    //if the line has the right number of words
    if(Line.length() == 4)
    {
        QString LineSection = Line.at(1);
        //Check first word is in the right format
        if(LineSection[0] == 'R')
        {
            LineSection.remove(0, 1);
            bool Parsed;
            int Number = LineSection.toInt(&Parsed, 10);
            if(!Parsed || Number < 0 || Number > 15) return false;
            bool Bin[24];
            BinaryUtilities::DecToBin24Bit(Number, Bin);
            std::copy(Bin + 20, Bin + 24, Instruction + 8);

            LineSection = Line.at(2);
            if(LineSection[0] == 'R')
            {
                LineSection.remove(0, 1);
                Number = LineSection.toInt(&Parsed, 10);
                if(!Parsed || Number < 0 || Number > 15) return false;
            }
            else return false;

            BinaryUtilities::DecToBin24Bit(Number, Bin);
            std::copy(Bin + 20, Bin + 24, Instruction + 16);

            LineSection = Line.at(3);
            //If direct addressing
            if(LineSection[0] == 'R')
            {
                Instruction[15] = 0;
                LineSection.remove(0, 1);
                Number = LineSection.toInt(&Parsed, 10);
                if(!Parsed || Number < 0 || Number > 15) return false;
            }
            else if(LineSection[0] == '#')
            {
                Instruction[15] = 1;
                LineSection.remove(0, 1);
                Number = LineSection.toInt(&Parsed, 10);
                if(!Parsed || Number < 0 || Number > 15) return false;
            }
            else return false;

            BinaryUtilities::DecToBin24Bit(Number, Bin);
            std::copy(Bin + 20, Bin + 24, Instruction + 20);

            MemHelper->SetLocation(Instruction, LineNo);

            return true;
        }
        else return false;
    }
    else return false;
}


bool Assembler::Or(class QStringList Line, int LineNo, class MemoryHelper *MemHelper)
{
    //Instruction in format ORR Rd Rn <operand 2>

    bool Instruction[24]= {0,0,0,0,
                           0,0,0,0,
                           0,0,0,0,
                           0,0,0,0,
                           0,0,0,0,
                           0,0,0,0};

    bool OpCode[8] = {0,0,0,1,
                     0,0,1,1};

    std::copy(OpCode, OpCode + 8, Instruction);

    //if the line has the right number of words
    if(Line.length() == 4)
    {
        QString LineSection = Line.at(1);
        //Check first word is in the right format
        if(LineSection[0] == 'R')
        {
            LineSection.remove(0, 1);
            bool Parsed;
            int Number = LineSection.toInt(&Parsed, 10);
            if(!Parsed || Number < 0 || Number > 15) return false;
            bool Bin[24];
            BinaryUtilities::DecToBin24Bit(Number, Bin);
            std::copy(Bin + 20, Bin + 24, Instruction + 8);

            LineSection = Line.at(2);
            if(LineSection[0] == 'R')
            {
                LineSection.remove(0, 1);
                Number = LineSection.toInt(&Parsed, 10);
                if(!Parsed || Number < 0 || Number > 15) return false;
            }
            else return false;

            BinaryUtilities::DecToBin24Bit(Number, Bin);
            std::copy(Bin + 20, Bin + 24, Instruction + 16);

            LineSection = Line.at(3);
            if(LineSection[0] == 'R')
            {
                Instruction[15] = 0;
                LineSection.remove(0, 1);
                Number = LineSection.toInt(&Parsed, 10);
                if(!Parsed || Number < 0 || Number > 15) return false;
            }
            else if(LineSection[0] == '#')
            {
                Instruction[15] = 1;
                LineSection.remove(0, 1);
                Number = LineSection.toInt(&Parsed, 10);
                if(!Parsed || Number < 0 || Number > 15) return false;
            }
            else return false;

            BinaryUtilities::DecToBin24Bit(Number, Bin);
            std::copy(Bin + 20, Bin + 24, Instruction + 20);

            MemHelper->SetLocation(Instruction, LineNo);

            return true;
        }
        else return false;
    }
    else return false;
}


bool Assembler::ExOr(class QStringList Line, int LineNo, class MemoryHelper *MemHelper)
{
    //Instruction in format EOR Rd Rn <operand 2>

    bool Instruction[24]= {0,0,0,0,
                           0,0,0,0,
                           0,0,0,0,
                           0,0,0,0,
                           0,0,0,0,
                           0,0,0,0};

    bool OpCode[8] = {0,0,0,1,
                     0,1,0,0};

    std::copy(OpCode, OpCode + 8, Instruction);

    //if the line has the right number of words
    if(Line.length() == 4)
    {
        QString LineSection = Line.at(1);
        //Check first word is in the right format
        if(LineSection[0] == 'R')
        {
            LineSection.remove(0, 1);
            bool Parsed;
            int Number = LineSection.toInt(&Parsed, 10);
            if(!Parsed || Number < 0 || Number > 15) return false;
            bool Bin[24];
            BinaryUtilities::DecToBin24Bit(Number, Bin);
            std::copy(Bin + 20, Bin + 24, Instruction + 8);

            LineSection = Line.at(2);
            if(LineSection[0] == 'R')
            {
                LineSection.remove(0, 1);
                Number = LineSection.toInt(&Parsed, 10);
                if(!Parsed || Number < 0 || Number > 15) return false;
            }
            else return false;

            BinaryUtilities::DecToBin24Bit(Number, Bin);
            std::copy(Bin + 20, Bin + 24, Instruction + 16);

            LineSection = Line.at(3);
            if(LineSection[0] == 'R')
            {
                Instruction[15] = 0;
                LineSection.remove(0, 1);
                Number = LineSection.toInt(&Parsed, 10);
                if(!Parsed || Number < 0 || Number > 15) return false;
            }
            else if(LineSection[0] == '#')
            {
                Instruction[15] = 1;
                LineSection.remove(0, 1);
                Number = LineSection.toInt(&Parsed, 10);
                if(!Parsed || Number < 0 || Number > 15) return false;
            }
            else return false;

            BinaryUtilities::DecToBin24Bit(Number, Bin);
            std::copy(Bin + 20, Bin + 24, Instruction + 20);

            MemHelper->SetLocation(Instruction, LineNo);

            return true;
        }
        else return false;
    }
    else return false;
}


bool Assembler::Not(class QStringList Line, int LineNo, class MemoryHelper *MemHelper)
{
    //Instruction in format MVN Rd <operand 2>

    bool Instruction[24]= {0,0,0,0,
                           0,0,0,0,
                           0,0,0,0,
                           0,0,0,0,
                           0,0,0,0,
                           0,0,0,0};

    bool OpCode[8] = {0,0,0,1,
                     0,1,0,1};

    std::copy(OpCode, OpCode + 8, Instruction);

    //if the line has the right number of words
    if(Line.length() == 3)
    {
        QString LineSection = Line.at(1);
        //Check first word is in the right format
        if(LineSection[0] == 'R')
        {
            LineSection.remove(0, 1);
            bool Parsed;
            int Number = LineSection.toInt(&Parsed, 10);
            if(!Parsed || Number < 0 || Number > 15) return false;
            bool Bin[24];
            BinaryUtilities::DecToBin24Bit(Number, Bin);
            std::copy(Bin + 20, Bin + 24, Instruction + 8);

            LineSection = Line.at(2);
            if(LineSection[0] == 'R')
            {
                Instruction[15] = 0;
                LineSection.remove(0, 1);
                Number = LineSection.toInt(&Parsed, 10);
                if(!Parsed || Number < 0 || Number > 15) return false;
            }
            else if(LineSection[0] == '#')
            {
                Instruction[15] = 1;
                LineSection.remove(0, 1);
                Number = LineSection.toInt(&Parsed, 10);
                if(!Parsed || Number < 0 || Number > 15) return false;
            }
            else return false;

            BinaryUtilities::DecToBin24Bit(Number, Bin);
            std::copy(Bin + 20, Bin + 24, Instruction + 20);

            MemHelper->SetLocation(Instruction, LineNo);

            return true;
        }
        else return false;
    }
    else return false;
}


bool Assembler::LeftShift(class QStringList Line, int LineNo, class MemoryHelper *MemHelper)
{
    //Instruction in format LSL Rd Rn <operand 2>

    bool Instruction[24]= {0,0,0,0,
                           0,0,0,0,
                           0,0,0,0,
                           0,0,0,0,
                           0,0,0,0,
                           0,0,0,0};

    bool OpCode[8] = {0,0,0,1,
                     0,1,1,0};

    std::copy(OpCode, OpCode + 8, Instruction);

    //if the line has the right number of words
    if(Line.length() == 4)
    {
        QString LineSection = Line.at(1);
        //Check first word is in the right format
        if(LineSection[0] == 'R')
        {
            LineSection.remove(0, 1);
            bool Parsed;
            int Number = LineSection.toInt(&Parsed, 10);
            if(!Parsed || Number < 0 || Number > 15) return false;
            bool Bin[24];
            BinaryUtilities::DecToBin24Bit(Number, Bin);
            std::copy(Bin + 20, Bin + 24, Instruction + 8);

            LineSection = Line.at(2);
            if(LineSection[0] == 'R')
            {
                LineSection.remove(0, 1);
                Number = LineSection.toInt(&Parsed, 10);
                if(!Parsed || Number < 0 || Number > 15) return false;
            }
            else return false;

            BinaryUtilities::DecToBin24Bit(Number, Bin);
            std::copy(Bin + 20, Bin + 24, Instruction + 16);

            LineSection = Line.at(3);
            if(LineSection[0] == 'R')
            {
                Instruction[15] = 0;
                LineSection.remove(0, 1);
                Number = LineSection.toInt(&Parsed, 10);
                if(!Parsed || Number < 0 || Number > 15) return false;
            }
            else if(LineSection[0] == '#')
            {
                Instruction[15] = 1;
                LineSection.remove(0, 1);
                Number = LineSection.toInt(&Parsed, 10);
                if(!Parsed || Number < 0 || Number > 15) return false;
            }
            else return false;

            BinaryUtilities::DecToBin24Bit(Number, Bin);
            std::copy(Bin + 20, Bin + 24, Instruction + 20);

            MemHelper->SetLocation(Instruction, LineNo);

            return true;
        }
        else return false;
    }
    else return false;
}


bool Assembler::RightShift(class QStringList Line, int LineNo, class MemoryHelper *MemHelper)
{
    //Instruction in format LSR Rd Rn <operand 2>

    bool Instruction[24]= {0,0,0,0,
                           0,0,0,0,
                           0,0,0,0,
                           0,0,0,0,
                           0,0,0,0,
                           0,0,0,0};

    bool OpCode[8] = {0,0,0,1,
                     0,1,1,1};

    std::copy(OpCode, OpCode + 8, Instruction);

    //if the line has the right number of words
    if(Line.length() == 4)
    {
        QString LineSection = Line.at(1);
        //Check first word is in the right format
        if(LineSection[0] == 'R')
        {
            LineSection.remove(0, 1);
            bool Parsed;
            int Number = LineSection.toInt(&Parsed, 10);
            if(!Parsed || Number < 0 || Number > 15) return false;
            bool Bin[24];
            BinaryUtilities::DecToBin24Bit(Number, Bin);
            std::copy(Bin + 20, Bin + 24, Instruction + 8);

            LineSection = Line.at(2);
            if(LineSection[0] == 'R')
            {
                LineSection.remove(0, 1);
                Number = LineSection.toInt(&Parsed, 10);
                if(!Parsed || Number < 0 || Number > 15) return false;
            }
            else return false;

            BinaryUtilities::DecToBin24Bit(Number, Bin);
            std::copy(Bin + 20, Bin + 24, Instruction + 16);

            LineSection = Line.at(3);
            if(LineSection[0] == 'R')
            {
                Instruction[15] = 0;
                LineSection.remove(0, 1);
                Number = LineSection.toInt(&Parsed, 10);
                if(!Parsed || Number < 0 || Number > 15) return false;
            }
            else if(LineSection[0] == '#')
            {
                Instruction[15] = 1;
                LineSection.remove(0, 1);
                Number = LineSection.toInt(&Parsed, 10);
                if(!Parsed || Number < 0 || Number > 15) return false;
            }
            else return false;

            BinaryUtilities::DecToBin24Bit(Number, Bin);
            std::copy(Bin + 20, Bin + 24, Instruction + 20);

            MemHelper->SetLocation(Instruction, LineNo);

            return true;
        }
        else return false;
    }
    else return false;
}


bool Assembler::Compare(class QStringList Line, int LineNo, class MemoryHelper *MemHelper)
{
    //Instruction in format CMP Rn <operand 2>

    bool Instruction[24]= {0,0,0,0,
                           0,0,0,0,
                           0,0,0,0,
                           0,0,0,0,
                           0,0,0,0,
                           0,0,0,0};

    bool OpCode[8] = {0,0,0,1,
                     1,0,0,0};

    std::copy(OpCode, OpCode + 8, Instruction);

    //if the line has the right number of words
    if(Line.length() == 3)
    {
        QString LineSection = Line.at(1);
        //Check first word is in the right format
        if(LineSection[0] == 'R')
        {
            LineSection.remove(0, 1);
            bool Parsed;
            int Number = LineSection.toInt(&Parsed, 10);
            if(!Parsed || Number < 0 || Number > 15) return false;
            bool Bin[24];
            BinaryUtilities::DecToBin24Bit(Number, Bin);
            std::copy(Bin + 20, Bin + 24, Instruction + 16);

            LineSection = Line.at(2);
            if(LineSection[0] == 'R')
            {
                Instruction[15] = 0;
                LineSection.remove(0, 1);
                Number = LineSection.toInt(&Parsed, 10);
                if(!Parsed || Number < 0 || Number > 15) return false;
            }
            else if(LineSection[0] == '#')
            {
                Instruction[15] = 1;
                LineSection.remove(0, 1);
                Number = LineSection.toInt(&Parsed, 10);
                if(!Parsed || Number < 0 || Number > 15) return false;
            }
            else return false;

            BinaryUtilities::DecToBin24Bit(Number, Bin);
            std::copy(Bin + 20, Bin + 24, Instruction + 20);

            MemHelper->SetLocation(Instruction, LineNo);

            return true;
        }
        else return false;
    }
    else return false;
}

bool Assembler::Branch(class QStringList Line, int LineNo, class MemoryHelper *MemHelper)
{
    //Instruction in format BRN <label> or BRN <condition> <label>

    bool Instruction[24]= {0,0,0,0,
                           0,0,0,0,
                           0,0,0,0,
                           0,0,0,0,
                           0,0,0,0,
                           0,0,0,0};

    bool OpCode[8] = {0,0,1,0,
                     0,0,0,0};

    std::copy(OpCode, OpCode + 8, Instruction);

    //In unconditional
    if(Line.length() == 2)
    {
        //Add label to referenced labels
        ReferencedLabels[LineNo] = Line.at(1);

        MemHelper->SetLocation(Instruction, LineNo);
    }
    else if(Line.length() == 3)
    {
        //Write branching condition to instruction
        QString Condition = Line.at(1);
        if(QString::compare(Condition, "EQ", Qt::CaseSensitive) == 0)
        {
            bool Equal[4] = {0,0,0,1};
            std::copy(Equal, Equal + 4, Instruction + 8);
        }
        else if(QString::compare(Condition, "GT", Qt::CaseSensitive) == 0)
        {
            bool MoreThan[4] = {0,0,1,0};
            std::copy(MoreThan, MoreThan + 4, Instruction + 8);
        }
        else if(QString::compare(Condition, "LT", Qt::CaseSensitive) == 0)
        {
            bool LessThan[4] = {0,0,1,1};
            std::copy(LessThan, LessThan + 4, Instruction + 8);
        }
        else if(QString::compare(Condition, "NE", Qt::CaseSensitive) == 0)
        {
            bool NotEqual[4] = {0,1,0,0};
            std::copy(NotEqual, NotEqual + 4, Instruction + 8);
        }
        else return false;

        ReferencedLabels[LineNo] = Line.at(2);

        MemHelper->SetLocation(Instruction, LineNo);
    }
    else return false;
}

bool Assembler::Halt(class QStringList Line, int LineNo, class MemoryHelper *MemHelper)
{
    bool Instruction[24]= {1,1,1,1,
                           0,0,0,0,
                           0,0,0,0,
                           0,0,0,0,
                           0,0,0,0,
                           0,0,0,0};


    if(Line.length() == 1)
    {
        MemHelper->SetLocation(Instruction, LineNo);
    }
    else return false;
}
