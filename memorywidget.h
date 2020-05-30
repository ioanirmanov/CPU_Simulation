#ifndef MEMORYWIDGET_H
#define MEMORYWIDGET_H

#include <QWidget>

class MemoryWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MemoryWidget(QWidget *parent = nullptr);
    MemoryWidget(QWidget *parent, class Memory *MemPtr);
signals:
   void Update();
public slots:
   void ShowMemInfoWindow(int Loc);
protected:
    void paintEvent(QPaintEvent *event) override;
private:
    class Memory *MemoryPtr;
    class QPushButton *LocationButtons[256];
    class QSignalMapper *SignalMapper;
};

#endif // MEMORYWIDGET_H
