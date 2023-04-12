#include <QtGui>
#include <QLabel>
#include <QHBoxLayout>
#include <QGraphicsView>
#include <QButtonGroup>
#include <QAbstractButton>
#include <QToolButton>
#include <QVariant>
#include <QAction>
#include <QToolButton>
#include <QFontComboBox>
#include <QMenuBar>
#include <QMessageBox>
#include <QGraphicsSceneWheelEvent>


#include "robot.h"
#include "mainwindow.h"
#include "item.h"
#include "scene.h"
#include "textitem.h"


MainWindow::MainWindow()
: kalman(new Kalman())
, robot(new Robot())
, navigator(new Navigator(this))
{
    createActions();
    createToolBox();
    createMenus();

    hall = new Hall(this, itemMenu);
    scene = new Scene(itemMenu, this, robot, kalman, hall, navigator);
    scene->setSceneRect(QRectF(0, 0, 40000, 20000));
    connect(scene, SIGNAL(itemInserted(Item *)),
            this, SLOT(itemInserted(Item *)));
    createToolbars();

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(toolBox);
    view = new QGraphicsView(scene);
    layout->addWidget(view);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);

    setCentralWidget(widget);
    setWindowTitle(tr("RoboSim"));
    setUnifiedTitleAndToolBarOnMac(true);

    timer = new QTimer;
    timer2 = new QTimer;
    QObject::connect(timer2, SIGNAL(timeout()), scene, SLOT(process()));
    QObject::connect(timer, SIGNAL(timeout()), scene, SLOT(advance()));
    timer->start(100);
    timer2->start(20);

    QMatrix oldMatrix = view->matrix();
    view->resetMatrix();
    view->translate(oldMatrix.dx(), oldMatrix.dy());
    view->scale(0.1, 0.1);
    actScale = 0.1;
}

void MainWindow::buttonGroupClicked(int id)
{
   int roboter = 0;
   int target = 0;

   foreach(QGraphicsItem *item, scene->items())
   {
      if( qgraphicsitem_cast<Item *>(item) != nullptr )
      {
         if( qgraphicsitem_cast<Item *>(item)->itemType() == Item::Roboter )
         {
            roboter = 1;
         }
         else
         if( qgraphicsitem_cast<Item *>(item)->itemType() == Item::Target )
         {
            target = 1;
         }
      }
   }

   QList<QAbstractButton *> buttons = buttonGroup->buttons();
   foreach (QAbstractButton *button, buttons)
   {
      if( buttonGroup->button(id) != button )
      {
         button->setChecked(false);
      }
   }

   if( (Item::ItemType(id) == Item::Roboter) && (roboter == 1) )
   {
      buttonGroup->button(id)->setChecked(false);
   }
   else
   if( (Item::ItemType(id) == Item::Target) && (target == 1) )
   {
      buttonGroup->button(id)->setChecked(false);
   }
   else
   {
      scene->setItemType(Item::ItemType(id));
      scene->setMode(Scene::InsertItem);
   }
}

void MainWindow::deleteItem()
{
    bool bRemoveLines = false;
    int counter = 0;
    foreach (QGraphicsItem *item, scene->selectedItems())
    {
        if (item->type() == Item::Type)
        {
            if( qgraphicsitem_cast<Item *>(item)->itemType() == Item::Roboter )
            {
                bRemoveLines = true;
                robot->reset();
                counter++;
                navigator->setStartRoom(0);
                navigator->clearNavPoints();
            }
            else
            if( qgraphicsitem_cast<Item *>(item)->itemType() == Item::Target )
            {
                counter++;
                navigator->setFinishRoom(0);
                navigator->clearNavPoints();
            }
        }
        scene->removeItem(item);
    }
    if( counter > 0 )
    {
       naviAction->setEnabled(false);
    }
    if( bRemoveLines == true )
    {
        foreach (QGraphicsItem *item, scene->items())
        {
            if( qgraphicsitem_cast<QGraphicsLineItem*>(item) != nullptr )
            {
                if( dynamic_cast<Door*>(item) == nullptr )
                {
                    scene->removeItem(item);
                }
            }
            if( qgraphicsitem_cast<QGraphicsEllipseItem*>(item) != nullptr )
            {
                scene->removeItem(item);
            }
        }
    }

}

void MainWindow::itemInserted(Item *item)
{
   int counter = 0;
   qDebug() << "Item inserted";
   //scene->setMode(Scene::Mode(Scene::MoveItem));
   scene->setMode(Scene::Mode(Scene::NoItem));
   buttonGroup->button(int(item->itemType()))->setChecked(false);
   QList<QGraphicsItem *> InsItems = scene->items();
   foreach (QGraphicsItem *item, InsItems)
   {
      if( qgraphicsitem_cast<Item *>(item) != nullptr )
      {
         if( qgraphicsitem_cast<Item *>(item)->itemType() == Item::Target )
         {
            counter++;
         }
         else
         if( qgraphicsitem_cast<Item *>(item)->itemType() == Item::Roboter )
         {
            counter++;
         }
      }
   }
   if( counter >= 2 )
   {
      naviAction->setEnabled(true);
   }
}

