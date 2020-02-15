#pragma once

#include <QtWidgets/QMainWindow>
#include <QGraphicsScene>
#include "ui_DotMatrix.h"

class DotMatrix : public QMainWindow
{
	Q_OBJECT

public:
	DotMatrix(QWidget* parent = Q_NULLPTR);
	Ui::DotMatrixClass ui;
};