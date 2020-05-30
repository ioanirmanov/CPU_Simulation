#include "controlunit.h"
#include "indatabus.h"
#include "addressbus.h"
#include "databus.h"
#include "register.h"
#include "controlbus.h"
#include "alucontrolbus.h"
#include "binaryutilities.h"
#include <QDebug>

ControlUnit::ControlUnit()
{

}

ControlUnit::ControlUnit(class Register *CIRPointer, class Register *PCPointer, class Register *WrkRegPointer, class InDataBus *InBusPointer, class ControlBus *CntrlBusPointer, class AddressBus *AddBusPointer, class DataBus *DataBusPointer, class AluControlBus *AlCoBusPointer)
{
    CIRRegister = CIRPointer;
    PCRegister = PCPointer;
    WorkReg = WrkRegPointer;
    InBus = InBusPointer;
    CntrlBus = CntrlBusPointer;
    AddBus = AddBusPointer;
    DatBus = DataBusPointer;
    ACoBus = AlCoBusPointer;
    State = CoUnState::Halt;
}

void ControlUnit::Step()
{
    if(FuncQueue.size() == 0)
    {
        if(State == CoUnState::Execute)
        {
            State = CoUnState::Fetch;
            Fetch(); 
        }
        else if(State == CoUnState::Fetch)
        {
            State = CoUnState::Execute;
            Execute(); 
        }
        else if(State == CoUnState::Start)
        {
            State = CoUnState::Fetch;
            Fetch();
        }
        else if(State == CoUnState::Halt) return;
    }
    bool Stepped = false;
    //Loop through and execute functions untill one that should take a step has been run
    while(FuncQueue.size() > 0 && !Stepped)
    {
        (this->*FuncQueue[0].FuncPointer)(FuncQueue[0].Arguments);
        if(FuncQueue[0].Step) Stepped = true;
        FuncQueue.erase(FuncQueue.begin());
    }
    //if queue has been finished and no function took a step run the fucntion again to move onto the next step in the cycle
    if(!Stepped) Step();
}

int ControlUnit::GetState()
{
    return State;
}

CoUnState ControlUnit::GetStateEnum()
{
    return State;
}

void ControlUnit::SetStart()
{
    State = CoUnState::Start;
    FuncQueue.clear();
}

void ControlUnit::Fetch()
{
    bool tmpBool[24];
    PCRegister->Read(tmpBool);
    qDebug() << "PC Dec Value: " << BinaryUtilities::BinToDec(tmpBool, 24);

    std::vector<int> SrcDest = {8,10};
    std::vector<int> Empty;
    //Move address from PC to MAR
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::InBusWrite, SrcDest, true));
    //Set indatabus to idle
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::SetInBusIdle, Empty, false));
    //Set control bus, and therefore memory to read
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::SetControlBusRead, Empty, true));
    //Write address to memory
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::AddressBusWrite, Empty, true));
    //Set addressbus to idle
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::SetAddressBusIdle, Empty, false));
    //Write from memory to MBR
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::DataBusRegWrite, Empty, true));
    //Set Data bus back to idle
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::SetDataBusIdle, Empty, false));
    //Set controlbus back to idle
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::SetControlBusIdle, Empty, false));
    SrcDest = {11,9};
    //Write inctruction from MBR to CIR
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::InBusWrite, SrcDest, true));
    //Set indatabus to idle
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::SetInBusIdle, Empty, false));
    //Increment PC
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::IncrementPC, Empty, true));
}

