#include "PlanetDockWidget.h"
#include <QFileDialog>
#include <QComboBox>

using namespace std;
PlanetDockWidget::PlanetDockWidget(PlanetViewer * _viewer, QWidget * parent ):QDockWidget(parent)
{
    viewer = _viewer;

    QWidget * contents = new QWidget();

    QVBoxLayout * contentLayout = new QVBoxLayout(contents);


    contentLayout->addStretch(0);
    this->setWidget(contents);
}
