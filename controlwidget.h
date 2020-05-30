#ifndef CONTROLWIDGET_H
#define CONTROLWIDGET_H

#include <QWidget>

class ControlWidget : public QWidget
{
    Q_OBJECT
public:
    //explicit ControlWidget(QWidget *parent = nullptr);
    ControlWidget(QWidget *parent);
signals:
    void Play();
    void Pause();
    void Step();
    void SelectSpeed(int i);
public slots:
    void PlayButtonPressed();
    void StepButtonPressed();
    void SpeedSelectChanged(int i);
    void SimulationHalted();
    void SimulationActivated();
private:
    bool Playing = false;
    class QPushButton *PlayButton;
    class QComboBox *SpeedSelect;
    bool SimulationActive = false;
};

#endif // CONTROLWIDGET_H