void ControlUnit::Execute()
{
    bool Instruction[24];
    CIRRegister->GetValue(Instruction);

    //Check first 4 bits too see type of instruction
    bool Data[4] = {0,0,0,0};
    bool ArithBits[4] = {0,0,0,1};
    bool Misc[4] = {0,0,1,0};
    bool Halt[4] = {1,1,1,1};
    //Get type of instruciton
    if(BinaryUtilities::BinaryEqual(Instruction, Data, 4))
    {
        qDebug() << "Running Data Instruction";
        //Get dec value of last 4 bits of opcode
        int Index = BinaryUtilities::BinToDecUnsigned(Instruction + 4, 4);
        //If index in range for array of data instructions
        if(Index >= 0 && Index < DataInstructionNumber)
        {
            //Run data instruciton function at index
            (this->*DataInstrucFuncArray[Index])();
        }
    }
    else if(BinaryUtilities::BinaryEqual(Instruction, ArithBits, 4))
    {
        qDebug() << "Running Arith Instruction";
        //Run arith function
        Arith(); 
    }
    else if(BinaryUtilities::BinaryEqual(Instruction, Misc, 4))
    {
        qDebug() << "Running Branch Instruction";
        //Only Misc fucntion is Branch
        Branch();
    }
    else if(BinaryUtilities::BinaryEqual(Instruction, Halt, 4))
    {
        qDebug() << "Running Halt Instruction";
        //Halt
        State = CoUnState::Halt;
    }
    else State = CoUnState::Halt;

}

void ControlUnit::Load()
{
    std::vector<int> Empty;

    bool Instruction[24];
    CIRRegister->GetValue(Instruction);
    //Get save register, bits 9-13 (8-12)
    int SaveReg = BinaryUtilities::BinToDecUnsigned(Instruction + 8, 4);
    //If save register is not in the right range
    if(SaveReg < 0 || SaveReg >= 16) return;
    //Get addressing mode and so load relevant value into MAR
    bool Direct[4] = {0,0,0,0};
    bool Immediate[4] = {0,0,0,1};

    //see if addressing mode is Direct, addressing mode is bits 13-16 (12-15)
    if(BinaryUtilities::BinaryEqual(Direct, Instruction + 12, 4))
    {
        //Get source register operand is bits 17-24 (16-23)
        int SourceReg = BinaryUtilities::BinToDecUnsigned(Instruction + 16, 8);
        if(SourceReg < 0 || SourceReg >= 16) return;
        std::vector<int> SrcDest = {SourceReg,10};
        //Move address from source register to MAR
        FuncQueue.push_back(FuncQueueElement(&ControlUnit::InBusWrite, SrcDest, true));
        //Set indatabus to idle
        FuncQueue.push_back(FuncQueueElement(&ControlUnit::SetInBusIdle, Empty, false));
    }
    //see if addressing mode is Immdiate
    else if (BinaryUtilities::BinaryEqual(Immediate, Instruction + 12, 4))
    {
        std::vector<int> SrcStrtNoDest = {9, 16, 8, 10};
        //Move address from CIR to MAR
        FuncQueue.push_back(FuncQueueElement(&ControlUnit::InBusWritePart, SrcStrtNoDest, true));
        //Set indatabus to idle
        FuncQueue.push_back(FuncQueueElement(&ControlUnit::SetInBusIdle, Empty, false));
    }
    else return;

    //Set control bus, and therefore memory to read
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::SetControlBusRead, Empty, true));
    //Write address to memory
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::AddressBusWrite, Empty, true));
    //Set addressbus to idle
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::SetAddressBusIdle, Empty, false));
    //Write from memory to MBR
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::DataBusRegWrite, Empty, true));
    //Set Data bus back to idle
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::SetDataBusIdle, Empty, false));
    //Set controlbus back to idle
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::SetControlBusIdle, Empty, false));
    std::vector<int> SrcDest = {11,SaveReg};
    //Write data from MBR to save register
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::InBusWrite, SrcDest, true));
    //Set indatabus to idle
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::SetInBusIdle, Empty, false));

    //Test
    /*
    std::vector<int> SrcDest;
    SrcDest.push_back(0);//Set source of move to register 0
    SrcDest.push_back(10);//Set destination of move o register 10 (MAR)
    InBusWrite(SrcDest);//Write from register 0 to 10
    SetControlBusRead(SrcDest);//Set memory to read //Args doesn't amtter here
    AddressBusWrite(SrcDest);//Write address to memory//Args deosn't matter here
    DataBusRegWrite(SrcDest);//Fetch value to MBR//Args deosn't matter here
    */

}

