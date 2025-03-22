#include "../include/mainwindow.h"
#include "./ui_mainwindow.h"

#include <QDebug>
#include <QOpenGLWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , graphicsScene(0, 0, 250, 180)
{
    ui->setupUi(this);
    ui->graphicsView->setScene(&graphicsScene);

    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    pumpLabels = { ui->pump1Stats, ui->pump2Stats, ui->pump3Stats };
    pumpStatusLabels = { ui->pump1Status, ui->pump2Status, ui->pump3Status };

    connect(ui->startButton, &QPushButton::clicked, this, &MainWindow::onStartButtonClick);
    connect(ui->valveUpButton, &QPushButton::clicked, this, &MainWindow::onValveUpButtonClick);
    connect(ui->valveDownButton, &QPushButton::clicked, this, &MainWindow::onValveDownButtonClick);
    connect(ui->pump1Status, &QPushButton::clicked, this, &MainWindow::onPump1StatusClick);
    connect(ui->pump2Status, &QPushButton::clicked, this, &MainWindow::onPump2StatusClick);
    connect(ui->pump3Status, &QPushButton::clicked, this, &MainWindow::onPump3StatusClick);

    connect(&updateTimer, &QTimer::timeout, [this]() {
        if (!isRunning) {
            return;
        }

        iteration++;

        updatePump();
        updateValve();
        updateLabels();
    });
    updateTimer.start(20);
}

void MainWindow::updateWaterLevel(int newWaterLevel)
{
    if (newWaterLevel < 0 || newWaterLevel > 1000) return;

    waterLevel = newWaterLevel;

    graphicsScene.clear();
    graphicsScene.addRect(QRectF(0, 180 - 0.18 * waterLevel, 250, 0.18 * waterLevel), QPen(Qt::white), QBrush(Qt::blue));

    ui->waterLevelLabel->setText(QString::number(waterLevel / 10.0) + "%");
}

void MainWindow::updateLabels()
{
    ui->iterationLabel->setText(QString::number(iteration));

    std::unordered_map<PumpState, QString> state2str = {
        {ENABLED, "Работает"}, {DISABLED, "Простаивает"}, {BROKEN, "Сломан"}
    };

    for (int i = 0; i != 3; ++i) {
        if (pumpsStates[i] == ENABLED) {
            pumpLabels[i]->setText(QString::number(pumpsStats[i]));
        }
        pumpStatusLabels[i]->setText(state2str[pumpsStates[i]]);
    }
}

void MainWindow::updateValve()
{
    updateWaterLevel(waterLevel - valveLevel);
}

void MainWindow::updatePump()
{
    auto activePumpsCount = std::count_if(std::begin(pumpsStates), std::end(pumpsStates), [](auto element) {
        return element == ENABLED;
    });

    auto stopPumps = [&]() {
        for (int i = 0; i != 3; ++i) {
            if (pumpsStates[i] == BROKEN) continue;
            pumpsStates[i] = DISABLED;
        }
    };

    std::vector<std::pair<int, int>> activePumps;
    for (int i = 0; i != 3; ++i) {
        if (pumpsStates[i] != BROKEN) activePumps.push_back({pumpsStats[i], i});
    }
    std::sort(activePumps.begin(), activePumps.end());

    if (waterLevel < 0.25 * 1000 && activePumpsCount != 2)
    {
        stopPumps();
        for (activePumpsCount = 0; activePumpsCount != std::min(activePumps.size(), static_cast<std::size_t>(2)); ++activePumpsCount)
        {
            pumpsStates[std::get<1>(activePumps[activePumpsCount])] = ENABLED;
        }
    } else if (waterLevel >= 0.25 * 1000 && waterLevel < 0.5 * 1000 && activePumpsCount != 1)
    {
        stopPumps();
        for (activePumpsCount = 0; activePumpsCount != std::min(activePumps.size(), static_cast<std::size_t>(1)); ++activePumpsCount)
        {
            pumpsStates[std::get<1>(activePumps[activePumpsCount])] = ENABLED;
        }
    } else if (waterLevel > 0.5 * 1000 && activePumpsCount != 0)
    {
        stopPumps();
        activePumpsCount = 0;
    }

    for (int i = 0; i != 3; ++i) {
        if (pumpsStates[i] == ENABLED)
        {
            pumpsStats[i]++;
        }
    }

    updateWaterLevel(waterLevel + activePumpsCount * 2);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onStartButtonClick()
{
    isRunning = !isRunning;
    ui->startButton->setText(isRunning ? "Стоп" : "Запуск");
}

void MainWindow::onValveUpButtonClick()
{
    if (valveLevel == 4) return;
    ui->valveLabel->setText(QString::number(++valveLevel));
}

void MainWindow::onValveDownButtonClick()
{
    if (valveLevel == 0) return;
    ui->valveLabel->setText(QString::number(--valveLevel));
}

void MainWindow::onPump1StatusClick()
{
    pumpsStates[0] = pumpsStates[0] == BROKEN ? DISABLED : BROKEN;
    updateLabels();
}

void MainWindow::onPump2StatusClick()
{
    pumpsStates[1] = pumpsStates[1] == BROKEN ? DISABLED : BROKEN;
    updateLabels();

}

void MainWindow::onPump3StatusClick()
{
    pumpsStates[2] = pumpsStates[2] == BROKEN ? DISABLED : BROKEN;
    updateLabels();
}
