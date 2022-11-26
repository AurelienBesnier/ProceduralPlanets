#include "Window.hpp"
#include <QFileDialog>
Window::Window ()
{
	if (this->objectName ().isEmpty ())
		this->setObjectName ("mainWindow");
	this->resize (1280, 720);

	viewer = new PlanetViewer (this);

	QWidget *tectureWidget = new QWidget (this);
	QGridLayout *gridLayout = new QGridLayout (tectureWidget);

	gridLayout->addWidget (viewer, 0, 1, 1, 1);

	QAction *actionSaveOFF = new QAction ("Save Planet as .off", this);
	QAction *actionSaveOBJ = new QAction ("Save Planet as .obj", this);

	QMenu *menuFile = new QMenu ("File", this);

	menuFile->addAction (actionSaveOFF);
	menuFile->addAction (actionSaveOBJ);

	connect (actionSaveOFF, SIGNAL(triggered()), viewer, SLOT(savePlanetOff()));
	connect (actionSaveOBJ, SIGNAL(triggered()), viewer, SLOT(savePlanetObj()));

	QGroupBox *viewerGroupBox = new QGroupBox ("Planet Generator", this);
	QHBoxLayout *viewerLayout = new QHBoxLayout (viewerGroupBox);
	viewerLayout->addWidget (tectureWidget);

	madDockWidget = new PlanetDockWidget (viewer, this);

	this->addDockWidget (Qt::RightDockWidgetArea, madDockWidget);

	this->setCentralWidget (viewerGroupBox);

	QMenuBar *menubar = new QMenuBar (this);

	menubar->addAction (menuFile->menuAction ());

	this->setMenuBar (menubar);

	statusbar = new QStatusBar (this);

	this->setStatusBar (statusbar);

	this->setWindowTitle ("Planet Generator");
}