void ControlUnit::Store()
{
    std::vector<int> Empty;

    bool Instruction[24];
    CIRRegister->GetValue(Instruction);
    //Get save register (register that is being savedfrom), bits 9-13 (8-12)
    int SaveReg = BinaryUtilities::BinToDecUnsigned(Instruction + 8, 4);
    //If save register is not in the right range
    if(SaveReg < 0 || SaveReg >= 16) return;
    std::vector<int> SrcDest = {SaveReg,11};
    //Move data from save register to MBR
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::InBusWrite, SrcDest, true));
    //Set indatabus to idle
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::SetInBusIdle, Empty, false));

    //Get addressing mode and so load relevant value into MAR
    bool Direct[4] = {0,0,0,0};
    bool Immediate[4] = {0,0,0,1};

    //see if addressing mode is Direct, addressing mode is bits 13-16 (12-15)
    if(BinaryUtilities::BinaryEqual(Direct, Instruction + 12, 4))
    {
        //Get source register operand is bits 17-24 (16-23)
        int SourceReg = BinaryUtilities::BinToDecUnsigned(Instruction + 16, 8);
        if(SourceReg < 0 || SourceReg >= 16) return;
        SrcDest = {SourceReg,10};
        //Move address from source register to MAR
        FuncQueue.push_back(FuncQueueElement(&ControlUnit::InBusWrite, SrcDest, true));
        //Set indatabus to idle
        FuncQueue.push_back(FuncQueueElement(&ControlUnit::SetInBusIdle, Empty, false));
    }
    //see if addressing mode is Immdiate
    else if (BinaryUtilities::BinaryEqual(Immediate, Instruction + 12, 4))
    {
        std::vector<int> SrcStrtNoDest = {9, 16, 8, 10};
        //Move address from CIR to MAR
        FuncQueue.push_back(FuncQueueElement(&ControlUnit::InBusWritePart, SrcStrtNoDest, true));
        //Set indatabus to idle
        FuncQueue.push_back(FuncQueueElement(&ControlUnit::SetInBusIdle, Empty, false));
    }
    else return;

    //Set control bus, and therefore memory to write
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::SetControlBusWrite, Empty, true));
    //Write address to memory
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::AddressBusWrite, Empty, true));
    //Set addressbus to idle
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::SetAddressBusIdle, Empty, false));
    //Write data to memory
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::DataBusWrite, Empty, true));
    //Set databus to idle
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::SetDataBusIdle, Empty, false));
    //Set controlbus back to idle
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::SetControlBusIdle, Empty, false));

    //Test
    /*
    std::vector<int> SrcDest;
    SrcDest.push_back(0);//Set source of move to register 0
    SrcDest.push_back(10);//Set destination of move o register 10 (MAR)
    InBusWrite(SrcDest);//Write from register 0 to 10
    SrcDest[0] = 1;//Set source of move to register 1
    SrcDest[1] = 11;//Set source of move to register 11 (MBR)
    InBusWrite(SrcDest);//Write from register 1 to 11
    SetControlBusWrite(SrcDest);//Set memory to read //Args doesn't amtter here
    AddressBusWrite(SrcDest);//Write address to memory//Args deosn't matter here
    DataBusWrite(SrcDest);//Fetch value to MBR//Args deosn't matter here
    */
}

