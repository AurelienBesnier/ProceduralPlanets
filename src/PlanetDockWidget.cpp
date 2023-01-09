#include "PlanetDockWidget.hpp"
#include <QFileDialog>
#include <QComboBox>

PlanetDockWidget::PlanetDockWidget (PlanetViewer *_viewer, QWidget *parent) : QDockWidget (
		parent)
{
	viewer = _viewer;

	QWidget *contents = new QWidget ();

	QVBoxLayout *contentLayout = new QVBoxLayout (contents);

	QGroupBox *groupBox = new QGroupBox ("Planet Parameters", parent);
	groupBox->setMaximumSize (QSize (16777215, 200));
	contentLayout->addWidget (groupBox);

	//********************Planet Editor***********************/
	QGridLayout *planetParamLayout = new QGridLayout (groupBox);

	numPlateSlider = new QSlider (groupBox);
	numPlateSlider->setOrientation (Qt::Horizontal);
    numPlateSlider->setMinimum (2);
    numPlateSlider->setMaximum (30);
    numPlateSlider->setValue(17);

	planetParamLayout->addWidget (numPlateSlider, 0, 1, 1, 1);

	platenumLabel = new QLabel (
			QString ("Plate Number:%1").arg (numPlateSlider->value ()),
			groupBox);
	planetParamLayout->addWidget (platenumLabel, 0, 0, 1, 1);
	connect (numPlateSlider, SIGNAL(valueChanged(int)), viewer,
				SLOT(setPlateNumber(int)));
	connect (numPlateSlider, SIGNAL(valueChanged(int)), this,
				SLOT(setPlateNumText()));

	planetRadius = new QLineEdit ();
    planetRadius->setText("6370");
	planetParamLayout->addWidget (planetRadius, 1, 1, 1, 1);

	planetRadiusLabel = new QLabel (QString ("Planet Radius (in km):"));
	planetParamLayout->addWidget (planetRadiusLabel, 1, 0, 1, 1);

	connect (planetRadius, SIGNAL(textChanged(QString)), viewer,
				SLOT(setPlanetRadius(QString)));

    planetElements = new QLineEdit ();
    planetElements->setText("6000");
	QValidator *intValidator = new QIntValidator(10, 60000000, this);
	planetElements->setValidator(intValidator);

	planetElementLabel = new QLabel (QString ("Planet Resolution: "));
	planetParamLayout->addWidget (planetElementLabel, 2, 0, 1, 1);

	planetParamLayout->addWidget (planetElements, 2, 1, 1, 1);

	connect (planetElements, SIGNAL(textChanged(QString)), viewer,
                SLOT(setPlanetElem(QString)));

	confirmButton = new QPushButton ("Generate", groupBox);
	planetParamLayout->addWidget (confirmButton, 4, 1, 1, 1);
	connect (confirmButton, SIGNAL(clicked()), viewer, SLOT(generatePlanet()));

	clearButton = new QPushButton ("Clear", groupBox);
	planetParamLayout->addWidget (clearButton, 4, 2, 1, 1);
	connect (clearButton, SIGNAL(clicked()), viewer, SLOT(clearPlanet()));

	resgementButton = new QPushButton ("Resegment", groupBox);
	planetParamLayout->addWidget (resgementButton, 5, 1, 1, 2);
	connect (resgementButton, SIGNAL(clicked()), viewer, SLOT(resegment()));

	//********************Oceanic Editor***********************/
	QGroupBox *oceanicPlateBox = new QGroupBox ("Oceanic Plate", parent);
	oceanicPlateBox->setMaximumSize (QSize (16777215, 200));

	QGridLayout *oceanParamLayout = new QGridLayout (oceanicPlateBox);
	contentLayout->addWidget (oceanicPlateBox);

	oElevationLabel = new QLabel (QString ("Abyssal Plain Elevation (in km):"));
	oceanParamLayout->addWidget (oElevationLabel, 2, 0, 1, 1);
	oceanicElevation = new QLineEdit ();
	oceanicElevation->setText("-10");
	oceanParamLayout->addWidget (oceanicElevation, 2, 1, 1, 1);

	nbOctaveNoiseOceanic = new QSlider (oceanicPlateBox);
	nbOctaveNoiseOceanic->setOrientation (Qt::Horizontal);
    nbOctaveNoiseOceanic->setMinimum (1);
    nbOctaveNoiseOceanic->setMaximum (8);
    nbOctaveNoiseOceanic->setValue(1);

	oOctaveLabel = new QLabel (
			QString ("Octaves oceanic noise:%1").arg (nbOctaveNoiseOceanic->value ()),
			oceanicPlateBox);
	oceanParamLayout->addWidget (oOctaveLabel, 3, 0, 1, 1);
	oceanParamLayout->addWidget (nbOctaveNoiseOceanic, 3, 1, 1, 1);
	connect (nbOctaveNoiseOceanic, SIGNAL(valueChanged(int)), viewer,
				SLOT(setOceanicOctave(int)));
	connect (nbOctaveNoiseOceanic, SIGNAL(valueChanged(int)), this,
				SLOT(setOceanicOctaveText()));

	connect (oceanicElevation, SIGNAL(textChanged(QString)), viewer,
				SLOT(setOceanicElevation(QString)));

	//********************Continent Editor***********************/
	QGroupBox *continentalPlateBox = new QGroupBox ("Continental Plate",
													parent);
	continentalPlateBox->setMaximumSize (QSize (16777215, 200));
	QGridLayout *contParamLayout = new QGridLayout (continentalPlateBox);
	contentLayout->addWidget (continentalPlateBox);

	cElevationLabel = new QLabel (QString ("Max Continental Elevation (in km):"));
	contParamLayout->addWidget (cElevationLabel, 2, 0, 1, 1);
	continentalElevation = new QLineEdit ();
	continentalElevation->setText("10");
	contParamLayout->addWidget (continentalElevation, 2, 1, 1, 1);

	nbOctaveNoiseContinental = new QSlider (continentalPlateBox);
	nbOctaveNoiseContinental->setOrientation (Qt::Horizontal);
    nbOctaveNoiseContinental->setMinimum (1);
    nbOctaveNoiseContinental->setMaximum (8);
    nbOctaveNoiseContinental->setValue(1);

	cOctaveLabel = new QLabel (
		QString ("Octaves continental noise:%1").arg (nbOctaveNoiseContinental->value ()),
		continentalPlateBox);
	contParamLayout->addWidget (cOctaveLabel, 3, 0, 1, 1);
	contParamLayout->addWidget (nbOctaveNoiseContinental, 3, 1, 1, 1);
	connect (nbOctaveNoiseContinental, SIGNAL(valueChanged(int)), viewer,
				SLOT(setContinentalOctave(int)));
	connect (nbOctaveNoiseContinental, SIGNAL(valueChanged(int)), this,
				SLOT(setContinentalOctaveText()));

	connect (continentalElevation, SIGNAL(textChanged(QString)), viewer,
				SLOT(setContinentElevation(QString)));

	//Make lineedits accept numbers only
    QDoubleValidator *validator = new QDoubleValidator (-10000.0, 10000.0, 6,
														this);
	validator->setLocale (QLocale::C);
	validator->setNotation (QDoubleValidator::StandardNotation);
	planetRadius->setValidator (validator);
	oceanicElevation->setValidator (validator);
	continentalElevation->setValidator (validator);

	//********************Plate list***********************/
	connect(this, SIGNAL(planetFinished()),this, SLOT(setPlateIndicators()));
	QGroupBox *plateListBox = new QGroupBox ("Plate list",
													parent);
	plateListBox->setMaximumSize (QSize (16777215, 200));
	plateListLayout = new QGridLayout (plateListBox);

	contentLayout->addWidget (plateListBox);

	contentLayout->addStretch (0);

	this->setWidget (contents);
}

void PlanetDockWidget::setPlateNumText ()
{
	platenumLabel->setText (
			QString ("Plate Number:%1").arg (numPlateSlider->value ()));
	update ();
}

void PlanetDockWidget::setOceanicOctaveText ()
{
	oOctaveLabel->setText (
			QString ("Octaves oceanic noise:%1").arg (nbOctaveNoiseOceanic->value ()));
	update ();
}

void PlanetDockWidget::setContinentalOctaveText ()
{
	cOctaveLabel->setText (
			QString ("Octaves continental noise:%1").arg (nbOctaveNoiseContinental->value ()));
	update ();
}

void PlanetDockWidget::setPlateIndicators()
{
	for(unsigned short i = 0; i < viewer.plates.size(); i++)
	{
		//TODO: Make handler of plates
	}
}