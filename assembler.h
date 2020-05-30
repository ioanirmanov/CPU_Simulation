#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <QMap>

class Assembler
{
public:
    Assembler();
    static bool Assemble(class QString Text, class MemoryHelper *MemHelper);

private:
    //Functions to decode instructions
    static bool Load(class QStringList Line, int LineNo, class MemoryHelper *MemHelper);
    static bool Store(class QStringList Line, int LineNo, class MemoryHelper *MemHelper);
    static bool LoadIndirect(class QStringList Line, int LineNo, class MemoryHelper *MemHelper);
    static bool StoreIndirect(class QStringList Line, int LineNo, class MemoryHelper *MemHelper);
    static bool Move(class QStringList Line, int LineNo, class MemoryHelper *MemHelper);
    static bool Add(class QStringList Line, int LineNo, class MemoryHelper *MemHelper);
    static bool Subtract(class QStringList Line, int LineNo, class MemoryHelper *MemHelper);
    static bool And(class QStringList Line, int LineNo, class MemoryHelper *MemHelper);
    static bool Or(class QStringList Line, int LineNo, class MemoryHelper *MemHelper);
    static bool ExOr(class QStringList Line, int LineNo, class MemoryHelper *MemHelper);
    static bool Not(class QStringList Line, int LineNo, class MemoryHelper *MemHelper);
    static bool LeftShift(class QStringList Line, int LineNo, class MemoryHelper *MemHelper);
    static bool RightShift(class QStringList Line, int LineNo, class MemoryHelper *MemHelper);
    static bool Compare(class QStringList Line, int LineNo, class MemoryHelper *MemHelper);
    static bool Branch(class QStringList Line, int LineNo, class MemoryHelper *MemHelper);
    static bool Halt(class QStringList Line, int LineNo, class MemoryHelper *MemHelper);

    //Used to store labels branch instructions branch to value = line branch instruction is
    static QMap<int, class QString> ReferencedLabels;

    static const int InstructionNumber = 16;

};

#endif // ASSEMBLER_H