void ControlUnit::LoadIndirect()
{
    std::vector<int> Empty;

    bool Instruction[24];
    CIRRegister->GetValue(Instruction);
    //Get save register, bits 9-13 (8-12)
    int SaveReg = BinaryUtilities::BinToDecUnsigned(Instruction + 8, 4);
    //If save register is not in the right range
    if(SaveReg < 0 || SaveReg >= 16) return;
    //Get addressing mode and so load relevant value into MAR
    bool Direct[4] = {0,0,0,0};
    bool Immediate[4] = {0,0,0,1};

    //see if addressing mode is Direct, addressing mode is bits 13-16 (12-15)
    if(BinaryUtilities::BinaryEqual(Direct, Instruction + 12, 4))
    {
        //Get source register operand is bits 17-24 (16-23)
        int SourceReg = BinaryUtilities::BinToDecUnsigned(Instruction + 16, 8);
        if(SourceReg < 0 || SourceReg >= 16) return;
        std::vector<int> SrcDest = {SourceReg,10};
        //Move address from source register to MAR
        FuncQueue.push_back(FuncQueueElement(&ControlUnit::InBusWrite, SrcDest, true));
        //Set indatabus to idle
        FuncQueue.push_back(FuncQueueElement(&ControlUnit::SetInBusIdle, Empty, false));
    }
    //see if addressing mode is Immdiate
    else if (BinaryUtilities::BinaryEqual(Immediate, Instruction + 12, 4))
    {
        std::vector<int> SrcStrtNoDest = {9, 16, 8, 10};
        //Move address from CIR to MAR
        FuncQueue.push_back(FuncQueueElement(&ControlUnit::InBusWritePart, SrcStrtNoDest, true));
        //Set indatabus to idle
        FuncQueue.push_back(FuncQueueElement(&ControlUnit::SetInBusIdle, Empty, false));
    }
    else return;

    //Load address to load from
    //Set control bus, and therefore memory to read
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::SetControlBusRead, Empty, true));
    //Write address to memory
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::AddressBusWrite, Empty, true));
    //Set addressbus to idle
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::SetAddressBusIdle, Empty, false));
    //Write from memory to MBR
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::DataBusRegWrite, Empty, true));
    //Set Data bus back to idle
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::SetDataBusIdle, Empty, false));
    //Set controlbus back to idle
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::SetControlBusIdle, Empty, false));
    std::vector<int> SrcDest = {11,10};
    //Write address from MBR to MAR
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::InBusWrite, SrcDest, true));
    //Set indatabus to idle
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::SetInBusIdle, Empty, false));

    //Load data from loaded address
    //Set control bus, and therefore memory to read
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::SetControlBusRead, Empty, true));
    //Write address to memory
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::AddressBusWrite, Empty, true));
    //Set addressbus to idle
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::SetAddressBusIdle, Empty, false));
    //Write from memory to MBR
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::DataBusRegWrite, Empty, true));
    //Set Data bus back to idle
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::SetDataBusIdle, Empty, false));
    //Set controlbus back to idle
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::SetControlBusIdle, Empty, false));
    SrcDest = {11,SaveReg};
    //Write address from MBR to MAR
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::InBusWrite, SrcDest, true));
    //Set indatabus to idle
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::SetInBusIdle, Empty, false));

}

void ControlUnit::StoreIndirect()
{

    std::vector<int> Empty;

    bool Instruction[24];
    CIRRegister->GetValue(Instruction);
    //Get save register, bits 9-13 (8-12)
    int SaveReg = BinaryUtilities::BinToDecUnsigned(Instruction + 8, 4);
    //If save register is not in the right range
    if(SaveReg < 0 || SaveReg >= 16) return;
    //Get addressing mode and so load relevant value into MAR
    bool Direct[4] = {0,0,0,0};
    bool Immediate[4] = {0,0,0,1};

    //see if addressing mode is Direct, addressing mode is bits 13-16 (12-15)
    if(BinaryUtilities::BinaryEqual(Direct, Instruction + 12, 4))
    {
        //Get source register operand is bits 17-24 (16-23)
        int SourceReg = BinaryUtilities::BinToDecUnsigned(Instruction + 16, 8);
        if(SourceReg < 0 || SourceReg >= 16) return;
        std::vector<int> SrcDest = {SourceReg,10};
        //Move address from source register to MAR
        FuncQueue.push_back(FuncQueueElement(&ControlUnit::InBusWrite, SrcDest, true));
        //Set indatabus to idle
        FuncQueue.push_back(FuncQueueElement(&ControlUnit::SetInBusIdle, Empty, false));
    }
    //see if addressing mode is Immdiate
    else if (BinaryUtilities::BinaryEqual(Immediate, Instruction + 12, 4))
    {
        std::vector<int> SrcStrtNoDest = {9, 16, 8, 10};
        //Move address from CIR to MAR
        FuncQueue.push_back(FuncQueueElement(&ControlUnit::InBusWritePart, SrcStrtNoDest, true));
        //Set indatabus to idle
        FuncQueue.push_back(FuncQueueElement(&ControlUnit::SetInBusIdle, Empty, false));
    }
    else return;

    //Load address to load from
    //Set control bus, and therefore memory to read
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::SetControlBusRead, Empty, true));
    //Write address to memory
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::AddressBusWrite, Empty, true));
    //Set addressbus to idle
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::SetAddressBusIdle, Empty, false));
    //Write from memory to MBR
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::DataBusRegWrite, Empty, true));
    //Set Data bus back to idle
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::SetDataBusIdle, Empty, false));
    //Set controlbus back to idle
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::SetControlBusIdle, Empty, false));
    std::vector<int> SrcDest = {11,10};
    //Write address from MBR to MAR
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::InBusWrite, SrcDest, true));
    //Set indatabus to idle
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::SetInBusIdle, Empty, false));

    //Move data to BR
    SrcDest = {SaveReg,11};
    //Move data from save register to MBR
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::InBusWrite, SrcDest, true));
    //Set indatabus to idle
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::SetInBusIdle, Empty, false));

    //Store data in loaded address
    //Set control bus, and therefore memory to write
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::SetControlBusWrite, Empty, true));
    //Write address to memory
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::AddressBusWrite, Empty, true));
    //Set addressbus to idle
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::SetAddressBusIdle, Empty, false));
    //Write data to memory
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::DataBusWrite, Empty, true));
    //Set databus to idle
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::SetDataBusIdle, Empty, false));
    //Set controlbus back to idle
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::SetControlBusIdle, Empty, false));

}

