#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>

#include <QStatusBar>
#include <QWidget>
#include <QGroupBox>

#include "PlanetDockWidget.hpp"

class Window : public QMainWindow
{
Q_OBJECT

public:
Window();

private:
    PlanetDockWidget *madDockWidget;
    PlanetViewer * viewer;
    QStatusBar * statusbar;

public slots:
};


#endif // MAINWINDOW_H
