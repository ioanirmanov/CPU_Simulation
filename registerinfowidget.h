#ifndef REGISTERINFOWIDGET_H
#define REGISTERINFOWIDGET_H

#include <QWidget>

class RegisterInfoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RegisterInfoWidget(QWidget *parent = nullptr);
    RegisterInfoWidget(QWidget *parent, class Register *RegsPtr, int RegIndex);
signals:
    void RegValueUpdated();
public slots:
    void Update();
    void ShowValueEditWidget();
    void UpdateRegValue();
private:
    void GenerateLabelText();
    class Register *RegistersPtr;
    int RegisterIndex = 0;
    class QLabel *Label;
    class QPushButton *Button;
    QString RegisterNames[16] = {"R0", "R1", "R2", "R3", "R4", "R5", "R6", "R7"
                                , "PC", "CIR", "MAR", "MBR", "OP1", "OP2", "RESULT", "FLAG"};

    QWidget *ValueEditWidget;
    class QTextEdit *ValueTextEdit;
    class QRadioButton *RadioEditDec;
    class QRadioButton *RadioEditHex;
    class QRadioButton *RadioEditBin;
};

#endif // REGISTERINFOWIDGET_H
