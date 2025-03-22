#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QTimer>
#include <QLabel>
#include <QPushButton>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

enum PumpState {
    ENABLED,
    DISABLED,
    BROKEN
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void updateWaterLevel(int newWaterLevel);

private slots:
    void onStartButtonClick();

    void onValveUpButtonClick();
    void onValveDownButtonClick();

    void onPump1StatusClick();
    void onPump2StatusClick();
    void onPump3StatusClick();

private:
    Ui::MainWindow *ui;
    QGraphicsScene graphicsScene;

    bool isRunning{false};
    int iteration{0}, waterLevel{0}, valveLevel{0};
    QTimer updateTimer;

    std::array<QLabel*, 3> pumpLabels;
    std::array<QPushButton*, 3> pumpStatusLabels;
    PumpState pumpsStates[3] = { DISABLED, DISABLED, DISABLED };
    int pumpsStats[3] = {0};

    void updateLabels();
    void updateValve();
    void updatePump();
};
#endif // MAINWINDOW_H