void ControlUnit::Move()
{
    std::vector<int> Empty;

    bool Instruction[24];
    CIRRegister->GetValue(Instruction);
    //Get save register, bits 9-13 (8-12)
    int SaveReg = BinaryUtilities::BinToDecUnsigned(Instruction + 8, 4);
    //If save register is not in the right range
    if(SaveReg < 0 || SaveReg >= 16) return;
    //Get addressing mode and so load relevant value into MAR
    bool Direct[4] = {0,0,0,0};
    bool Immediate[4] = {0,0,0,1};

    //see if addressing mode is Direct, addressing mode is bits 13-16 (12-15)
    if(BinaryUtilities::BinaryEqual(Direct, Instruction + 12, 4))
    {
        //Get source register operand is bits 17-24 (16-23)
        int SourceReg = BinaryUtilities::BinToDecUnsigned(Instruction + 16, 8);
        if(SourceReg < 0 || SourceReg >= 16) return;
        std::vector<int> SrcDest = {SourceReg,SaveReg};
        //Move data from source register to save register
        FuncQueue.push_back(FuncQueueElement(&ControlUnit::InBusWrite, SrcDest, true));
        //Set indatabus to idle
        FuncQueue.push_back(FuncQueueElement(&ControlUnit::SetInBusIdle, Empty, false));
    }
    //see if addressing mode is Immdiate
    else if (BinaryUtilities::BinaryEqual(Immediate, Instruction + 12, 4))
    {
        std::vector<int> SrcStrtNoDest = {9, 16, 8, SaveReg};
        //Move data from CIR to save register
        FuncQueue.push_back(FuncQueueElement(&ControlUnit::InBusWritePart, SrcStrtNoDest, true));
        //Set indatabus to idle
        FuncQueue.push_back(FuncQueueElement(&ControlUnit::SetInBusIdle, Empty, false));
    }
    else return;

}

