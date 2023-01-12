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
#include <QWidget>
#include <QFormLayout>
#include <QListWidget>
#include <QScrollArea>
#include <QLineEdit>

#include "PlanetViewer.hpp"

class PlanetDockWidget : public QDockWidget {
Q_OBJECT
public:
	PlanetDockWidget (PlanetViewer *_viewer, QWidget *parent);
	PlanetViewer *viewer;

	//General planet parameters
    QSlider *numPlateSlider;
	QPushButton *confirmButton, *clearButton, *resgementButton,
	 *reInitElevationOcean,  *reInitElevationCont, *movementButton;
	QLabel *platenumLabel, *planetRadiusLabel, *planetElementLabel;
    QLineEdit *planetRadius, *planetElements;

	//Plate type parameters
	QLabel *oOctaveLabel,*oElevationLabel;
	QLineEdit *oceanicElevation;
	QLabel *cOctaveLabel, *cElevationLabel;
	QLineEdit *continentalElevation;
	QSlider *nbOctaveNoiseContinental, *nbOctaveNoiseOceanic;
	QGridLayout *plateListLayout, *PlateListGridLayout;
	QGroupBox *plateListBox;
	QListWidget *plateList;
public slots:
	void setPlateNumText ();
	void setOceanicOctaveText();
	void setContinentalOctaveText ();
	void setPlateIndicators();

	void clear();
	void generate();

signals:
    void planetFinished();
};

#endif // TEXTUREDOCKWINDOW_H
