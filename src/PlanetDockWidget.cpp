#include "PlanetDockWidget.hpp"
#include <QFileDialog>
#include <QComboBox>

using namespace std;
PlanetDockWidget::PlanetDockWidget(PlanetViewer * _viewer, QWidget * parent ):QDockWidget(parent)
{
    viewer = _viewer;

    QWidget * contents = new QWidget();

    QVBoxLayout * contentLayout = new QVBoxLayout(contents);

    QGroupBox * groupBox = new QGroupBox("Planet Parameters", parent);
    groupBox->setMaximumSize(QSize(16777215, 200));
    contentLayout->addWidget ( groupBox) ;

    QGridLayout * planetParamLayout = new QGridLayout(groupBox);

    numPlateSlider = new QSlider(groupBox);
    numPlateSlider->setOrientation(Qt::Horizontal);
    numPlateSlider->setMinimum(1);
    numPlateSlider->setMaximum(10);
    
    planetParamLayout->addWidget(numPlateSlider, 0, 1, 1, 1);

    platenumLabel = new QLabel(QString("Plate Number:%1").arg(numPlateSlider->value()), groupBox);
    planetParamLayout->addWidget(platenumLabel, 0, 0, 1, 1);
    connect(numPlateSlider, SIGNAL(valueChanged(int)), viewer, SLOT(setPlateNumber(int)));
    connect(numPlateSlider, SIGNAL(valueChanged(int)), this, SLOT(setPlateNumText()));


    planetRadius = new QLineEdit();
    planetParamLayout->addWidget(planetRadius, 1,1,1,1) ;

    planetRadiusLabel = new QLabel(QString("Planet Radius:"));
    planetParamLayout->addWidget(planetRadiusLabel, 1,0,1,1); 


    confirmButton = new QPushButton("Generate", groupBox);
    planetParamLayout->addWidget(confirmButton, 4, 1, 1, 1);
    connect(confirmButton, SIGNAL(clicked()), viewer, SLOT(generatePlanet()));


    QGroupBox *oceanicPlateBox = new QGroupBox("Oceanic Plate", parent);
    oceanicPlateBox->setMaximumSize(QSize(16777215, 200));
    
    QGridLayout * oceanParamLayout = new QGridLayout(oceanicPlateBox);
    contentLayout->addWidget(oceanicPlateBox);

    oThicknessLabel = new QLabel(QString("Oceanic Thickness:"));
    oceanParamLayout->addWidget(oThicknessLabel, 1,0,1,1); 
    oceanicThickness = new QLineEdit();
    oceanParamLayout->addWidget(oceanicThickness, 1,1,1,1) ;

    oElevationLabel = new QLabel(QString("Oceanic Elevation:"));
    oceanParamLayout->addWidget(oElevationLabel, 2,0,1,1); 
    oceanicElevation = new QLineEdit();
    oceanParamLayout->addWidget(oceanicElevation, 2,1,1,1) ;

    QGroupBox *continentalPlateBox = new QGroupBox("Continental Plate", parent);
    continentalPlateBox->setMaximumSize(QSize(16777215, 200));
    QGridLayout * contParamLayout = new QGridLayout(continentalPlateBox);
    contentLayout->addWidget(continentalPlateBox);

    cThicknessLabel = new QLabel(QString("Continental Thickness:"));
    contParamLayout->addWidget(cThicknessLabel, 1,0,1,1); 
    continentalThickness = new QLineEdit();
    contParamLayout->addWidget(continentalThickness, 1,1,1,1) ;

    cElevationLabel = new QLabel(QString("Oceanic Elevation:"));
    contParamLayout->addWidget(cElevationLabel, 2,0,1,1); 
    continentalElevation = new QLineEdit();
    contParamLayout->addWidget(continentalElevation, 2,1,1,1) ;

    contentLayout->addStretch(0);
    this->setWidget(contents);
}

void PlanetDockWidget::setPlateNumText()
{
    platenumLabel->setText(QString("Plate Number:%1").arg(numPlateSlider->value()));
    update();
}
