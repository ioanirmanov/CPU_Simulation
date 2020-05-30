#include "memoryinfowindow.h"
#include "memory.h"
#include "binaryutilities.h"
#include "instructionhelper.h"
#include <QLabel>
#include <QBoxLayout>

MemoryInfoWindow::MemoryInfoWindow(QWidget *parent) : QWidget(parent)
{

}

MemoryInfoWindow::MemoryInfoWindow(QWidget *parent, Memory *MemPtr, int Loc) : QWidget(parent)
{
    MemoryPtr = MemPtr;
    if(Loc >= 0 && Loc < 256) MemoryLocation = Loc;
    else close();

    QBoxLayout *Layout = new QBoxLayout(QBoxLayout::Down, this);

    Label = new QLabel(this);
    QFont Font;
    Font.setPixelSize(15);
    Label->setFont(Font);
    Layout->addWidget(Label);

    setLayout(Layout);

    GenerateLabelText();

    this->setWindowTitle("Memory Location Information");
}

void MemoryInfoWindow::Update()
{
    GenerateLabelText();
}

void MemoryInfoWindow::GenerateLabelText()
{
    QString LabelText = "Memory location: ";
    LabelText.append(QString::number(MemoryLocation));
    bool ValueBin[24];
    MemoryPtr->ReadLocation(MemoryLocation, ValueBin);
    QString ValueHex = BinaryUtilities::ToStringHex24Bits(ValueBin);
    QString ValueBinStr = BinaryUtilities::ToString24BitSpaced(ValueBin);
    int ValueDec = BinaryUtilities::BinToDec(ValueBin, 24);
    int ValueDecUnsigned = BinaryUtilities::BinToDecUnsigned(ValueBin, 24);

    LabelText.append("\nBinary value: " + ValueBinStr);
    LabelText.append("\nHex value: " + ValueHex);
    LabelText.append("\nDecimal value (signed): " + QString::number(ValueDec));
    LabelText.append("\nDecimal value (unsigned): " + QString::number(ValueDecUnsigned));

    QString AsInstruction = InstructionHelper::BinaryToInstruction(ValueBin);
    LabelText.append("\nAs instruction mneumonic: " + AsInstruction);

    Label->setText(LabelText);
}
