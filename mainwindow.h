#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QMessageBox>
#include <QFileDialog>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_ReadFile_clicked();

    void on_pushButton_OPENFILE_clicked();

    void on_pushButton_PPP_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
