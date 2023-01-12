#ifndef PLANETDOCKWINDOW_H
#define PLANETDOCKWINDOW_H

#include <QDockWidget>
#include <QFrame>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QWidget>
#include <QListWidget>
#include <QLineEdit>

#include "PlanetViewer.hpp"

class PlanetDockWidget : public QDockWidget {
Q_OBJECT
public:
	PlanetDockWidget (PlanetViewer *_viewer, QWidget *parent);
	PlanetViewer *viewer;

	//General planet parameters
    QSlider *numPlateSlider, *timeStep;
	QPushButton *confirmButton, *clearButton, *resgementButton,
	 *reInitElevationOcean,  *reInitElevationCont, *movementButton;
	QLabel *platenumLabel, *planetRadiusLabel, *planetElementLabel, *timeStepLabel;
    QLineEdit *planetRadius, *planetElements;

	//Plates parameters
	QLabel *oOctaveLabel,*oElevationLabel;
	QLineEdit *oceanicElevation;
	QLabel *cOctaveLabel, *cElevationLabel;
	QLineEdit *continentalElevation;
	QSlider *nbOctaveNoiseContinental, *nbOctaveNoiseOceanic;
	QGridLayout *plateListLayout, *PlateListGridLayout;
	QGroupBox *plateListBox;
	QListWidget *plateList;

public slots:

	/**
	* @brief Set the Plate Num Text object
	* 
	*/
	void setPlateNumText ();

	/**
	 * @brief Set the Oceanic Octave Text object
	 * 
	 */
	void setOceanicOctaveText();

	/**
	 * @brief Set the Continental Octave Text object
	 * 
	 */
	void setContinentalOctaveText ();

	/**
	 * @brief Set the Plate Indicators object
	 * 
	 */
	void setPlateIndicators();

	/**
	 * @brief Set the Time Step Label object
	 * 
	 */
	void setTimeStepLabel();

	/**
	 * @brief 
	 * 
	 */
	void clear();

	/**
	 * @brief 
	 * 
	 */
	void generate();

signals:

	/**
	 * @brief Signal sent after the planet has been created.
	 * This signal is used to fill the plate list widget in the Dock Widget.
	 */
    void planetFinished();
};

#endif // PLANETDOCKWINDOW_H