void ControlUnit::Arith()
{
    std::vector<int> Empty;

    bool Instruction[24];
    CIRRegister->GetValue(Instruction);
    //Get save register, bits 9-13 (8-12)
    int SaveReg = BinaryUtilities::BinToDecUnsigned(Instruction + 8, 4);
    //If save register is not in the right range
    if(SaveReg < 0 || SaveReg >= 16) return;

    //Move value from R), bits 17-20 (16-19) to OP1 (12)
    //Get source register operand is bits 17-24 (16-23)
    int SourceRegOne = BinaryUtilities::BinToDecUnsigned(Instruction + 16, 4);
    if(SourceRegOne < 0 || SourceRegOne >= 16) return;
    std::vector<int> SrcDest = {SourceRegOne, 12};
    //Move first operand from source register ti OP1 (12)
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::InBusWrite, SrcDest, true));
    //Set indatabus to idle
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::SetInBusIdle, Empty, false));

    //Get second operand and move it to OP2 (13)
    //Get addressing mode and so load relevant value into MAR
    bool Direct[4] = {0,0,0,0};
    bool Immediate[4] = {0,0,0,1};

    //see if addressing mode is Direct, addressing mode is bits 13-16 (12-15)
    if(BinaryUtilities::BinaryEqual(Direct, Instruction + 12, 4))
    {
        //Get source register operand is bits 21-24 (20-23)
        int SourceRegTwo = BinaryUtilities::BinToDecUnsigned(Instruction + 20, 4);
        if(SourceRegTwo < 0 || SourceRegTwo >= 16) return;
        SrcDest = {SourceRegTwo, 13};
        //Move data from source register to save register
        FuncQueue.push_back(FuncQueueElement(&ControlUnit::InBusWrite, SrcDest, true));
        //Set indatabus to idle
        FuncQueue.push_back(FuncQueueElement(&ControlUnit::SetInBusIdle, Empty, false));
    }
    //see if addressing mode is Immdiate
    else if (BinaryUtilities::BinaryEqual(Immediate, Instruction + 12, 4))
    {
        std::vector<int> SrcStrtNoDest = {9, 20, 4, 13};
        //Move data from CIR to save register
        FuncQueue.push_back(FuncQueueElement(&ControlUnit::InBusWritePart, SrcStrtNoDest, true));
        //Set indatabus to idle
        FuncQueue.push_back(FuncQueueElement(&ControlUnit::SetInBusIdle, Empty, false));
    }
    else return;

    //Send read signal with code for operation, bits 5-8 (4-7) to ALU via ALu control bus
    //Use union to convert from bool array to int
    union Argument
    {
        int i;
        bool b[4];
    };
    Argument Arg;
    std::copy(Instruction + 4, Instruction + 8, Arg.b);
    std::vector<int> ArithArg = {Arg.i};
    //Write value to ALU control bus
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::WriteAluControlBus, ArithArg, true));
    //Set ALU control bus to idle
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::SetAluControlBusIdle, Empty, false));

    if(Arg.b[0] == 1 && Arg.b[1] == 0 && Arg.b[2] == 0 && Arg.b[3] == 0) return;
    SrcDest = {14, SaveReg};
    //Move result from result register (14) to save register
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::InBusWrite, SrcDest, true));
    //Set indatabus to idle
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::SetInBusIdle, Empty, false));
}

void ControlUnit::Branch()
{
    //Condition in bits 9-12(8-11), branch address is operand
    std::vector<int> Empty;

    bool Instruction[24];
    CIRRegister->GetValue(Instruction);

    std::vector<int> SrcDest = {15, 16};
    //Move data from flag register to work register
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::InBusWrite, SrcDest, true));
    //Set indatabus to idle
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::SetInBusIdle, Empty, false));

    //Get condition from instruction
    //Use union to convert from bool array to int
    union Argument
    {
        int i;
        bool b[4];
    };
    Argument Arg;
    std::copy(Instruction + 8, Instruction + 12, Arg.b);
    std::vector<int> BranchArg = {Arg.i};

    //Make branch decision
    FuncQueue.push_back(FuncQueueElement(&ControlUnit::BranchDecision, BranchArg, true));
}

void ControlUnit::InBusWrite(std::vector<int> Args)
{
    if(Args.size() >= 2)
    {
        int Source = Args[0];
        int Destination = Args[1];

        InBus->Write(Source, Destination);
    }
}

void ControlUnit::InBusWritePart(std::vector<int> Args)
{
    if(Args.size() >= 2)
    {
        int Source = Args[0];
        int StartIndex = Args[1];
        int NoBits = Args[2];
        int Destination = Args[3];

        InBus->WritePart(Source, StartIndex, NoBits, Destination);
    }
}


void ControlUnit::AddressBusWrite(std::vector<int> Args)
{
    AddBus->WriteToMemory();
}

void ControlUnit::DataBusWrite(std::vector<int> Args)
{
    DatBus->WriteToMemory();
}

void ControlUnit::DataBusRegWrite(std::vector<int> Args)
{
    DatBus->WriteToRegister();
}

void ControlUnit::SetControlBusRead(std::vector<int> Args)
{
    CntrlBus->SetRead();
}

void ControlUnit::SetControlBusWrite(std::vector<int> Args)
{
    CntrlBus->SetWrite();
}

