#ifndef TEXTUREDOCKWINDOW_H
#define TEXTUREDOCKWINDOW_H


#include <QButtonGroup>
#include <QCheckBox>
#include <QDockWidget>
#include <QFrame>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QPushButton>
#include <QSlider>
#include <QSpacerItem>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QStatusBar>
#include <QWidget>
#include <QFormLayout>
#include <QListWidget>
#include <QScrollArea>
#include <QTabWidget>
#include <QSignalMapper>

#include "PlanetViewer.h"


class PlanetDockWidget : public QDockWidget
{
    Q_OBJECT
public:
    PlanetDockWidget(PlanetViewer * _viewer, QWidget * parent );
    PlanetViewer *viewer;
protected:
    QGroupBox * getCuttingPlaneGroupBox(QWidget * parent);
    QGroupBox * displayImageGroupBox;
    QGridLayout * segIGridLayout;
    QSignalMapper * signalIMapper;

public slots:
};

#endif // TEXTUREDOCKWINDOW_H
