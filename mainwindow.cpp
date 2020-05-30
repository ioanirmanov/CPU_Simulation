#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QTimer>
#include "controlwidget.h"
#include<QGridLayout>
#include "diagram.h"
#include<QScrollArea>
#include "memorywidget.h"
#include<QAction>
#include "assemblerwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    SetUp();
    CreateMenus();
    QGridLayout *mainLayout = new QGridLayout;
    CPUDiagram = new Diagram(this, Registers, &ArLogUnit, &Mem, &InBus, &MemAddBus, &MemDatBus, &CntrlBus, &ACoBus, &CoUnit);
    CntrlWidget = new ControlWidget(this);
    connect(CntrlWidget, SIGNAL(Play()), this, SLOT(Play()));
    connect(CntrlWidget, SIGNAL(Pause()), this, SLOT(Pause()));
    connect(CntrlWidget, SIGNAL(Step()), this, SLOT(StepSimulation()));
    connect(CntrlWidget, SIGNAL(SelectSpeed(int)), this, SLOT(SetSpeed(int)));
    connect(this, SIGNAL(SimulationActive()), CntrlWidget, SLOT(SimulationActivated()));
    connect(this, SIGNAL(SimulationHalted()), CntrlWidget, SLOT(SimulationHalted()));

    mainLayout->addWidget(CPUDiagram, 0, 0, 0);
    mainLayout->addWidget(CntrlWidget, 1, 0, Qt::AlignCenter);
    mainLayout->setRowStretch(0, 5);
    mainLayout->setColumnStretch(0, 5);
    // Set layout in QWidget
    QWidget *window = new QWidget();
    window->setLayout(mainLayout);
    setCentralWidget(window);

    //Show windows
    ToggleMemoryWindow();
    ToggleAssemblerWindow();

    this->setWindowTitle("CPU Simulation");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SetUp()
{
    Mem = Memory();
    InBus = InDataBus(Registers);
    MemAddBus = AddressBus(&Mem, &Registers[10]);
    MemDatBus = DataBus(&Mem, &Registers[11]);
    Mem.SetDataBusPtr(&MemDatBus);
    CntrlBus = ControlBus(&Mem);
    CoUnit = ControlUnit(&Registers[9], &Registers[8], &Registers[16], &InBus, &CntrlBus, &MemAddBus, &MemDatBus,&ACoBus);
    ArLogUnit = ArithLogUnit(&Registers[12], &Registers[13], &Registers[14], &Registers[15]);
    ACoBus = AluControlBus(&ArLogUnit);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(StepSimulation()));

}

void MainWindow::CreateMenus()
{
    QMenu *WindowMenu = menuBar()->addMenu(tr("&Windows"));

    ToggleMemWindow = new QAction(tr("&Memory Window"), this);
    ToggleMemWindow->setCheckable(true);

    WindowMenu->addAction(ToggleMemWindow);
    connect(ToggleMemWindow, &QAction::triggered, this, &MainWindow::ToggleMemoryWindow);

    ToggleAssemWindow = new QAction(tr("&Assembler Window"), this);
    ToggleAssemWindow->setCheckable(true);

    WindowMenu->addAction(ToggleAssemWindow);
    connect(ToggleAssemWindow, &QAction::triggered, this, &MainWindow::ToggleAssemblerWindow);
}

void MainWindow::StepSimulation()
{
    if(CoUnit.GetState() != CoUnState::Halt)
    {
        for(int i = 0; i < 17; i++)
        {
            Registers[i].SetStateIdle();
        }
        ArLogUnit.SetStateIdle();
        CoUnit.Step();
        if(CurrentSpeed != 4)
        {
            CPUDiagram->UpdateDiagram();
            if(MemWindowShowing) MemWidget->Update();
            emit SimulationStepped();
        }
    }
    else
    {
        timer->stop();
        emit SimulationHalted();
        if(CurrentSpeed == 4)
        {
            CPUDiagram->UpdateDiagram();
            if(MemWindowShowing) MemWidget->Update();
            emit SimulationStepped();
        }

    }
}

void MainWindow::Play()
{
    timer->start(SimulationSpeeds[CurrentSpeed]);
}

void MainWindow::Pause()
{
    timer->stop();
}

void MainWindow::SetSpeed(int i)
{
    if(i >= 0 && i < 5)
    {
        CurrentSpeed = i;
        timer->setInterval(SimulationSpeeds[CurrentSpeed]);
    }
}

void MainWindow::ToggleMemoryWindow()
{
    if(MemWindowShowing)
    {
        if(MemoryWindow)
        {
            MemoryWindow->close();
        }
        MemWindowShowing = false;
    }
    else
    {
        //Make window
        MemoryWindow = new QScrollArea;
        MemoryWindow->setWindowFlag(Qt::WindowStaysOnTopHint, true);
        MemoryWindow->setMinimumSize(QSize(130,500));
        MemWidget = new MemoryWidget(MemoryWindow, &Mem);
        MemoryWindow->show();
        MemoryWindow->setWidget(MemWidget); 
        MemoryWindow->setAttribute( Qt::WA_DeleteOnClose );
        MemoryWindow->setWindowTitle("Memory Window");

        MemWindowShowing = true;
        ToggleMemWindow->setChecked(true);

        //If memory window is closed toggle checked menu, set MemWindowSHowing to false
        connect( MemoryWindow, &QWidget::destroyed, this, &MainWindow::MemoryWindowClosed);
        //Update memory window when program assembled
        if(AssemblerWindowShowing) connect(AssemWindow, &AssemblerWindow::Assembled, MemWidget, &MemoryWidget::Update);
    }
}

void MainWindow::MemoryWindowClosed()
{
    MemWindowShowing = false;
    ToggleMemWindow->setChecked(false);
}

void MainWindow::ToggleAssemblerWindow()
{
    if(AssemblerWindowShowing)
    {
        if(AssemWindow)
        {
            AssemWindow->close();
        }
        AssemblerWindowShowing = false;
    }
    else
    {
        //Show window
        if(!AssemWindow)
        {
            AssemWindow = new AssemblerWindow(this, &Mem);
        }
        AssemWindow->show();
        AssemWindow->setWindowTitle("Assembler Window");

        AssemblerWindowShowing = true;
        ToggleAssemWindow->setChecked(true);

        //If assembler window is closed toggle checked menu, set MemWindowShowing to false
        connect(AssemWindow, &AssemblerWindow::closed, this, &MainWindow::AssemblerWindowClosed);
        //Update memory window, diagram and reset simulation when program assembled
        if(MemWindowShowing) connect(AssemWindow, &AssemblerWindow::Assembled, MemWidget, &MemoryWidget::Update);
        connect(AssemWindow, &AssemblerWindow::Assembled, CPUDiagram, &Diagram::UpdateDiagram);
        connect(AssemWindow, &AssemblerWindow::Assembled, this, &MainWindow::Assembled);
    }
}

void MainWindow::AssemblerWindowClosed()
{
    AssemblerWindowShowing = false;
    ToggleAssemWindow->setChecked(false);
}

void MainWindow::Assembled()
{
    CoUnit.SetStart();
    bool Zero[24] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    for(int i = 0; i < 17; i++)
    {
        Registers[i].Write(Zero);
        Registers[i].SetStateIdle();
    }
    InBus.SetIdle();
    MemAddBus.SetIdle();
    MemDatBus.SetIdle();
    ACoBus.SetIdle();
    CntrlBus.SetIdle();

    emit SimulationActive();
}