void MainWindow::sceneScaleChanged(const QString &scale)
{
    view->setTransformationAnchor(QGraphicsView::AnchorViewCenter);

    if( scale.left(scale.indexOf(tr("%"))).compare("Zoom In") == 0)
    {
        actScale += 0.05;
        if(actScale > 1.0) actScale = 1.0;
        QMatrix oldMatrix = view->matrix();
        view->resetMatrix();
        view->translate(oldMatrix.dx(), oldMatrix.dy());
        view->scale(actScale, actScale);
    }
    else
    if( scale.left(scale.indexOf(tr("%"))).compare("Zoom Out") == 0)
    {
        actScale -= 0.05;
        if(actScale < 0.05) actScale = 0.05;
        QMatrix oldMatrix = view->matrix();
        view->resetMatrix();
        view->translate(oldMatrix.dx(), oldMatrix.dy());
        view->scale(actScale, actScale);
    }
    else
    if( scale.left(scale.indexOf(tr("%"))).compare("Fit to Window") == 0)
    {
        //QPointF bottomRight = QPointF(0,0);
        //QPointF topLeft = scene->sceneRect().bottomRight();
        QList<QGraphicsItem *> InsItems = scene->items();
        if(InsItems.size() > 0)
        {
            //QRectF rect = QRectF(18000,8000,4000,4000);
            QRectF rect = hall->getDimension();
            QMatrix oldMatrix = view->matrix();
            view->resetMatrix();
            view->translate(oldMatrix.dx(), oldMatrix.dy());
            view->fitInView(rect,Qt::KeepAspectRatio);
            double newScale = (view->width()-25)/rect.width();
            if( view->height() < view->width() )
            {
                newScale = (view->height()-25)/rect.height();
            }
            actScale = newScale;
        }
    }
    else
    {
        actScale = scale.left(scale.indexOf(tr("%"))).toDouble() / 100.0;
        QMatrix oldMatrix = view->matrix();
        view->resetMatrix();
        view->translate(oldMatrix.dx(), oldMatrix.dy());
        view->scale(actScale, actScale);
    }

    qDebug() << "View size: " << view->size();
    qDebug() << "View geometry: " << view->geometry();
    qDebug() << "View width: " << view->width() << " height: " << view->height();
    qDebug() << "New scale: " << actScale;
}

void MainWindow::robotTriggered()
{
    robot->show();
}

void MainWindow::filterTriggered()
{
    kalman->show();
}

void MainWindow::hallTriggered()
{
    hall->show();
}

void MainWindow::mazeTriggered()
{
    scene->triggerMaze();
}

void MainWindow::trackingTriggered()
{
    static bool trackingOn = false;
    if( trackingOn == false )
    {
        qDebug() << "tracking triggered on";
        kalman->setPositionAvailable(true);
        robot->setSensorPosAvailable(true);
        robot->getSensorCheckBox()->setChecked(true);
        kalman->getPositionCheckBox()->setChecked(true);
        trackingOn = true;
    }
    else
    {
        qDebug() << "tracking triggered off";
        kalman->setPositionAvailable(false);
        robot->setSensorPosAvailable(false);
        robot->getSensorCheckBox()->setChecked(false);
        kalman->getPositionCheckBox()->setChecked(false);
        trackingOn = false;
    }
    //scene->triggerMaze();

}

void MainWindow::naviTriggered()
{
    navigator->show();
}

void MainWindow::acceptHallBox()
{
    scene->createHall(hall->getRooms(), hall->getGrid());
    hall->hide();
}

void MainWindow::updateHallBox()
{
    scene->updateHall();
    hall->hide();
}

void MainWindow::acceptNaviBox()
{
    scene->createNavigation();
    navigator->hide();
}

void MainWindow::planNaviBox()
{
    scene->createNavigation();
}

void MainWindow::rejectNaviBox()
{
    scene->removeNavigation();
}


void MainWindow::about()
{
    QMessageBox::about(this, tr("About Robot Simulator"),
                       tr("The <b>Robot Simulator</b> for simulations of\n "
                          "Kalmanfiltering and Search-Algorithms."));
}

