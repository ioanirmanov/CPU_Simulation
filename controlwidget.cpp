#include "controlwidget.h"
#include<QBoxLayout>
#include<QLabel>
#include<QPushButton>
#include<QFrame>
#include<QComboBox>


ControlWidget::ControlWidget(QWidget *parent) : QWidget(parent)
{
    setMaximumSize(QSize(300,100));

    QBoxLayout *centralLayout = new QBoxLayout(QBoxLayout::Down, this);

    QFrame *Frame = new QFrame(parent, 0);
    Frame->setFrameStyle(QFrame::Panel | QFrame::Raised);
    Frame->setLineWidth(2);

    QBoxLayout *VertLayout = new QBoxLayout(QBoxLayout::Down, this);

    QLabel *Label = new QLabel();
    Label->setText("Controls");
    VertLayout->addWidget(Label);

    QBoxLayout *ButtonLayout = new QBoxLayout(QBoxLayout::LeftToRight, this);

    PlayButton = new QPushButton("Play", this);
    ButtonLayout->addWidget(PlayButton);
    connect(PlayButton, SIGNAL(released()), this, SLOT(PlayButtonPressed()));

    QPushButton *StepButton = new QPushButton("Step", this);
    ButtonLayout->addWidget(StepButton);
    connect(StepButton, SIGNAL(released()), this, SLOT(StepButtonPressed()));

    QLabel *SpeedLabel = new QLabel();
    SpeedLabel->setText("Speed:");
    ButtonLayout->addWidget(SpeedLabel);

    SpeedSelect = new QComboBox(this);
    SpeedSelect->addItem("Slow");
    SpeedSelect->addItem("Medium");
    SpeedSelect->addItem("Fast");
    SpeedSelect->addItem("Fastest");
    SpeedSelect->addItem("Instant");
    ButtonLayout->addWidget(SpeedSelect);
    connect(SpeedSelect, SIGNAL(currentIndexChanged(int)), this, SLOT(SpeedSelectChanged(int)));

    VertLayout->addLayout(ButtonLayout);

    Frame->setLayout(VertLayout);

    centralLayout->addWidget(Frame);

    setLayout(centralLayout);

    SimulationActive = false;
}


void ControlWidget::PlayButtonPressed()
{
    if(SimulationActive)
    {
        if(Playing)
        {
            Pause();
            Playing = false;
            PlayButton->setText("Play");
        }
        else {
            Play();
            Playing = true;
            PlayButton->setText("Pause");
        }
    }
}

void ControlWidget::StepButtonPressed()
{
    Step();
}

void ControlWidget::SpeedSelectChanged(int i)
{
    SelectSpeed(i);
}

void ControlWidget::SimulationActivated()
{
    SimulationActive = true;
}

void ControlWidget::SimulationHalted()
{
    SimulationActive = false;
    Playing = false;
    PlayButton->setText("Play");
}



