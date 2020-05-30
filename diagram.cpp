#include "diagram.h"
#include "register.h"
#include "binaryutilities.h"
#include "arithlogunit.h"
#include "memory.h"
#include "indatabus.h"
#include "memorybus.h"
#include "controlbus.h"
#include "alucontrolbus.h"
#include <QPushButton>
#include <QSignalMapper>
#include "registerinfowidget.h"
#include "instructionhelper.h"
#include "controlunit.h"

Diagram::Diagram(QWidget *parent) : QWidget(parent)
{

}

Diagram::Diagram(QWidget *parent, Register *RegsPtr, ArithLogUnit *ALUPtr, Memory *MemPtr, InDataBus *InBusPtr, class MemoryBus *MemAddBusPtr, class MemoryBus *MemDatBusPtr, ControlBus *CntrlBusPtr, AluControlBus *AluCntrlBusPtr, ControlUnit *CoUnitPtr) : QWidget(parent)
{
    Registers = RegsPtr;
    ALU = ALUPtr;
    Mem = MemPtr;
    InBus = InBusPtr;
    MemDataBus = MemDatBusPtr;
    MemAddressBus = MemAddBusPtr;
    CntrlBus = CntrlBusPtr;
    AluCntrlBus = AluCntrlBusPtr;
    CoUnit = CoUnitPtr;

    SignalMapper = new QSignalMapper(this);
    for(int i = 0; i < 16; i++)
    {
        RegisterButtons[i] = new QPushButton("",this);
        //Make invisible
        RegisterButtons[i]->setStyleSheet("background-color: rgba(255, 255, 255, 0);");
        RegisterButtons[i]->setAttribute(Qt::WA_TranslucentBackground);
        connect(RegisterButtons[i], SIGNAL(clicked()), SignalMapper, SLOT(map()));
        SignalMapper->setMapping(RegisterButtons[i], i);
    }
    connect(SignalMapper, SIGNAL(mapped(int)), this, SLOT(ShowRegInfoWindow(int)));
    connect(this, SIGNAL(UpdateDiagram()), this, SLOT(update()));

}

void Diagram::ShowRegInfoWindow(int Reg)
{
    RegisterInfoWidget *RegInfWidget = new RegisterInfoWidget(nullptr, Registers, Reg);
    RegInfWidget->setWindowFlag(Qt::WindowStaysOnTopHint, true);
    connect(this, SIGNAL(UpdateDiagram()), RegInfWidget, SLOT(Update()));
    connect(RegInfWidget, SIGNAL(RegValueUpdated()), this, SLOT(update()));
    RegInfWidget->show();
}

