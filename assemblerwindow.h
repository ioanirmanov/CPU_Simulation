#ifndef ASSEMBLERWINDOW_H
#define ASSEMBLERWINDOW_H

#include <QMainWindow>

class AssemblerWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit AssemblerWindow(QWidget *parent = nullptr);
    AssemblerWindow(QWidget *parent, class Memory *MemPtr);
signals:
    void Assembled();
    void closed();
public slots:
private slots:
    void Save();
    void Open();
    void Assemble();
private:
    class QTextEdit *TextEdit;

    class Memory *MemoryPtr;

    void CreateMenu();
protected:
    void closeEvent(QCloseEvent *event) override;
};

#endif // ASSEMBLERWINDOW_H
