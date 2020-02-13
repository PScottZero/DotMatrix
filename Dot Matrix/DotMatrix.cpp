#include "DotMatrix.h"

DotMatrix::DotMatrix(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	scene = new QGraphicsScene(this);
	ui.graphicsView->setScene(scene);
	ui.graphicsView->setSceneRect(QRect(0, 0, 160, 160));
	ui.graphicsView->scale(4, 4);
}
