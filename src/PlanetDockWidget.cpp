#include "PlanetDockWidget.h"
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

    planetParamLayout->addWidget(numPlateSlider, 1, 0, 1, 1);

    platenumLabel = new QLabel(QString("Plate Number:%1").arg(numPlateSlider->value()), groupBox);
    planetParamLayout->addWidget(platenumLabel, 0, 0, 1, 1);

    connect(numPlateSlider, SIGNAL(valueChanged(int)), viewer, SLOT(setPlateNumber(int)));
    connect(numPlateSlider, SIGNAL(valueChanged(int)), this, SLOT(setPlateNumText()));

    confirmButton = new QPushButton("Generate", groupBox);
    planetParamLayout->addWidget(confirmButton, 2, 1, 1, 1);


    connect(confirmButton, SIGNAL(clicked()), viewer, SLOT(generatePlanet()));

    contentLayout->addStretch(0);
    this->setWidget(contents);
}

void PlanetDockWidget::setPlateNumText()
{
    platenumLabel->setText(QString("Plate Number:%1").arg(numPlateSlider->value()));
    update();
}
