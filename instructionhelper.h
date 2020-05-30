#ifndef INSTRUCTIONHELPER_H
#define INSTRUCTIONHELPER_H


class InstructionHelper
{
public:
    InstructionHelper();
    //Get an assembly line instruction from binary instruction
    static class QString BinaryToInstruction(bool *Source);
    //Get a description of a binary instruction
    static class QString InstructionDescription(bool *Source);
    //Get a description of the step of the simulation from the state of the components
    static class QString StepDescription(class InDataBus *InBus, class Memory *Mem, class MemoryBus *AddBus, class MemoryBus *DatBus, class AluControlBus *ACoBus, class ControlBus *CoBus, class Register *Registers, bool Instruction[24]);
};

#endif // INSTRUCTIONHELPER_H
