#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QString>
#include <QMessageBox>
#include <windows.h>
#include <QScopedPointer>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setFixedSize(450*devicePixelRatio(),150*devicePixelRatio());
}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_EXEPicker_Btn_pressed()
{
    QFileDialog QFD;
    QFD.setAcceptMode(QFileDialog::AcceptOpen);
    QFD.setFileMode(QFileDialog::ExistingFile);
    QFD.setNameFilter("EXE Files (*.exe)");
    QFD.setViewMode(QFileDialog::List);
    QFD.setDirectory(QDir::currentPath());
    QFD.setOption(QFileDialog::DontUseNativeDialog, false);
    if (QFD.exec())
    {
        auto lists=QFD.selectedFiles();
        if (lists.size()!=0)
        {
            this->ui->EXEPath_Le->setText(QDir::toNativeSeparators(lists[0]));
        }
    }

}

void MainWindow::on_DLLPicker_Btn_pressed()
{
    QFileDialog QFD;
    QFD.setAcceptMode(QFileDialog::AcceptOpen);
    QFD.setFileMode(QFileDialog::ExistingFile);
    QFD.setNameFilter("DLL Files (*.dll)");
    QFD.setViewMode(QFileDialog::List);
    QFD.setDirectory(QDir::currentPath());
    QFD.setOption(QFileDialog::DontUseNativeDialog, false);
    if (QFD.exec())
    {
        auto lists=QFD.selectedFiles();
        if (lists.size()!=0)
        {
            this->ui->DLLPath_Le->setText(QDir::toNativeSeparators(lists[0]));
        }
    }
}
typedef bool (__stdcall* Runinject)(wchar_t* DLLFullPath, wchar_t* EXEFullPath);
#define BUFSIZE 4096

bool GetFullPath(wchar_t* RelPath, wchar_t* lp_FullPath)
{
    DWORD  retval = 0;
    retval=GetFullPathNameW(RelPath, BUFSIZE, lp_FullPath, NULL);
    if (retval == 0)
    {
        QMessageBox::critical(NULL,"错误","获取完整路径失败",QMessageBox::StandardButton::Ok);
        return false;
    }
    else
    {
        return true;
    }
}

bool RunInjectProc(wchar_t* DLLPath,wchar_t* EXEPath)
{
    wchar_t clp_DllPath[BUFSIZE];
    wchar_t clp_ExePath[BUFSIZE];
    memset(clp_DllPath,0,BUFSIZE*2);
    memset(clp_ExePath,0,BUFSIZE*2);
    HMODULE hDLL = LoadLibrary(L"DllInjector.dll");
    if (hDLL == NULL)
    {
        QMessageBox::critical(NULL,"错误","加载DllInjector.dll失败",QMessageBox::StandardButton::Ok);
        return false;
    }
    Runinject RI_fp1 = Runinject(GetProcAddress(hDLL, "Runinject"));
    if (RI_fp1 == NULL)
    {
        QMessageBox::critical(NULL,"错误","获取函数地址失败",QMessageBox::StandardButton::Ok);
        return false;
    }
    if (!GetFullPath(DLLPath, clp_DllPath))
    {
        QMessageBox::critical(NULL,"错误","获取DLL完整路径失败",QMessageBox::StandardButton::Ok);
        return false;
    }
    if (!GetFullPath(EXEPath, clp_ExePath))
    {
        QMessageBox::critical(NULL,"错误","获取EXE完整路径失败",QMessageBox::StandardButton::Ok);
        return false;
    }
    auto status=RI_fp1(clp_DllPath, clp_ExePath);
    return status?true:false;
}
void MainWindow::on_Act_Btn_pressed()
{
    if(!ui->DLLPath_Le->text().isEmpty()&&!ui->EXEPath_Le->text().isEmpty())
    {
        QScopedPointer<wchar_t> EXEPath(new wchar_t[4096]);
        QScopedPointer<wchar_t> DLLPath(new wchar_t[4096]);
        memset(EXEPath.data(),0,4096*2);
        memset(DLLPath.data(),0,4096*2);
        ui->EXEPath_Le->text().toWCharArray(EXEPath.data());
        ui->DLLPath_Le->text().toWCharArray(DLLPath.data());
        auto status=RunInjectProc(DLLPath.data(),EXEPath.data());
        if(!status)
        {
            QMessageBox::critical(this,"错误","注入失败",QMessageBox::StandardButton::Ok);
        }
        else
        {
            QMessageBox::information(this,"完成","注入完成",QMessageBox::StandardButton::Ok);
        }
    }
    else
    {
        QMessageBox::critical(this,"错误","路径为空",QMessageBox::StandardButton::Ok);
    }
}
