#ifndef MEMORYINFOWINDOW_H
#define MEMORYINFOWINDOW_H

#include <QWidget>

class MemoryInfoWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MemoryInfoWindow(QWidget *parent = nullptr);
    MemoryInfoWindow(QWidget *parent, class Memory *MemPtr, int Loc);
signals:

public slots:
    void Update();
private:
    void GenerateLabelText();
    class Memory *MemoryPtr;
    int MemoryLocation = 0;
    class QLabel *Label;
};

#endif // MEMORYINFOWINDOW_H
