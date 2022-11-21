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
#include <QLineEdit>

#include "PlanetViewer.hpp"


class PlanetDockWidget : public QDockWidget
{
    Q_OBJECT
public:
    PlanetDockWidget(PlanetViewer * _viewer, QWidget * parent );
    PlanetViewer *viewer;

    //General planet parameters
    QSlider *numPlateSlider;
    QPushButton *confirmButton;
    QLabel * platenumLabel, *planetRadiusLabel;
    QLineEdit *planetRadius;


    //Plate type parameters
    QLabel *oThicknessLabel, *oElevationLabel;
    QLineEdit *oceanicThickness, *oceanicElevation;
    QLabel *cThicknessLabel, *cElevationLabel;
    QLineEdit *continentalElevation, *continentalThickness;

protected:
    QGridLayout * segIGridLayout;
    QSignalMapper * signalIMapper;

public slots:
    void setPlateNumText();
};

#endif // TEXTUREDOCKWINDOW_H