void ControlUnit::IncrementPC(std::vector<int> Args)
{
    bool PCValue[24];
    //get Value in PC
    PCRegister->Read(PCValue);
    //Binary value 2
    bool tmpBool[24] = {0,0,0,0,0,0,0,0,
                        0,0,0,0,0,0,0,0,
                        0,0,0,0,0,0,0,1};

    //Add 1 to value in PC, save result to NewValue;
    BinaryUtilities::BinaryAdd24Bit(PCValue, tmpBool, PCValue);

    //Write value to PC
    PCRegister->Write(PCValue);
}

void ControlUnit::WriteAluControlBus(std::vector<int> Args)
{
    if(Args.size() >= 1)
    {
        //Use union to convert from int to bool array
        union Argument
        {
            int i;
            bool b[4];
        };
        Argument Arg;
        Arg.i = Args[0];
        bool Value[4];
        std::copy(Arg.b, Arg.b + 4, Value);
        ACoBus->WriteInstruction(Value);
    }

}

void ControlUnit::BranchDecision(std::vector<int> Args)
{
    if(Args.size() >= 1)
    {
        //Use union to convert from int to bool array
        union Argument
        {
            int i;
            bool b[4];
        };
        Argument Arg;
        Arg.i = Args[0];
        bool Cond[4];
        std::copy(Arg.b, Arg.b + 4, Cond);
        bool NotEqual[4] = {0,1,0,0};

        bool tmpBool[24];
        WorkReg->Read(tmpBool);
        bool Flag[4];
        std::copy(tmpBool + 20, tmpBool + 24, Flag);
        bool Zero[4] = {0,0,0,0};

        qDebug() << "Cond: " << BinaryUtilities::BinToDecUnsigned(Cond, 4);
        qDebug() << "Flag: " << BinaryUtilities::BinToDecUnsigned(Flag, 4);

        if(BinaryUtilities::BinaryEqual(Cond, NotEqual, 4))
        {
            bool MoreThan[4] = {0,0,1,0};
            bool LessThan[4] = {0,0,1,1};
            if(BinaryUtilities::BinaryEqual(Flag, MoreThan, 4) || BinaryUtilities::BinaryEqual(Flag, LessThan, 4))
            {
                std::vector<int> SrcStrtNoDest = {9, 16, 8, 8};
                std::vector<int> Empty;
                //Move data from CIR to PC register
                FuncQueue.push_back(FuncQueueElement(&ControlUnit::InBusWritePart, SrcStrtNoDest, true));
                //Set indatabus to idle
                FuncQueue.push_back(FuncQueueElement(&ControlUnit::SetInBusIdle, Empty, false));
            }
        }
        else if(BinaryUtilities::BinaryEqual(Flag, Cond, 4) || BinaryUtilities::BinaryEqual(Zero, Cond, 4))
        {
            std::vector<int> SrcStrtNoDest = {9, 16, 8, 8};
            std::vector<int> Empty;
            //Move data from CIR to PC register
            FuncQueue.push_back(FuncQueueElement(&ControlUnit::InBusWritePart, SrcStrtNoDest, true));
            //Set indatabus to idle
            FuncQueue.push_back(FuncQueueElement(&ControlUnit::SetInBusIdle, Empty, false));
        }

    }
}

void ControlUnit::SetControlBusIdle(std::vector<int> Args)
{
    CntrlBus->SetIdle();
}

void ControlUnit::SetInBusIdle(std::vector<int> Args)
{
    InBus->SetIdle();
}

void ControlUnit::SetAddressBusIdle(std::vector<int> Args)
{
    AddBus->SetIdle();
}

void ControlUnit::SetDataBusIdle(std::vector<int> Args)
{
    DatBus->SetIdle();
}

void ControlUnit::SetAluControlBusIdle(std::vector<int> Args)
{
    ACoBus->SetIdle();
}

ControlUnit::FuncQueueElement::FuncQueueElement(void (ControlUnit::*FuncPtr)(std::vector<int>), std::vector<int> Args, bool StepWhenRun)
{
    FuncPointer = FuncPtr;
    Arguments = Args;
    Step = StepWhenRun;
}