void MainWindow::createToolBox()
{
    buttonGroup = new QButtonGroup;
    buttonGroup->setExclusive(false);
    connect(buttonGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(buttonGroupClicked(int)));
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(createCellWidget(tr("Barrier"), Item::Barrier), 0, 0);
    layout->addWidget(createCellWidget(tr("Target"), Item::Target),0, 1);
    layout->addWidget(createCellWidget(tr("Cover"), Item::Cover), 1, 0);
    layout->addWidget(createCellWidget(tr("Robot"), Item::Roboter), 1, 1);

    layout->setRowStretch(3, 10);
    layout->setColumnStretch(2, 10);

    QWidget *itemWidget = new QWidget;
    itemWidget->setLayout(layout);

    toolBox = new QToolBox;
    toolBox->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Ignored));
    toolBox->setMinimumWidth(itemWidget->sizeHint().width());
    toolBox->addItem(itemWidget, tr("Shapes"));
}

void MainWindow::createActions()
{
    robotAction = new QAction(tr("Robot"), this);
    robotAction->setStatusTip(tr("Robot settings"));
    connect(robotAction, SIGNAL(triggered()), this, SLOT(robotTriggered()));

    filterAction = new QAction(tr("Filter"), this);
    filterAction->setStatusTip(tr("Filter settings"));
    connect(filterAction, SIGNAL(triggered()), this, SLOT(filterTriggered()));

    hallAction = new QAction(tr("Hall"), this);
    hallAction->setStatusTip(tr("Creating Hall"));
    connect(hallAction, SIGNAL(triggered()), this, SLOT(hallTriggered()));

    mazeAction = new QAction(QIcon(":/images/maze.png"), tr("Maze"), this);
    mazeAction->setStatusTip(tr("Creating Maze"));
    connect(mazeAction, SIGNAL(triggered()), this, SLOT(mazeTriggered()));

    trackingAction = new QAction(QIcon(":/images/tracking.png"), tr("Tracking"), this);
    trackingAction->setStatusTip(tr("Tracking"));
    connect(trackingAction, SIGNAL(triggered()), this, SLOT(trackingTriggered()));

    naviAction = new QAction(tr("Navigation"), this);
    naviAction->setStatusTip(tr("Navigation to target"));
    naviAction->setEnabled(false);
    connect(naviAction, SIGNAL(triggered()), this, SLOT(naviTriggered()));

    deleteAction = new QAction(QIcon(":/images/delete.png"), tr("&Delete"), this);
    deleteAction->setShortcut(tr("Delete"));
    deleteAction->setStatusTip(tr("Delete item from diagram"));
    connect(deleteAction, SIGNAL(triggered()),
        this, SLOT(deleteItem()));

    exitAction = new QAction(tr("E&xit"), this);
    exitAction->setShortcuts(QKeySequence::Quit);
    exitAction->setStatusTip(tr("Quit Scenediagram example"));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

    aboutAction = new QAction(tr("A&bout"), this);
    aboutAction->setShortcut(tr("Ctrl+B"));
    connect(aboutAction, SIGNAL(triggered()),
            this, SLOT(about()));
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&Session"));
    fileMenu->addAction(robotAction);
    fileMenu->addAction(filterAction);
    fileMenu->addAction(hallAction);
    fileMenu->addAction(naviAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    itemMenu = menuBar()->addMenu(tr("&Item"));
    itemMenu->addAction(deleteAction);
    itemMenu->addSeparator();

    aboutMenu = menuBar()->addMenu(tr("&Help"));
    aboutMenu->addAction(aboutAction);
}

void MainWindow::createToolbars()
{
    editToolBar = addToolBar(tr("Edit"));
    editToolBar->addAction(deleteAction);
    editToolBar->addAction(mazeAction);
    editToolBar->addAction(trackingAction);

    sceneScaleCombo = new QComboBox;
    QStringList scales;
    //scales << tr("5%") << tr("10%") << tr("15%") << tr("25%") << tr("50%") << tr("75%") << tr("100%") << tr("125%")
    //       << tr("150%") << tr("Zoom In") << tr("Zoom Out") << tr("Fit to Window");
    scales <<tr("Fit to Window") << tr("Zoom In") << tr("Zoom Out") << tr("10%") << tr("15%") << tr("25%") << tr("50%") << tr("75%") << tr("100%") << tr("125%")
           << tr("150%");
    sceneScaleCombo->addItems(scales);
    sceneScaleCombo->setCurrentIndex(3);
    actScale = 0.1;
    connect(sceneScaleCombo, SIGNAL(activated(const QString &)),
            this, SLOT(sceneScaleChanged(const QString &)));

    pointerToolbar = addToolBar(tr("Scale"));
    pointerToolbar->addWidget(sceneScaleCombo);
}


QWidget *MainWindow::createCellWidget(const QString &text, Item::ItemType type)
{

    Item item(type, itemMenu, nullptr, scene);
    QIcon icon(item.image());

    QToolButton *button = new QToolButton;
    button->setIcon(icon);
    button->setIconSize(QSize(50, 50));
    button->setCheckable(true);
    buttonGroup->addButton(button, int(type));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(button, 0, 0, Qt::AlignHCenter);
    layout->addWidget(new QLabel(text), 1, 0, Qt::AlignCenter);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);

    return widget;
}

