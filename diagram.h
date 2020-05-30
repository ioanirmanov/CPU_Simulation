#ifndef DIAGRAM_H
#define DIAGRAM_H

#include <QWidget>
#include <QLabel>
#include <QPainter>

class Diagram : public QWidget
{
    Q_OBJECT
public:
    explicit Diagram(QWidget *parent = nullptr);
    Diagram(QWidget *parent, class Register *RegsPtr, class ArithLogUnit *ALUPtr, class Memory *MemPtr, class InDataBus *InBusPtr, class MemoryBus *MemAddBusPtr, class MemoryBus *MemDatBusPtr, class ControlBus *CntrlBusPtr, class AluControlBus *AluCntrlBusPtr, class ControlUnit *CoUnitPtr);
signals:
    void UpdateDiagram();
public slots:
    void ShowRegInfoWindow(int Reg);
private:
    class Register *Registers;
    class ArithLogUnit *ALU;
    class Memory *Mem;
    class InDataBus *InBus;
    class MemoryBus *MemAddressBus;
    class MemoryBus *MemDataBus;
    class ControlBus *CntrlBus;
    class AluControlBus *AluCntrlBus;
    class ControlUnit *CoUnit;

    class QPushButton *RegisterButtons[16];

    class QSignalMapper *SignalMapper;
protected:
    void paintEvent(QPaintEvent *event) override;


};

#endif // DIAGRAM_H
