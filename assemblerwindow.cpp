#include "assemblerwindow.h"
#include "memory.h"
#include "memoryhelper.h"
#include "assembler.h"
#include <QTextEdit>
#include <QSizePolicy>
#include <QMessageBox>
#include <QMenu>
#include <QAction>
#include <QMenuBar>
#include <QFileDialog>
#include <QTextStream>
#include "memoryhelper.h"


AssemblerWindow::AssemblerWindow(QWidget *parent) : QMainWindow(parent)
{

}

AssemblerWindow::AssemblerWindow(QWidget *parent, Memory *MemPtr) : QMainWindow(parent)
{
    MemoryPtr = MemPtr;

    TextEdit = new QTextEdit(this);
    TextEdit->setMinimumSize(QSize(400,400));

    setCentralWidget(TextEdit);

    CreateMenu();
}

void AssemblerWindow::Save()
{
    QString Instructions = TextEdit->toPlainText();
    QString fileName = QFileDialog::getSaveFileName(this,
            tr("Save Program"), "",
            tr("Program (*.prg);;All Files (*)"));
    if (fileName.isEmpty()) return;
    else
    {
        //Save file
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::information(this, tr("Unable to open file"), file.errorString());
            return;
        }
        QTextStream out(&file);
        out << Instructions;
    }
}

void AssemblerWindow::Open()
{
    QString Instructions;
    QString fileName = QFileDialog::getOpenFileName(this,
            tr("Save Program"), "",
            tr("Program (*.prg);;All Files (*)"));

    if (fileName.isEmpty()) return;
    else
    {
        //Open file
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly))
        {
            QMessageBox::information(this, tr("Unable to open file"), file.errorString());
            return;
        }
        QTextStream in(&file);
        Instructions = in.readAll();
        TextEdit->setText(Instructions);
    }
}

void AssemblerWindow::Assemble()
{
    QString Instructions = TextEdit->toPlainText();

    MemoryHelper MemHelper;

    if(Assembler::Assemble(Instructions, &MemHelper))
    {
        MemoryPtr->SetContents(&MemHelper);
        emit Assembled();
        QMessageBox MsgBox;
        MsgBox.setText("Assemble successful");
        MsgBox.setWindowTitle("Assemble successful");
        MsgBox.exec();
    }
    else
    {
        QMessageBox MsgBox;
        MsgBox.setText("There is an error in the assembler code, assemble unsuccessful");
        MsgBox.setWindowTitle("Error");
        MsgBox.exec();
    }
}

void AssemblerWindow::CreateMenu()
{
     QMenu *FileMenu = menuBar()->addMenu(tr("&File"));

     QAction *SaveAction = new QAction(tr("&Save"), this);
     connect(SaveAction, &QAction::triggered, this, &AssemblerWindow::Save);
     FileMenu->addAction(SaveAction);

     QAction *OpenAction = new QAction(tr("&Open"), this);
     connect(OpenAction, &QAction::triggered, this, &AssemblerWindow::Open);
     FileMenu->addAction(OpenAction);

     QMenu *AsseblerMenu = menuBar()->addMenu(tr("&Assembler"));

     QAction *AssembleAction = new QAction(tr("&Assemble"), this);
     connect(AssembleAction, &QAction::triggered, this, &AssemblerWindow::Assemble);
     AsseblerMenu->addAction(AssembleAction);
}

void AssemblerWindow::closeEvent(QCloseEvent *event)
{
    emit closed();
}
