#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "control.h"
#include "dialog.h"

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    control *m_control = nullptr;

protected:
    void paintEvent(QPaintEvent *event);

private:
    Ui::MainWindow *ui;
    Dialog *dial = nullptr;
    QString srcDirPath;
    QImage mImage;

private:
    void _conn();

private slots:
    void on_Selected_Clicked();
    void on_Media_Clicked();
    void on_Info_Clicked();
    void slotGetOneFrame(QImage &img);
    void showFileError();
};
#endif // MAINWINDOW_H
