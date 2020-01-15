/********************************************************************************
** Form generated from reading UI file 'DotMatrix.ui'
**
** Created by: Qt User Interface Compiler version 5.12.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DOTMATRIX_H
#define UI_DOTMATRIX_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QOpenGLWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DotMatrixClass
{
public:
    QWidget *centralWidget;
    QOpenGLWidget *openGLWidget;
    QMenuBar *menuBar;
    QMenu *menuSettings;
    QMenu *menuFile;

    void setupUi(QMainWindow *DotMatrixClass)
    {
        if (DotMatrixClass->objectName().isEmpty())
            DotMatrixClass->setObjectName(QString::fromUtf8("DotMatrixClass"));
        DotMatrixClass->resize(500, 500);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(DotMatrixClass->sizePolicy().hasHeightForWidth());
        DotMatrixClass->setSizePolicy(sizePolicy);
        DotMatrixClass->setMinimumSize(QSize(500, 500));
        DotMatrixClass->setMaximumSize(QSize(500, 500));
        centralWidget = new QWidget(DotMatrixClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        openGLWidget = new QOpenGLWidget(centralWidget);
        openGLWidget->setObjectName(QString::fromUtf8("openGLWidget"));
        openGLWidget->setGeometry(QRect(0, 0, 500, 500));
        DotMatrixClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(DotMatrixClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 500, 26));
        menuSettings = new QMenu(menuBar);
        menuSettings->setObjectName(QString::fromUtf8("menuSettings"));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        DotMatrixClass->setMenuBar(menuBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuSettings->menuAction());

        retranslateUi(DotMatrixClass);

        QMetaObject::connectSlotsByName(DotMatrixClass);
    } // setupUi

    void retranslateUi(QMainWindow *DotMatrixClass)
    {
        DotMatrixClass->setWindowTitle(QApplication::translate("DotMatrixClass", "Dot Matrix v0.0.0", nullptr));
        menuSettings->setTitle(QApplication::translate("DotMatrixClass", "Settings", nullptr));
        menuFile->setTitle(QApplication::translate("DotMatrixClass", "File", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DotMatrixClass: public Ui_DotMatrixClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DOTMATRIX_H
