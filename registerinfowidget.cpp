#include "registerinfowidget.h"
#include "register.h"
#include "binaryutilities.h"
#include "instructionhelper.h"
#include <QLabel>
#include <QPushButton>
#include <QBoxLayout>
#include <QTextEdit>
#include <QRadioButton>
#include <QGroupBox>
#include <QMessageBox>

RegisterInfoWidget::RegisterInfoWidget(QWidget *parent) : QWidget(parent)
{

}

RegisterInfoWidget::RegisterInfoWidget(QWidget *parent, class Register *RegsPtr, int RegIndex) : QWidget(parent)
{
    RegistersPtr = RegsPtr;
    if(RegIndex >= 0 && RegIndex < 16) RegisterIndex = RegIndex;
    else close();

    QBoxLayout *Layout = new QBoxLayout(QBoxLayout::Down, this);

    Label = new QLabel(this);
    QFont Font;
    Font.setPixelSize(15);
    Label->setFont(Font);
    Layout->addWidget(Label);

    Button = new QPushButton("Edit Value", this);
    Layout->addWidget(Button);
    connect(Button, SIGNAL(clicked()), this, SLOT(ShowValueEditWidget()));

    setLayout(Layout);

    GenerateLabelText();

    this->setWindowTitle("Register Information");
}

void RegisterInfoWidget::Update()
{
    GenerateLabelText();
}

void RegisterInfoWidget::GenerateLabelText()
{
    QString LabelText = "Register: ";
    LabelText.append(RegisterNames[RegisterIndex]);
    bool ValueBin[24];
    RegistersPtr[RegisterIndex].GetValue(ValueBin);
    QString ValueHex = BinaryUtilities::ToStringHex24Bits(ValueBin);
    QString ValueBinStr = BinaryUtilities::ToString24BitSpaced(ValueBin);
    int ValueDec = BinaryUtilities::BinToDec(ValueBin, 24);
    int ValueDecUnsigned = BinaryUtilities::BinToDecUnsigned(ValueBin, 24);

    LabelText.append("\nBinary value: " + ValueBinStr);
    LabelText.append("\nHex value: " + ValueHex);
    LabelText.append("\nDecimal value (signed): " + QString::number(ValueDec));
    LabelText.append("\nDecimal value (unsigned): " + QString::number(ValueDecUnsigned));

    //if CIR
    if(RegisterIndex == 9)
    {
        QString AsInstruction = InstructionHelper::BinaryToInstruction(ValueBin);
        LabelText.append("\nAs instruction mneumonic: " + AsInstruction);
    }

    //if FLAG
    if(RegisterIndex == 15)
    {
        int Flag = ValueDecUnsigned;
        if(Flag == 1) LabelText.append("\nBranch flag: EQ (equal to)");
        else if(Flag == 2) LabelText.append("\nBranch flag: GT (greater than)");
        else if(Flag == 3) LabelText.append("\nBranch flag: LT (less than)");
    }

    Label->setText(LabelText);
}


void RegisterInfoWidget::ShowValueEditWidget()
{
    ValueEditWidget = new QWidget;
    ValueEditWidget->setWindowFlag(Qt::WindowStaysOnTopHint, true);
    ValueEditWidget->setWindowTitle("Edit Register Value");
    QBoxLayout *Layout = new QBoxLayout(QBoxLayout::Down, ValueEditWidget);

    ValueTextEdit = new QTextEdit(ValueEditWidget);
    ValueTextEdit->setMaximumHeight(25);
    Layout->addWidget(ValueTextEdit);

    QBoxLayout *GroupBoxLayout = new QBoxLayout(QBoxLayout::LeftToRight,ValueEditWidget);
    QGroupBox *GroupBox = new QGroupBox(tr("Value type"),ValueEditWidget);
    RadioEditDec = new QRadioButton(tr("Decimal"), ValueTextEdit);
    GroupBoxLayout->addWidget(RadioEditDec);
    RadioEditDec->setChecked(true);
    RadioEditHex = new QRadioButton(tr("Hexidecimal"), ValueTextEdit);
    GroupBoxLayout->addWidget(RadioEditHex);
    RadioEditBin = new QRadioButton(tr("Binary"), ValueTextEdit);
    GroupBoxLayout->addWidget(RadioEditBin);
    GroupBox->setLayout(GroupBoxLayout);

    Layout->addWidget(GroupBox);

    QPushButton *EditValueButton = new QPushButton("Edit Value", ValueEditWidget);
    Layout->addWidget(EditValueButton);
    connect(EditValueButton, SIGNAL(clicked()), this, SLOT(UpdateRegValue()));

    ValueEditWidget->show();
}

void RegisterInfoWidget::UpdateRegValue()
{
    QString StringValue = ValueTextEdit->toPlainText();
    bool Value[24];
    if(RadioEditDec->isChecked())
    {
       if(BinaryUtilities::StringDecToBin(StringValue, Value))
       {
           RegistersPtr[RegisterIndex].Write(Value);
           RegistersPtr[RegisterIndex].SetStateIdle();
           emit RegValueUpdated();
           Update();
           ValueEditWidget->close();
           return;
       }
    }
    else if(RadioEditHex->isChecked())
    {
        if(BinaryUtilities::StringHexToBin(StringValue, Value))
        {
            RegistersPtr[RegisterIndex].Write(Value);
            RegistersPtr[RegisterIndex].SetStateIdle();
            emit RegValueUpdated();
            Update();
            ValueEditWidget->close();
            return;
        }
    }
    else if(RadioEditBin->isChecked())
    {
        if(BinaryUtilities::StringBinToBin(StringValue, Value))
        {
            RegistersPtr[RegisterIndex].Write(Value);
            RegistersPtr[RegisterIndex].SetStateIdle();
            emit RegValueUpdated();
            Update();
            ValueEditWidget->close();
            return;
        }
    }

    QMessageBox MsgBox;
    MsgBox.setText("Input number invalid, please enter a valid number.");
    MsgBox.setWindowTitle("Error");
    MsgBox.exec();

}