void Diagram::paintEvent(QPaintEvent *event)
{
    if(Registers == nullptr) return;

    static const QPoint Register[4] = {
        QPoint(0, 0),
        QPoint(100,0),
        QPoint(100,50),
        QPoint(0,50)
    };

    static const QPoint Box[4] = {
        QPoint(0, 0),
        QPoint(50,0),
        QPoint(50,50),
        QPoint(0,50)
    };


    static const QPoint GeneralRegBox[4] = {
        QPoint(0,0),
        QPoint(320,0),
        QPoint(320,320),
        QPoint(0,320)
    };

    static const QPoint ControlUnitBox[4] = {
        QPoint(0,0),
        QPoint(190,0),
        QPoint(190,180),
        QPoint(0,180)
    };

    static const QPoint ALUBox[4] = {
        QPoint(0,0),
        QPoint(100,0),
        QPoint(100,200),
        QPoint(0,200)
    };

    static const QPoint RAMBox[4] = {
        QPoint(0,0),
        QPoint(150,0),
        QPoint(150,150),
        QPoint(0,150)
    };


    QPen IdlePen;
    IdlePen.setWidth(3);
    IdlePen.setColor(Qt::black);
    QColor IdleBrush = Qt::white;

    QPen ReadPen;
    ReadPen.setWidth(3);
    ReadPen.setColor(Qt::blue);
    QColor ReadBrush = QColor(255,165,0, 127);

    QPen WritePen;
    WritePen.setWidth(3);
    WritePen.setColor(Qt::red);
    QColor WriteBrush = QColor(0,255,0, 127);

    QFont RegFont;
    RegFont.setPointSize(15);
    RegFont.setBold(true);

    QFont RegLabelFont;
    RegLabelFont.setPointSize(20);
    RegLabelFont.setBold(true);

    QFont LabelFont;
    LabelFont.setPointSize(15);
    LabelFont.setBold(true);

    //Bianry value for Use
    bool BinaryValue[24];


    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing, true);

    painter.setPen(IdlePen);
    painter.setBrush(IdleBrush);


    //Dimesions 2000 * 800
    int side = qMin((int)(width()/2.0f), (int)(height()/0.8f));
    painter.save();
    double scale = side / 1000.0;
    painter.scale(scale, scale);



    //Draw general purpose registers
    painter.save();
    painter.translate(550, 60);
    painter.drawConvexPolygon(GeneralRegBox, 4);
    painter.setFont(LabelFont);
    painter.drawText(0, 0, 320, 40, Qt::AlignCenter, "General Purpose Registers", nullptr);
    painter.translate(50, 40);
    //R0
    painter.setPen(IdlePen);
    painter.setFont(RegLabelFont);
    painter.drawText(-40, 5, 35, 40, Qt::AlignCenter, "R0", nullptr);
    if(Registers[0].GetState() == 0)
    {
        painter.setPen(IdlePen);
        painter.setBrush(IdleBrush);
    }
    else if(Registers[0].GetState() == 1)
    {
        painter.setPen(ReadPen);
        painter.setBrush(ReadBrush);
    }
    else if(Registers[0].GetState() == 2)
    {
        painter.setPen(WritePen);
        painter.setBrush(WriteBrush);
    }
    painter.drawConvexPolygon(Register, 4);
    Registers[0].GetValue(BinaryValue);
    painter.setFont(RegFont);
    painter.drawText(0, 0, 100, 50, Qt::AlignCenter, BinaryUtilities::ToStringHex24Bits(BinaryValue), nullptr);
    RegisterButtons[0]->resize((int)(100.0f * scale), (int)(50.0f * scale));
    RegisterButtons[0]->move((int)(600.0f * scale), (int)(100.0f * scale));
    painter.translate(150, 0);
    //Move button

    //R1
    painter.setPen(IdlePen);
    painter.setFont(RegLabelFont);
    painter.drawText(-40, 5, 35, 40, Qt::AlignCenter, "R1", nullptr);
    if(Registers[1].GetState() == 0)
    {
        painter.setPen(IdlePen);
        painter.setBrush(IdleBrush);
    }
    else if(Registers[1].GetState() == 1)
    {
        painter.setPen(ReadPen);
        painter.setBrush(ReadBrush);
    }
    else if(Registers[1].GetState() == 2)
    {
        painter.setPen(WritePen);
        painter.setBrush(WriteBrush);
    }
    painter.drawConvexPolygon(Register, 4);
    Registers[1].GetValue(BinaryValue);
    painter.setFont(RegFont);
    painter.drawText(0, 0, 100, 50, Qt::AlignCenter, BinaryUtilities::ToStringHex24Bits(BinaryValue), nullptr);
    RegisterButtons[1]->resize((int)(100.0f * scale), (int)(50.0f * scale));
    RegisterButtons[1] ->move((int)(750.0f * scale), (int)(100.0f * scale));
    painter.translate(-150, 70);
    //R2
    painter.setPen(IdlePen);
    painter.setFont(RegLabelFont);
    painter.drawText(-40, 5, 35, 40, Qt::AlignCenter, "R2", nullptr);
    if(Registers[2].GetState() == 0)
    {
        painter.setPen(IdlePen);
        painter.setBrush(IdleBrush);
    }
    else if(Registers[2].GetState() == 1)
    {
        painter.setPen(ReadPen);
        painter.setBrush(ReadBrush);
    }
    else if(Registers[2].GetState() == 2)
    {
        painter.setPen(WritePen);
        painter.setBrush(WriteBrush);
    }
    painter.drawConvexPolygon(Register, 4);
    Registers[2].GetValue(BinaryValue);
    painter.setFont(RegFont);
    painter.drawText(0, 0, 100, 50, Qt::AlignCenter, BinaryUtilities::ToStringHex24Bits(BinaryValue), nullptr);
    RegisterButtons[2]->resize((int)(100.0f * scale), (int)(60.0f * scale));
    RegisterButtons[2] ->move((int)(600.0f * scale), (int)(170.0f * scale));
    painter.translate(150, 0);
    //R3
    painter.setPen(IdlePen);
    painter.setFont(RegLabelFont);
    painter.drawText(-40, 5, 35, 40, Qt::AlignCenter, "R3", nullptr);
    if(Registers[3].GetState() == 0)
    {
        painter.setPen(IdlePen);
        painter.setBrush(IdleBrush);
    }
    else if(Registers[3].GetState() == 1)
    {
        painter.setPen(ReadPen);
        painter.setBrush(ReadBrush);
    }
    else if(Registers[3].GetState() == 2)
    {
        painter.setPen(WritePen);
        painter.setBrush(WriteBrush);
    }
    painter.drawConvexPolygon(Register, 4);
    Registers[3].GetValue(BinaryValue);
    painter.setFont(RegFont);
    painter.drawText(0, 0, 100, 50, Qt::AlignCenter, BinaryUtilities::ToStringHex24Bits(BinaryValue), nullptr);
    RegisterButtons[3]->resize((int)(100.0f * scale), (int)(50.0f * scale));
    RegisterButtons[3] ->move((int)(750.0f * scale), (int)(170.0f * scale));
    painter.translate(-150, 70);
    //R4
    painter.setPen(IdlePen);
    painter.setFont(RegLabelFont);
    painter.drawText(-40, 5, 35, 40, Qt::AlignCenter, "R4", nullptr);
    if(Registers[4].GetState() == 0)
    {
        painter.setPen(IdlePen);
        painter.setBrush(IdleBrush);
    }
    else if(Registers[4].GetState() == 1)
    {
        painter.setPen(ReadPen);
        painter.setBrush(ReadBrush);
    }
    else if(Registers[4].GetState() == 2)
    {
        painter.setPen(WritePen);
        painter.setBrush(WriteBrush);
    }
    painter.drawConvexPolygon(Register, 4);
    Registers[4].GetValue(BinaryValue);
    painter.setFont(RegFont);
    painter.drawText(0, 0, 100, 50, Qt::AlignCenter, BinaryUtilities::ToStringHex24Bits(BinaryValue), nullptr);
    RegisterButtons[4]->resize((int)(100.0f * scale), (int)(50.0f * scale));
    RegisterButtons[4] ->move((int)(600.0f * scale), (int)(240.0f * scale));
    painter.translate(150, 0);
    //R5
    painter.setPen(IdlePen);
    painter.setFont(RegLabelFont);
    painter.drawText(-40, 5, 35, 40, Qt::AlignCenter, "R5", nullptr);
    if(Registers[5].GetState() == 0)
    {
        painter.setPen(IdlePen);
        painter.setBrush(IdleBrush);
    }
    else if(Registers[5].GetState() == 1)
    {
        painter.setPen(ReadPen);
        painter.setBrush(ReadBrush);
    }
    else if(Registers[5].GetState() == 2)
    {
        painter.setPen(WritePen);
        painter.setBrush(WriteBrush);
    }
    painter.drawConvexPolygon(Register, 4);
    Registers[5].GetValue(BinaryValue);
    painter.setFont(RegFont);
    painter.drawText(0, 0, 100, 50, Qt::AlignCenter, BinaryUtilities::ToStringHex24Bits(BinaryValue), nullptr);
    RegisterButtons[5]->resize((int)(100.0f * scale), (int)(50.0f * scale));
    RegisterButtons[5] ->move((int)(750.0f * scale), (int)(240.0f * scale));
    painter.translate(-150, 70);
    //R6
    painter.setPen(IdlePen);
    painter.setFont(RegLabelFont);
    painter.drawText(-40, 5, 35, 40, Qt::AlignCenter, "R6", nullptr);
    if(Registers[6].GetState() == 0)
    {
        painter.setPen(IdlePen);
        painter.setBrush(IdleBrush);
    }
    else if(Registers[6].GetState() == 1)
    {
        painter.setPen(ReadPen);
        painter.setBrush(ReadBrush);
    }
    else if(Registers[6].GetState() == 2)
    {
        painter.setPen(WritePen);
        painter.setBrush(WriteBrush);
    }
    painter.drawConvexPolygon(Register, 4);
    Registers[6].GetValue(BinaryValue);
    painter.setFont(RegFont);
    painter.drawText(0, 0, 100, 50, Qt::AlignCenter, BinaryUtilities::ToStringHex24Bits(BinaryValue), nullptr);
    RegisterButtons[6]->resize((int)(100.0f * scale), (int)(50.0f * scale));
    RegisterButtons[6] ->move((int)(600.0f * scale), (int)(310.0f * scale));
    painter.translate(150, 0);
    //R7
    painter.setPen(IdlePen);
    painter.setFont(RegLabelFont);
    painter.drawText(-40, 5, 35, 40, Qt::AlignCenter, "R7", nullptr);
    if(Registers[7].GetState() == 0)
    {
        painter.setPen(IdlePen);
        painter.setBrush(IdleBrush);
    }
    else if(Registers[7].GetState() == 1)
    {
        painter.setPen(ReadPen);
        painter.setBrush(ReadBrush);
    }
    else if(Registers[7].GetState() == 2)
    {
        painter.setPen(WritePen);
        painter.setBrush(WriteBrush);
    }
    painter.drawConvexPolygon(Register, 4);
    Registers[7].GetValue(BinaryValue);
    painter.setFont(RegFont);
    painter.drawText(0, 0, 100, 50, Qt::AlignCenter, BinaryUtilities::ToStringHex24Bits(BinaryValue), nullptr);
    RegisterButtons[7]->resize((int)(100.0f * scale), (int)(50.0f * scale));
    RegisterButtons[7] ->move((int)(750.0f * scale), (int)(3100.0f * scale));
    painter.restore();

    //Draw Control Unit
    painter.save();
    painter.translate(280, 60);
    painter.drawConvexPolygon(ControlUnitBox, 4);
    painter.setFont(LabelFont);
    painter.drawText(0, 0, 190, 40, Qt::AlignCenter, "Control Unit", nullptr);
    painter.translate(70, 40);
    //CIR (9)
    painter.setPen(IdlePen);
    painter.setFont(RegLabelFont);
    painter.drawText(-60, 5, 55, 40, Qt::AlignCenter, "CIR", nullptr);
    if(Registers[9].GetState() == 0)
    {
        painter.setPen(IdlePen);
        painter.setBrush(IdleBrush);
    }
    else if(Registers[9].GetState() == 1)
    {
        painter.setPen(ReadPen);
        painter.setBrush(ReadBrush);
    }
    else if(Registers[9].GetState() == 2)
    {
        painter.setPen(WritePen);
        painter.setBrush(WriteBrush);
    }
    painter.drawConvexPolygon(Register, 4);
    Registers[9].GetValue(BinaryValue);
    painter.setFont(RegFont);
    painter.drawText(0, 0, 100, 50, Qt::AlignCenter, BinaryUtilities::ToStringHex24Bits(BinaryValue), nullptr);
    RegisterButtons[9]->resize((int)(100.0f * scale), (int)(50.0f * scale));
    RegisterButtons[9] ->move((int)(350.0f * scale), (int)(100.0f * scale));
    painter.translate(0, 70);
    //PC (8)
    painter.setPen(IdlePen);
    painter.setFont(RegLabelFont);
    painter.drawText(-60, 5, 55, 40, Qt::AlignCenter, "PC", nullptr);
    if(Registers[8].GetState() == 0)
    {
        painter.setPen(IdlePen);
        painter.setBrush(IdleBrush);
    }
    else if(Registers[8].GetState() == 1)
    {
        painter.setPen(ReadPen);
        painter.setBrush(ReadBrush);
    }
    else if(Registers[8].GetState() == 2)
    {
        painter.setPen(WritePen);
        painter.setBrush(WriteBrush);
    }
    painter.drawConvexPolygon(Register, 4);
    Registers[8].GetValue(BinaryValue);
    painter.setFont(RegFont);
    painter.drawText(0, 0, 100, 50, Qt::AlignCenter, BinaryUtilities::ToStringHex24Bits(BinaryValue), nullptr);
    RegisterButtons[8]->resize((int)(100.0f * scale), (int)(50.0f * scale));
    RegisterButtons[8] ->move((int)(350.0f * scale), (int)(170.0f * scale));
    painter.restore();

    //Draw OP1 and OP2
    painter.save();
    painter.translate(130, 425);
    //OP1 (12)
    painter.setPen(IdlePen);
    painter.setFont(RegLabelFont);
    painter.drawText(0, -40, 100, 45, Qt::AlignCenter, "OP1", nullptr);
    if(Registers[12].GetState() == 0)
    {
        painter.setPen(IdlePen);
        painter.setBrush(IdleBrush);
    }
    else if(Registers[12].GetState() == 1)
    {
        painter.setPen(ReadPen);
        painter.setBrush(ReadBrush);
    }
    else if(Registers[12].GetState() == 2)
    {
        painter.setPen(WritePen);
        painter.setBrush(WriteBrush);
    }
    painter.drawConvexPolygon(Register, 4);
    Registers[12].GetValue(BinaryValue);
    painter.setFont(RegFont);
    painter.drawText(0, 0, 100, 50, Qt::AlignCenter, BinaryUtilities::ToStringHex24Bits(BinaryValue), nullptr);
    RegisterButtons[12]->resize((int)(100.0f * scale), (int)(50.0f * scale));
    RegisterButtons[12] ->move((int)(130.0f * scale), (int)(425.0f * scale));
    painter.translate(0, 100);
    //OP2 (13)
    painter.setPen(IdlePen);
    painter.setFont(RegLabelFont);
    painter.drawText(0, -40, 100, 45, Qt::AlignCenter, "OP2", nullptr);
    if(Registers[13].GetState() == 0)
    {
        painter.setPen(IdlePen);
        painter.setBrush(IdleBrush);
    }
    else if(Registers[13].GetState() == 1)
    {
        painter.setPen(ReadPen);
        painter.setBrush(ReadBrush);
    }
    else if(Registers[13].GetState() == 2)
    {
        painter.setPen(WritePen);
        painter.setBrush(WriteBrush);
    }
    painter.drawConvexPolygon(Register, 4);
    Registers[13].GetValue(BinaryValue);
    painter.setFont(RegFont);
    painter.drawText(0, 0, 100, 50, Qt::AlignCenter, BinaryUtilities::ToStringHex24Bits(BinaryValue), nullptr);
    RegisterButtons[13]->resize((int)(100.0f * scale), (int)(50.0f * scale));
    RegisterButtons[13] ->move((int)(130.0f * scale), (int)(525.0f * scale));
    painter.restore();

    //Draw RESULT
    painter.save();
    painter.translate(280, 650);
    //RESULT (14)
    painter.setPen(IdlePen);
    painter.setFont(RegLabelFont);
    painter.drawText(-110, 5, 105, 40, Qt::AlignCenter, "RESULT", nullptr);
    if(Registers[14].GetState() == 0)
    {
        painter.setPen(IdlePen);
        painter.setBrush(IdleBrush);
    }
    else if(Registers[14].GetState() == 1)
    {
        painter.setPen(ReadPen);
        painter.setBrush(ReadBrush);
    }
    else if(Registers[14].GetState() == 2)
    {
        painter.setPen(WritePen);
        painter.setBrush(WriteBrush);
    }
    painter.drawConvexPolygon(Register, 4);
    Registers[14].GetValue(BinaryValue);
    painter.setFont(RegFont);
    painter.drawText(0, 0, 100, 50, Qt::AlignCenter, BinaryUtilities::ToStringHex24Bits(BinaryValue), nullptr);
    painter.translate(0, 100);
    RegisterButtons[14]->resize((int)(100.0f * scale), (int)(50.0f * scale));
    RegisterButtons[14] ->move((int)(280.0f * scale), (int)(650.0f * scale));
    painter.restore();

    //Draw RESULT
    painter.save();
    painter.translate(430, 475);
    //RESULT (15)
    painter.setPen(IdlePen);
    painter.setFont(RegLabelFont);
    painter.drawText(0, -40, 100, 45, Qt::AlignCenter, "FLAG", nullptr);
    if(Registers[15].GetState() == 0)
    {
        painter.setPen(IdlePen);
        painter.setBrush(IdleBrush);
    }
    else if(Registers[15].GetState() == 1)
    {
        painter.setPen(ReadPen);
        painter.setBrush(ReadBrush);
    }
    else if(Registers[15].GetState() == 2)
    {
        painter.setPen(WritePen);
        painter.setBrush(WriteBrush);
    }
    painter.drawConvexPolygon(Register, 4);
    Registers[15].GetValue(BinaryValue);
    painter.setFont(RegFont);
    painter.drawText(0, 0, 100, 50, Qt::AlignCenter, BinaryUtilities::ToStringHex24Bits(BinaryValue), nullptr);
    painter.translate(0, 100);
    RegisterButtons[15]->resize((int)(100.0f * scale), (int)(50.0f * scale));
    RegisterButtons[15] ->move((int)(430.0f * scale), (int)(475.0f * scale));
    painter.restore();

    //Draw MAR and MBR
    painter.save();
    painter.translate(1020, 100);
    //MAR (10)
    painter.setPen(IdlePen);
    painter.setFont(RegLabelFont);
    painter.drawText(0, -40, 100, 45, Qt::AlignCenter, "MAR", nullptr);
    if(Registers[10].GetState() == 0)
    {
        painter.setPen(IdlePen);
        painter.setBrush(IdleBrush);
    }
    else if(Registers[10].GetState() == 1)
    {
        painter.setPen(ReadPen);
        painter.setBrush(ReadBrush);
    }
    else if(Registers[10].GetState() == 2)
    {
        painter.setPen(WritePen);
        painter.setBrush(WriteBrush);
    }
    painter.drawConvexPolygon(Register, 4);
    Registers[10].GetValue(BinaryValue);
    painter.setFont(RegFont);
    painter.drawText(0, 0, 100, 50, Qt::AlignCenter, BinaryUtilities::ToStringHex24Bits(BinaryValue), nullptr);
    RegisterButtons[10]->resize((int)(100.0f * scale), (int)(50.0f * scale));
    RegisterButtons[10] ->move((int)(1020.0f * scale), (int)(100.0f * scale));
    painter.translate(0, 100);
    //MBR (11)
    painter.setPen(IdlePen);
    painter.setFont(RegLabelFont);
    painter.drawText(0, -40, 100, 45, Qt::AlignCenter, "MBR", nullptr);
    if(Registers[11].GetState() == 0)
    {
        painter.setPen(IdlePen);
        painter.setBrush(IdleBrush);
    }
    else if(Registers[11].GetState() == 1)
    {
        painter.setPen(ReadPen);
        painter.setBrush(ReadBrush);
    }
    else if(Registers[11].GetState() == 2)
    {
        painter.setPen(WritePen);
        painter.setBrush(WriteBrush);
    }
    painter.drawConvexPolygon(Register, 4);
    Registers[11].GetValue(BinaryValue);
    painter.setFont(RegFont);
    painter.drawText(0, 0, 100, 50, Qt::AlignCenter, BinaryUtilities::ToStringHex24Bits(BinaryValue), nullptr);
    RegisterButtons[11]->resize((int)(100.0f * scale), (int)(50.0f * scale));
    RegisterButtons[11] ->move((int)(1020.0f * scale), (int)(200.0f * scale));
    painter.restore();


    //Draw ALU
    painter.save();
    painter.translate(280,400);
    if(ALU->GetState() == 0)
    {
        painter.setPen(IdlePen);
        painter.setBrush(IdleBrush);
    }
    else if(ALU->GetState() == 1)
    {
        painter.setPen(WritePen);
        painter.setBrush(WriteBrush);
    }
    painter.drawConvexPolygon(ALUBox,4);
    painter.setFont(RegLabelFont);
    painter.drawText(0, 0, 100, 200, Qt::AlignCenter, "ALU", nullptr);
    painter.restore();

    //Draw RAM
    painter.save();
    painter.translate(1170,100);
    if(Mem->GetState() == 0)
    {
        painter.setPen(IdlePen);
        painter.setBrush(IdleBrush);
    }
    else if(Mem->GetState() == 1)
    {
        painter.setPen(ReadPen);
        painter.setBrush(ReadBrush);
    }
    else if(Mem->GetState() == 2)
    {
        painter.setPen(WritePen);
        painter.setBrush(WriteBrush);
    }
    painter.drawConvexPolygon(RAMBox,4);
    painter.setFont(RegLabelFont);
    painter.drawText(0, 0, 150, 150, Qt::AlignCenter, "RAM", nullptr);
    painter.restore();

    //Draw Lines
    QPen LineIdlePen;
    LineIdlePen.setWidth(5);
    LineIdlePen.setColor(Qt::black);

    QPen LineWritePen;
    LineWritePen.setWidth(10);
    LineWritePen.setColor(Qt::blue);

    painter.save();
    //Draw stretch between FLAG and RESULT
    const QLine FlagToRegOne = QLine(480, 525, 480, 750);
    const QLine FlagToRegTwo = QLine(480, 750, 330, 750);
    painter.setPen(LineIdlePen);
    if(InBus->GetState() == 1)
    {
        //Flag is 15
        if(InBus->GetSource() == 15 ||  InBus->GetDestination() == 15)
        {
            painter.setPen(LineWritePen);
        }
    }
    painter.drawLine(FlagToRegOne);
    painter.drawLine(FlagToRegTwo);

    painter.setPen(LineIdlePen);
    const QLine ResultLine = QLine(330, 750, 330, 700);
    if(InBus->GetState() == 1)
    {
        //Result is 14
        if(InBus->GetSource() == 14 ||  InBus->GetDestination() == 14)
        {
            painter.setPen(LineWritePen);
        }
    }
    painter.drawLine(ResultLine);

    //Draw stretch between RESULT and OP2
    const QLine ResToOP2One = QLine(330, 750, 80, 750);
    const QLine ResToOP2Two = QLine(80, 750, 80, 550);
    painter.setPen(LineIdlePen);
    if(InBus->GetState() == 1)
    {
        //Result is 14
        if((InBus->GetSource() >= 14 && InBus->GetSource() != 16 && InBus->GetDestination() < 14) ||  (InBus->GetDestination() >= 14 && InBus->GetDestination() != 16  && InBus->GetSource() < 14)
           || (InBus->GetSource() >= 14 && InBus->GetSource() != 16  && InBus->GetDestination() == 16) ||  (InBus->GetDestination() >= 14 && InBus->GetDestination() != 16 && InBus->GetSource() == 16))
        {
            painter.setPen(LineWritePen);
        }
    }
    painter.drawLine(ResToOP2One);
    painter.drawLine(ResToOP2Two);

    painter.setPen(LineIdlePen);
    const QLine OP2Line = QLine(80, 550, 130, 550);
    if(InBus->GetState() == 1)
    {
        //OP2 is 13
        if(InBus->GetSource() == 13 ||  InBus->GetDestination() == 13)
        {
            painter.setPen(LineWritePen);
        }
    }
    painter.drawLine(OP2Line);

    //Draw stretch between OP2 and OP1
    const QLine OP2ToOP1 = QLine(80, 550, 80, 450);
    painter.setPen(LineIdlePen);
    if(InBus->GetState() == 1)
    {
        //OP2 is 13
        if((InBus->GetSource() >= 13  && InBus->GetSource() != 16 && InBus->GetDestination() < 13) ||  (InBus->GetDestination() >= 13 && InBus->GetDestination() != 16 && InBus->GetSource() < 13)
             || (InBus->GetSource() >= 13  && InBus->GetSource() != 16 && InBus->GetDestination() == 16) ||  (InBus->GetDestination() >= 13 && InBus->GetDestination() != 16 && InBus->GetSource() == 16) )
        {
            painter.setPen(LineWritePen);
        }
    }
    painter.drawLine(OP2ToOP1);

    painter.setPen(LineIdlePen);
    const QLine OP1Line = QLine(80, 450, 130, 450);
    if(InBus->GetState() == 1)
    {
        //OP1 is 12
        if(InBus->GetSource() == 12 ||  InBus->GetDestination() == 12)
        {
            painter.setPen(LineWritePen);
        }
    }
    painter.drawLine(OP1Line);

    //Directory for easier identifying of olaction of Registers
    //GPR - General Purpose Registers
    //CU COntrol Unit
    const QString RegisterLocations[17] = {"GPR","GPR", "GPR" , "GPR", "GPR", "GPR", "GPR", "GPR",
                                          "CU", "CU", "MAR", "MBR", "OP1" "OP2", "RESULT", "Flag", "CU"};

    //Draw strech between OP1 and Control Unit
    //Draw stretch between FLAG and RESULT
    const QLine OP1ToCUOne = QLine(80, 450, 80, 10);
    const QLine OP1ToCUTwo = QLine(80, 10, 375, 10);
    painter.setPen(LineIdlePen);
    if(InBus->GetState() == 1)
    {
        //Flag is 15
        if((RegisterLocations[InBus->GetSource()] == "CU" && InBus->GetDestination() >= 12)
                || (RegisterLocations[InBus->GetDestination()] == "CU" && InBus->GetSource() >= 12 && InBus->GetSource() != 16)
               || (RegisterLocations[InBus->GetSource()] == "GPR" && InBus->GetDestination() >= 12 && InBus->GetDestination() != 16)
               || (RegisterLocations[InBus->GetDestination()] == "GPR" && InBus->GetSource() >= 12 && InBus->GetSource() != 16)
            || ((RegisterLocations[InBus->GetSource()] == "MAR" || RegisterLocations[InBus->GetSource()] == "MBR") && InBus->GetDestination() >= 12 && InBus->GetDestination() != 16)
               || ((RegisterLocations[InBus->GetDestination()] == "MAR" || RegisterLocations[InBus->GetDestination()] == "MBR") && InBus->GetSource() >= 12 && InBus->GetSource() != 16))
        {
            painter.setPen(LineWritePen);
        }
    }
    painter.drawLine(OP1ToCUOne);
    painter.drawLine(OP1ToCUTwo);

    painter.setPen(LineIdlePen);
    const QLine CULine = QLine(375, 10, 375, 60);
    if(InBus->GetState() == 1)
    {
        //
        if((RegisterLocations[InBus->GetSource()] == "CU" &&  RegisterLocations[InBus->GetDestination()] != "CU") ||  (RegisterLocations[InBus->GetDestination()] == "CU" && RegisterLocations[InBus->GetSource()] != "CU"))
        {
            painter.setPen(LineWritePen);
        }
    }
    painter.drawLine(CULine);

    //Draw stretch between control unit and general purpose registers
    const QLine CUToGPROne = QLine(375, 10, 710, 10);
    painter.setPen(LineIdlePen);
    if(InBus->GetState() == 1)
    {
        //Flag is 15
        if((RegisterLocations[InBus->GetSource()] == "GPR" && InBus->GetDestination() >= 12 && InBus->GetDestination() != 16)
               || (RegisterLocations[InBus->GetDestination()] == "GPR" && InBus->GetSource() >= 12 && InBus->GetSource() != 16)
               || (RegisterLocations[InBus->GetSource()] == "GPR" && RegisterLocations[InBus->GetDestination()] == "CU")
               || (RegisterLocations[InBus->GetDestination()] == "GPR" && RegisterLocations[InBus->GetSource()] == "CU")
               || (RegisterLocations[InBus->GetSource()] == "MAR" && RegisterLocations[InBus->GetDestination()] == "CU")
               || (RegisterLocations[InBus->GetDestination()] == "MAR" && RegisterLocations[InBus->GetSource()] == "CU")
               || (RegisterLocations[InBus->GetSource()] == "MBR" && RegisterLocations[InBus->GetDestination()] == "CU")
               || (RegisterLocations[InBus->GetDestination()] == "MBR" && RegisterLocations[InBus->GetSource()] == "CU")
               || ((RegisterLocations[InBus->GetSource()] == "MAR" || RegisterLocations[InBus->GetSource()] == "MBR") && InBus->GetDestination() >= 12 && InBus->GetDestination() != 16)
               || ((RegisterLocations[InBus->GetDestination()] == "MAR" || RegisterLocations[InBus->GetDestination()] == "MBR") && InBus->GetSource() >= 12 && InBus->GetSource() != 16))
        {
            painter.setPen(LineWritePen);
        }
    }
    painter.drawLine(CUToGPROne);

    painter.setPen(LineIdlePen);
    const QLine GPRLine = QLine(710, 10, 710, 60);
    if(InBus->GetState() == 1)
    {
        //If one of source or destination is a general purpose register
        if((RegisterLocations[InBus->GetSource()] == "GPR" && RegisterLocations[InBus->GetDestination()] != "GPR")
                ||  (RegisterLocations[InBus->GetDestination()] == "GPR" && RegisterLocations[InBus->GetSource()] != "GPR"))
        {
            painter.setPen(LineWritePen);
        }
    }
    painter.drawLine(GPRLine);

    //Draw strech between general purpose registers and  MAR
    const QLine GPRToMAROne = QLine(710, 10, 970, 10);
    const QLine GPRToMARTwo = QLine(970, 10, 970, 125);
    painter.setPen(LineIdlePen);
    if(InBus->GetState() == 1)
    {
        //
        if((RegisterLocations[InBus->GetSource()] == "MAR" && RegisterLocations[InBus->GetDestination()] != "MBR")
            || (RegisterLocations[InBus->GetDestination()] == "MAR" && RegisterLocations[InBus->GetSource()] != "MBR")
            || (RegisterLocations[InBus->GetSource()] == "MBR" && RegisterLocations[InBus->GetDestination()] != "MAR")
            || (RegisterLocations[InBus->GetDestination()] == "MBR" && RegisterLocations[InBus->GetSource()] != "MAR"))
        {
            painter.setPen(LineWritePen);
        }
    }
    painter.drawLine(GPRToMAROne);
    painter.drawLine(GPRToMARTwo);

    painter.setPen(LineIdlePen);
    const QLine MARLine = QLine(970, 125, 1020, 125);
    if(InBus->GetState() == 1)
    {
        //
        if(RegisterLocations[InBus->GetSource()] == "MAR" ||  RegisterLocations[InBus->GetDestination()] == "MAR")
        {
            painter.setPen(LineWritePen);
        }
    }
    painter.drawLine(MARLine);

    //Draw stretch to MBR
    const QLine MBROne = QLine(970, 125, 970, 225);
    const QLine MBRTwo = QLine(970, 225, 1020, 225);
    painter.setPen(LineIdlePen);
    if(InBus->GetState() == 1)
    {
        //
        if(RegisterLocations[InBus->GetSource()] == "MBR" ||  RegisterLocations[InBus->GetDestination()] == "MBR")
        {
            painter.setPen(LineWritePen);
        }
    }
    painter.drawLine(MBROne);
    painter.drawLine(MBRTwo);

    //Draw label
    painter.setPen(LineIdlePen);
    painter.setFont(LabelFont);
    painter.drawText(90, 10, 190, 40, Qt::AlignCenter, "Internal Data Bus", nullptr);

    //Draw Memory Buses
    painter.setPen(LineIdlePen);
    const QLine MemAddBusLine = QLine(1120, 125, 1170, 125);
    if(MemAddressBus->GetState() == 1)
    {
        painter.setPen(LineWritePen);
    }
    painter.drawLine(MemAddBusLine);

    //Draw Memory Buses
    painter.setPen(LineIdlePen);
    const QLine MemDatBusLine = QLine(1120, 225, 1170, 225);
    if(MemDataBus->GetState() == 1 || MemDataBus->GetState() == 2)
    {
        painter.setPen(LineWritePen);
    }
    painter.drawLine(MemDatBusLine);

    painter.setPen(LineIdlePen);
    painter.setFont(LabelFont);
    painter.rotate(90);
    painter.drawText(-20, -1165, 190, 40, Qt::AlignCenter, "Address\nBus", nullptr);
    painter.drawText(160, -1165, 190, 40, Qt::AlignCenter, "Data\nBus", nullptr);
    painter.rotate(-90);

    //Draw lines between ALU and Registers
    //OP1
    painter.setPen(LineIdlePen);
    const QLine OP1ToALU = QLine(230, 450, 280, 450);
    if(ALU->GetState() == 1)
    {
        //OP1 is 12//If read from
        if(Registers[12].GetState() == 1)
        {
            painter.setPen(LineWritePen);
        }
    }
    painter.drawLine(OP1ToALU);

    //OP2
    painter.setPen(LineIdlePen);
    const QLine OP2ToALU = QLine(230, 550, 280, 550);
    if(ALU->GetState() == 1)
    {
        //OP2 is 13//If read from
        if(Registers[13].GetState() == 1)
        {
            painter.setPen(LineWritePen);
        }
    }
    painter.drawLine(OP2ToALU);

    //RESULT
    painter.setPen(LineIdlePen);
    const QLine RSTToALU = QLine(330, 600, 330, 650);
    if(ALU->GetState() == 1)
    {
        //RESULT is 14//If written to
        if(Registers[14].GetState() == 2)
        {
            painter.setPen(LineWritePen);
        }
    }
    painter.drawLine(RSTToALU);

    //FlAG
    painter.setPen(LineIdlePen);
    const QLine FLGToALU = QLine(380, 500, 430, 500);
    if(ALU->GetState() == 1)
    {
        //FLAG is 15//If written to
        if(Registers[15].GetState() == 2)
        {
            painter.setPen(LineWritePen);
        }
    }
    painter.drawLine(FLGToALU);

    //Draw ControlBus
    QPen CntrlBusIdlePen;
    CntrlBusIdlePen.setWidth(5);
    CntrlBusIdlePen.setColor(Qt::black);

    QPen CntrlBusReadPen;
    CntrlBusReadPen.setWidth(10);
    CntrlBusReadPen.setColor(Qt::blue);

    QPen CntrlBusWritePen;
    CntrlBusWritePen.setWidth(10);
    CntrlBusWritePen.setColor(Qt::red);



    const QLine CntrlBusLines[4] = {QLine(470,150, 520, 150),
                                   QLine(520, 150, 520, 420),
                                   QLine(520, 420, 1245, 420),
                                   QLine(1245, 420, 1245, 250)};
    painter.setPen(CntrlBusIdlePen);
    //Read
    if(CntrlBus->GetState() == 1)
    {
        painter.setPen(CntrlBusReadPen);
    }
    //Write
    else if(CntrlBus->GetState() == 2)
    {
        painter.setPen(CntrlBusWritePen);
    }
    painter.drawLines(CntrlBusLines,4);

    //Draw label
    painter.setPen(LineIdlePen);
    painter.setFont(LabelFont);
    painter.drawText(600, 420, 190, 40, Qt::AlignCenter, "Control Bus", nullptr);

    //Draw Alu control bus
    const QLine AluCntrlBusLine = QLine(330, 240,330, 400);
    painter.setPen(LineIdlePen);
    if(AluCntrlBus->GetState() == 1)
    {
        painter.setPen(LineWritePen);
    }
    painter.drawLine(AluCntrlBusLine);

    //Draw label
    painter.setPen(LineIdlePen);
    painter.setFont(LabelFont);
    painter.drawText(330, 300, 140, 40, Qt::AlignCenter, "ALU Control\nBus", nullptr);

    //Draw information
    QFont InstrFont;
    InstrFont.setPointSize(15);
    InstrFont.setBold(true);

    bool CurInstruction[24];
    //CIR (9)
    Registers[9].GetValue(CurInstruction);
    QString InstructionStr = InstructionHelper::BinaryToInstruction(CurInstruction);
    QString InstructionDesc = InstructionHelper::InstructionDescription(CurInstruction);
    QString StepDesc = InstructionHelper::StepDescription(InBus, Mem,MemAddressBus, MemDataBus, AluCntrlBus, CntrlBus, Registers, CurInstruction);

    painter.setFont(InstrFont);

    if(CoUnit->GetState() == 1)
    {
        painter.drawText(600, 500, 500, 40, Qt::AlignLeft, "Fetching", nullptr);
        painter.drawText(600, 520, 500, 40, Qt::AlignLeft, "Current step description:", nullptr);
        painter.drawText(600, 540, 850, 200, Qt::AlignLeft | Qt::TextWordWrap, StepDesc, nullptr);
    }
    else if(CoUnit->GetState() == 2)
    {
        painter.drawText(600, 500, 500, 40, Qt::AlignLeft, "Executing", nullptr);
        painter.drawText(600, 520, 500, 40, Qt::AlignLeft, "Current instruction:", nullptr);
        painter.drawText(600, 540, 300, 40, Qt::AlignLeft, InstructionStr, nullptr);
        painter.drawText(600, 560, 500, 40, Qt::AlignLeft, "Instruction description:", nullptr);
        painter.drawText(600, 580, 750, 200, Qt::AlignLeft | Qt::TextWordWrap, InstructionDesc, nullptr);
        painter.drawText(600, 620, 500, 40, Qt::AlignLeft, "Current step description:", nullptr);
        painter.drawText(600, 640, 750, 200, Qt::AlignLeft | Qt::TextWordWrap, StepDesc, nullptr);
    }
    else if(CoUnit->GetState() == 3)
    {
        painter.drawText(600, 500, 500, 100, Qt::AlignLeft | Qt::TextWordWrap, "Ready to start program, to run either press 'Play' or 'Step', press on registers to edit their values.", nullptr);
    }
    else if(CoUnit->GetState() == 0){
        painter.drawText(600, 500, 500, 40, Qt::AlignLeft, "Halted", nullptr);
        painter.drawText(600, 520, 700, 100, Qt::AlignLeft  | Qt::TextWordWrap, "To run a program assemble one from the Assembler window, either loading a program through the 'File' menu or writing a new one.", nullptr);
    }

    //Draw key
    QFont KeyFont;
    KeyFont.setPointSize(15);
    KeyFont.setBold(true);

    painter.setFont(KeyFont);
    painter.drawText(1400, 20, 500, 40, Qt::AlignLeft, "Key", nullptr);
    painter.drawText(1400, 70, 500, 40, Qt::AlignLeft, "Registers", nullptr);

    painter.save();

    painter.translate(1400, 100);
    painter.setPen(IdlePen);
    painter.setBrush(IdleBrush);
    painter.drawConvexPolygon(Register, 4);
    painter.drawText(0, 0, 100, 50, Qt::AlignCenter, "000000", nullptr);

    painter.translate(0, 70);
    painter.setPen(ReadPen);
    painter.setBrush(ReadBrush);
    painter.drawConvexPolygon(Register, 4);
    painter.drawText(0, 0, 100, 50, Qt::AlignCenter, "000000", nullptr);

    painter.translate(0, 70);
    painter.setPen(WritePen);
    painter.setBrush(WriteBrush);
    painter.drawConvexPolygon(Register, 4);
    painter.drawText(0, 0, 100, 50, Qt::AlignCenter, "000000", nullptr);

    painter.restore();

    painter.drawText(1500, 100, 150, 50, Qt::AlignCenter, "Idle", nullptr);
    painter.drawText(1500, 170, 150, 50, Qt::AlignCenter, "Read from", nullptr);
    painter.drawText(1500, 240, 150, 50, Qt::AlignCenter, "Written to", nullptr);

    painter.drawText(1400, 320, 500, 40, Qt::AlignLeft, "ControlBus", nullptr);

    const QLine KeyLine = QLine(0,0,100,0);

    painter.save();
    painter.setPen(CntrlBusIdlePen);
    painter.translate(1400,370);
    painter.drawLine(KeyLine);

    painter.setPen(CntrlBusReadPen);
    painter.translate(0,30);
    painter.drawLine(KeyLine);

    painter.setPen(CntrlBusWritePen);
    painter.translate(0,30);
    painter.drawLine(KeyLine);
    painter.restore();

    painter.drawText(1500, 355, 150, 30, Qt::AlignCenter, "Idle", nullptr);
    painter.drawText(1500, 385, 150, 30, Qt::AlignCenter, "Read signal", nullptr);
    painter.drawText(1500, 415, 150, 30, Qt::AlignCenter, "Write signal", nullptr);

    painter.drawText(1400, 450, 500, 40, Qt::AlignLeft, "RAM", nullptr);

    painter.save();
    painter.translate(1425, 480);
    painter.setPen(IdlePen);
    painter.setBrush(IdleBrush);
    painter.drawConvexPolygon(Box, 4);

    painter.translate(0, 70);
    painter.setPen(ReadPen);
    painter.setBrush(ReadBrush);
    painter.drawConvexPolygon(Box, 4);

    painter.translate(0, 70);
    painter.setPen(WritePen);
    painter.setBrush(WriteBrush);
    painter.drawConvexPolygon(Box, 4);
    painter.restore();

    painter.drawText(1500, 480, 150, 50, Qt::AlignCenter, "Idle", nullptr);
    painter.drawText(1500, 550, 150, 50, Qt::AlignCenter, "Reading from", nullptr);
    painter.drawText(1500, 620, 150, 50, Qt::AlignCenter, "Writing to", nullptr);

    painter.drawText(1700, 70, 500, 40, Qt::AlignLeft, "ALU", nullptr);

    painter.save();
    painter.translate(1725, 100);
    painter.setPen(IdlePen);
    painter.setBrush(IdleBrush);
    painter.drawConvexPolygon(Box, 4);

    painter.translate(0, 70);
    painter.setPen(ReadPen);
    painter.setBrush(ReadBrush);
    painter.drawConvexPolygon(Box, 4);

    painter.restore();

    painter.drawText(1800, 100, 150, 50, Qt::AlignCenter, "Idle", nullptr);
    painter.drawText(1800, 170, 150, 50, Qt::AlignCenter, "Active", nullptr);

    painter.drawText(1700, 240, 500, 40, Qt::AlignLeft, "Internal Data Bus", nullptr);

    painter.save();
    painter.setPen(CntrlBusIdlePen);
    painter.translate(1700,290);
    painter.drawLine(KeyLine);

    painter.setPen(CntrlBusReadPen);
    painter.translate(0,30);
    painter.drawLine(KeyLine);

    painter.restore();

    painter.drawText(1800, 265, 150, 50, Qt::AlignCenter, "Idle", nullptr);
    painter.drawText(1800, 295, 150, 50, Qt::AlignCenter, "Writing", nullptr);
}


