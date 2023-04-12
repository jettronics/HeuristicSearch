/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QToolBox>
#include <QToolBar>
#include <QComboBox>
#include <QGraphicsView>

#include "robot.h"
#include "kalman.h"
#include "hall.h"
#include "item.h"
#include "navigator.h"

class Scene;

QT_BEGIN_NAMESPACE
class QAction;
class QSpinBox;
class QFontComboBox;
class QButtonGroup;
class QLineEdit;
class QGraphicsTextItem;
class QFont;
class QToolButton;
class QAbstractButton;
class QTimer;
QT_END_NAMESPACE


//! [0]
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
   MainWindow();

   QGraphicsView* getView() {return view;}

private slots:
    void buttonGroupClicked(int id);
    void deleteItem();
    void itemInserted(Item *item);
    void sceneScaleChanged(const QString &scale);
    void about();
    void robotTriggered();
    void filterTriggered();
    void hallTriggered();
    void mazeTriggered();
    void trackingTriggered();
    void naviTriggered();
    void acceptHallBox();
    void updateHallBox();
    void acceptNaviBox();
    void planNaviBox();
    void rejectNaviBox();


private:
    void createToolBox();
    void createActions();
    void createMenus();
    void createToolbars();

    QWidget *createCellWidget(const QString &text,
                              Item::ItemType type);

    Scene *scene;
    QGraphicsView *view;

    QAction *exitAction;
    QAction *addAction;
    QAction *deleteAction;

    QAction *aboutAction;

    QAction *robotAction;
    QAction *filterAction;
    QAction *hallAction;
    QAction *mazeAction;
    QAction *trackingAction;
    QAction *naviAction;

    QMenu *fileMenu;
    QMenu *itemMenu;
    QMenu *aboutMenu;

    QToolBar *editToolBar;
    QToolBar *pointerToolbar;

    QComboBox *sceneScaleCombo;

    QToolBox *toolBox;
    QButtonGroup *buttonGroup;

    Kalman *kalman;
    Robot *robot;
    Hall *hall;
    Navigator *navigator;

    QTimer *timer;
    QTimer *timer2;

    double actScale;
};
//! [0]

#endif
