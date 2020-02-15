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
#include <QtGui/QIcon>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include "GB_PPU.h"

QT_BEGIN_NAMESPACE

class Ui_DotMatrixClass
{
public:
    QAction *actionQuit;
    QAction *actionx1;
    QAction *actionx2;
    QAction *actionx3;
    QAction *actionx4;
    GB_PPU *ppu;

    void setupUi(QMainWindow *DotMatrixClass)
    {
        if (DotMatrixClass->objectName().isEmpty())
            DotMatrixClass->setObjectName(QString::fromUtf8("DotMatrixClass"));
        DotMatrixClass->resize(640, 576);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(DotMatrixClass->sizePolicy().hasHeightForWidth());
        DotMatrixClass->setSizePolicy(sizePolicy);
        DotMatrixClass->setMinimumSize(QSize(0, 0));
        DotMatrixClass->setMaximumSize(QSize(10000, 10000));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/DotMatrix/dotmatrix.ico"), QSize(), QIcon::Normal, QIcon::On);
        DotMatrixClass->setWindowIcon(icon);
        actionQuit = new QAction(DotMatrixClass);
        actionQuit->setObjectName(QString::fromUtf8("actionQuit"));
        actionx1 = new QAction(DotMatrixClass);
        actionx1->setObjectName(QString::fromUtf8("actionx1"));
        actionx2 = new QAction(DotMatrixClass);
        actionx2->setObjectName(QString::fromUtf8("actionx2"));
        actionx3 = new QAction(DotMatrixClass);
        actionx3->setObjectName(QString::fromUtf8("actionx3"));
        actionx4 = new QAction(DotMatrixClass);
        actionx4->setObjectName(QString::fromUtf8("actionx4"));
        ppu = new GB_PPU(DotMatrixClass);
        ppu->setObjectName(QString::fromUtf8("ppu"));
        DotMatrixClass->setCentralWidget(ppu);

        retranslateUi(DotMatrixClass);

        QMetaObject::connectSlotsByName(DotMatrixClass);
    } // setupUi

    void retranslateUi(QMainWindow *DotMatrixClass)
    {
        DotMatrixClass->setWindowTitle(QApplication::translate("DotMatrixClass", "Dot Matrix v0.0.0", nullptr));
        actionQuit->setText(QApplication::translate("DotMatrixClass", "Quit", nullptr));
        actionx1->setText(QApplication::translate("DotMatrixClass", "x1", nullptr));
        actionx2->setText(QApplication::translate("DotMatrixClass", "x2", nullptr));
        actionx3->setText(QApplication::translate("DotMatrixClass", "x3", nullptr));
        actionx4->setText(QApplication::translate("DotMatrixClass", "x4", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DotMatrixClass: public Ui_DotMatrixClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DOTMATRIX_H
