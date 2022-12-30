#include "Window.hpp"
#include <QFileDialog>
Window::Window ()
{
	if (this->objectName ().isEmpty ())
		this->setObjectName ("mainWindow");
	this->resize (1280, 720);

	viewer = new PlanetViewer (this);

    QWidget *planetWidget = new QWidget (this);
    QGridLayout *gridLayout = new QGridLayout (planetWidget);

	gridLayout->addWidget (viewer, 0, 1, 1, 1);

	/**Actions**/
	QAction *actionSaveOFF = new QAction ("Save Planet as .off", this);
	QAction *actionSaveOBJ = new QAction ("Save Planet as .obj", this);
	QAction *actionHelp = new QAction ("Help", this);


	/**Menus**/
	QMenu *menuFile = new QMenu ("File", this);
	QMenu *menuAbout = new QMenu ("About", this);

	menuFile->addAction (actionSaveOFF);
	menuFile->addAction (actionSaveOBJ);

	menuAbout->addAction (actionHelp);

	connect (actionSaveOFF, SIGNAL(triggered()), viewer, SLOT(savePlanetOff()));
	connect (actionSaveOBJ, SIGNAL(triggered()), viewer, SLOT(savePlanetObj()));

	connect (actionHelp, SIGNAL(triggered()), viewer, SLOT(help()));

	QGroupBox *viewerGroupBox = new QGroupBox ("Planet Generator", this);
	QHBoxLayout *viewerLayout = new QHBoxLayout (viewerGroupBox);
    viewerLayout->addWidget (planetWidget);

	madDockWidget = new PlanetDockWidget (viewer, this);

	this->addDockWidget (Qt::RightDockWidgetArea, madDockWidget);

	this->setCentralWidget (viewerGroupBox);

	QMenuBar *menubar = new QMenuBar (this);

	menubar->addAction (menuFile->menuAction ());
	menubar->addAction (menuAbout->menuAction ());

	this->setMenuBar (menubar);

	statusbar = new QStatusBar (this);

	this->setStatusBar (statusbar);

	this->setWindowTitle ("Planet Generator");

    statusbar->showMessage(tr("Application Ready"), 5000);
}

void Window::printMessage(const char* message)
{
    statusbar->showMessage(tr(message),10000);
}
