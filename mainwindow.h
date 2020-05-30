#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "addressbus.h"
#include "databus.h"
#include "controlunit.h"
#include "arithlogunit.h"
#include "memory.h"
#include "indatabus.h"
#include "alucontrolbus.h"
#include "controlbus.h"
#include "register.h"




namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    //CPU components
    Register Registers[17];
    ControlUnit CoUnit;
    ArithLogUnit ArLogUnit;
    Memory Mem;
    InDataBus InBus;
    AddressBus MemAddBus;
    DataBus MemDatBus;
    AluControlBus ACoBus;
    ControlBus CntrlBus;

    //Pointers to widgets
    class Diagram *CPUDiagram;
    class ControlWidget *CntrlWidget;
    class QScrollArea *MemoryWindow;
    class MemoryWidget *MemWidget;
    class AssemblerWindow *AssemWindow;

    //Booleans to store whether windows are showing
    bool MemWindowShowing = false;
    bool AssemblerWindowShowing = false;

    //Timer to run simlation
    class QTimer *timer;

    int SimulationSpeeds[5] = {2000, 1000, 500, 0, 0};
    int CurrentSpeed = 0;

    void SetUp();
    void CreateMenus();

    //Pointer to actions in menus
    class QAction *ToggleMemWindow;
    class QAction *ToggleAssemWindow;
protected slots:
    void StepSimulation();
    void Play();
    void Pause();
    void SetSpeed(int i);
    void ToggleMemoryWindow();
    void MemoryWindowClosed();
    void ToggleAssemblerWindow();
    void AssemblerWindowClosed();
    void Assembled();
signals:
    void SimulationStepped();
    void SimulationHalted();
    void SimulationActive();
};

#endif // MAINWINDOW_H
