#ifndef CONTROLUNIT_H
#define CONTROLUNIT_H

#pragma once

#include<vector>

enum CoUnState : int
{
    Halt = 0,
    Fetch = 1,
    Execute = 2,
    Start = 3
};

class ControlUnit
{
public:
    ControlUnit();
    ControlUnit(class Register *CIRPointer, class Register *PCPointer, class Register *WrkRegPointer, class InDataBus *InBusPointer, class ControlBus *CntrlBusPointer, class AddressBus *AddBusPointer, class DataBus *DataBusPointer, class AluControlBus *AlCoBusPointer);
    int GetState();
    CoUnState GetStateEnum();
    void Step();
    void SetStart();
private:

    void Fetch();
    void Execute();

    //Functions for instructions
    void Load();
    void Store();
    void LoadIndirect();
    void StoreIndirect();
    void Move();
    void Arith();
    void Branch();

    //Pointers
    class Register *CIRRegister;
    class Register *PCRegister;
    class Register *WorkReg;
    class InDataBus *InBus;
    class ControlBus *CntrlBus;
    class AluControlBus *ACoBus;
    class AddressBus *AddBus;
    class DataBus *DatBus;

    CoUnState State = CoUnState::Fetch;

    //Step functions
    void InBusWrite(std::vector<int> Args);
    void InBusWritePart(std::vector<int> Args);
    void AddressBusWrite(std::vector<int> Args);
    void DataBusWrite(std::vector<int> Args);
    void DataBusRegWrite(std::vector<int> Args);
    void SetControlBusRead(std::vector<int> Args);
    void SetControlBusWrite(std::vector<int> Args);
    void IncrementPC(std::vector<int> Args);
    void WriteAluControlBus(std::vector<int> Args);
    void BranchDecision(std::vector<int> Args);

    //Zero step functiond
    void SetControlBusIdle(std::vector<int> Args);
    void SetInBusIdle(std::vector<int> Args);
    void SetAddressBusIdle(std::vector<int> Args);
    void SetDataBusIdle(std::vector<int> Args);
    void SetAluControlBusIdle(std::vector<int> Args);


    static const int DataInstructionNumber = 5;

    //Array of pointers to instruction functions
    void(ControlUnit::*DataInstrucFuncArray[DataInstructionNumber])() = {&ControlUnit::Load, &ControlUnit::Store, &ControlUnit::LoadIndirect,
            &ControlUnit::StoreIndirect, &ControlUnit::Move};

    //Struct to store information of fucntion to run
    struct FuncQueueElement
    {
    public:
        FuncQueueElement(void (ControlUnit::*FuncPtr)(std::vector<int>), std::vector<int> Args, bool StepWhenRun);
        void (ControlUnit::*FuncPointer)(std::vector<int>);
        std::vector<int> Arguments;
        bool Step;
    };

   //Function to run queue;
   std::vector<FuncQueueElement> FuncQueue;
};

#endif // CONTROLUNIT_H
