#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private slots:
  void on_EXEPicker_Btn_pressed();

  void on_DLLPicker_Btn_pressed();

  void on_Act_Btn_pressed();

private:
  Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
