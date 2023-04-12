#include <QtGui>
#include <QGraphicsSceneMouseEvent>
#include <QMessageBox>
#include <QGraphicsView>

#include "mainwindow.h"
#include "scene.h"

Scene::Scene(QMenu *itemMenu, QObject *parent, Robot *robot, Kalman *kalman,
             Hall *hall, Navigator *navigator)
    : QGraphicsScene(parent)
{
    myItemMenu = itemMenu;
    //myMode = InsertItem;
    myMode = NoItem;
    myItemType = Item::Target;
    myTextColor = Qt::black;
    myLineColor = Qt::black;
    this->robot = robot;
    this->kalman = kalman;
    this->hall = hall;
    this->navigator = navigator;
    textRoom = nullptr;
    startMaze = false;
    mazeState = InitMaze;
    mazeStateCounter = 0;
    parentWindow = parent;
    startPoint = QPoint(0,0);
}

Scene::~Scene()
{
   foreach (QGraphicsItem *item, items())
   {
       removeItem(item);
       if( qgraphicsitem_cast<QGraphicsTextItem*>(item) != nullptr )
       {
           delete item;
       }
   }
}

void Scene::setLineColor(const QColor &color)
{
    myLineColor = color;
    update();
}

void Scene::setTextColor(const QColor &color)
{
    myTextColor = color;
    if (isItemChange(TextItem::Type)) {
        TextItem *item =
            qgraphicsitem_cast<TextItem *>(selectedItems().first());
        item->setDefaultTextColor(myTextColor);
    }
}


void Scene::setFont(const QFont &font)
{
    myFont = font;

    if (isItemChange(TextItem::Type))
    {
        QGraphicsTextItem *item =
            qgraphicsitem_cast<TextItem *>(selectedItems().first());
        //At this point the selection can change so the first selected item might not be a TextItem
        if (item)
            item->setFont(myFont);
    }
}


void Scene::setMode(Mode mode)
{
   qDebug() << "Mode: " << mode;
   myMode = mode;
}

void Scene::setItemType(Item::ItemType type)
{
   qDebug() << "ItemType: " << type;
   myItemType = type;
}

void Scene::createHall(int rooms, int grid)
{
    // Delete all first
    foreach (QGraphicsItem *item, items())
    {
        removeItem(item);
        if( qgraphicsitem_cast<QGraphicsTextItem*>(item) != nullptr )
        {
            delete item;
        }
    }

    // Hall dimension
    QRectF rect = sceneRect();
    qDebug() << "Scene rect: " << rect;
    QPointF offset;
    rect.setHeight( rect.width()/10 );
    rect.setWidth( rect.width()/10 );
    qDebug() << "Hall rect: " << rect;
    offset.setX((sceneRect().width()/2)-(rect.width()/2));
    offset.setY((sceneRect().height()/2)-(rect.height()/2));
    qDebug() << "Hall offset: " << offset;
    //hall->generate(rect, offset );

    // Generate all rooms
    hall->generateMaze(rect, offset );

    // Add all rooms
    for( int i=0; i<rooms; i++ )
    {
        Room *room = hall->getRoom(i+1);
        room->setZValue(-0.1);
        room->setBrush(Qt::white);
        room->setPen(QPen(Qt::blue, 20, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
        addItem(room);
    }

    // Not needed because all doors are closed
    for( int i=0; i<hall->getDoorCount(); i++ )
    {
        Door *door = hall->getDoor(i);
        if( door->open == true )
        {
            door->setZValue(0.0);
            door->setPen(QPen(Qt::white, 22, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
            addItem(door);
        }
    }

    // Add room numbers
    for( int i=0; i<rooms; i++ )
    {
        Room *room = hall->getRoom(i+1);

        textRoom = new QGraphicsTextItem(QString("%1").arg(int(room->getNo())));
        textRoom->setFont(QFont("Courier", 50));
        textRoom->setZValue(-0.1);
        textRoom->setDefaultTextColor(Qt::gray);
        QPointF textPos;
        textPos.setX(room->get(0).x()+30);
        textPos.setY(room->get(2).y()+30);
        textRoom->setPos(textPos);
        addItem(textRoom);
    }

    // Tell navigator
    navigator->setRoomList(hall->getRoom(1), rooms);
    navigator->setDoorList(hall->getDoorList());

}

void Scene::updateHall()
{
   // Remove all doors (room without doors)
   foreach (QGraphicsItem *item, items())
   {
       if( qgraphicsitem_cast<Door*>(item) != nullptr )
       {
           removeItem(item);
       }
   }

   // Maze algorithm by opening doors
   hall->update();

   // Create open doors
   for( int i=0; i<hall->getDoorCount(); i++ )
   {
       Door *door = hall->getDoor(i);
       if( door->open == true )
       {
          door->setZValue(-0.05);
          door->setPen(QPen(Qt::white, 22, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
          addItem(door);
       }
   }

   // Tell navigator
   navigator->setDoorList(hall->getDoorList());
}

void Scene::createNavigation()
{
    QPen naviPen = QPen(Qt::gray, 8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);

    foreach (QGraphicsItem *item, items())
    {
        //if( qgraphicsitem_cast<QGraphicsEllipseItem*>(item) != nullptr )
       if( qgraphicsitem_cast<QGraphicsLineItem*>(item) != nullptr )
        {
            //QPen pen = qgraphicsitem_cast<QGraphicsEllipseItem*>(item)->pen();
            QPen pen = qgraphicsitem_cast<QGraphicsLineItem*>(item)->pen();
            if( pen.color() == Qt::gray )
            {
              removeItem(item);
            }
        }
    }

    QVector<QPointF>* navilist = navigator->getNaviList();
    if( navilist->size() > 0 )
    {
       for( int i = 0; i < (navilist->size()-1); i++ )
       {
          //addEllipse(navilist->at(i).x()-8, navilist->at(i).y()-8, 16, 16, naviPen);
          addLine(navilist->at(i).x(), navilist->at(i).y(), navilist->at(i+1).x(), navilist->at(i+1).y(), naviPen);
       }
    }
}

void Scene::removeNavigation()
{
    foreach (QGraphicsItem *item, items())
    {
        //if( qgraphicsitem_cast<QGraphicsEllipseItem*>(item) != nullptr )
        if( qgraphicsitem_cast<QGraphicsLineItem*>(item) != nullptr )
        {
            //QPen pen = qgraphicsitem_cast<QGraphicsEllipseItem*>(item)->pen();
            QPen pen = qgraphicsitem_cast<QGraphicsLineItem*>(item)->pen();
            if( pen.color() == Qt::gray )
            {
              removeItem(item);
            }
        }
    }
}

void Scene::editorLostFocus(TextItem *item)
{
    QTextCursor cursor = item->textCursor();
    cursor.clearSelection();
    item->setTextCursor(cursor);

    if (item->toPlainText().isEmpty()) {
        removeItem(item);
        item->deleteLater();
    }
}

void Scene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (mouseEvent->button() != Qt::LeftButton)
        return;

    QGraphicsScene::mousePressEvent(mouseEvent);

    Item *item;
    switch (myMode)
    {
        case InsertItem:
        {
            bool bAllowed = true;
            if( myItemType == Item::Roboter )
            {
                foreach(QGraphicsItem *item, items())
                {
                    if( qgraphicsitem_cast<Item *>(item) != nullptr )
                    {
                        if( qgraphicsitem_cast<Item *>(item)->itemType() == Item::Roboter )
                        {
                            bAllowed = false;
                            break;
                        }
                    }
                }
            }
            else
            if( myItemType == Item::Target )
            {
                foreach(QGraphicsItem *item, items())
                {
                    if( qgraphicsitem_cast<Item *>(item) != nullptr )
                    {
                        if( qgraphicsitem_cast<Item *>(item)->itemType() == Item::Target )
                        {
                            bAllowed = false;
                            break;
                        }
                    }
                }
            }
            if( bAllowed == true )
            {
                double zValue = 0.1;
                if( myItemType == Item::Roboter )
                {
                    item = new Item(myItemType, myItemMenu, nullptr, this, robot, kalman, navigator);
                    zValue = 0.0;
                    navigator->setStartPoint(mouseEvent->scenePos());
                    kalman->inactiveProcessRange();
                }
                else
                if( myItemType == Item::Target )
                {
                    item = new Item(myItemType, myItemMenu, nullptr, this, robot, nullptr, navigator);
                    zValue = -0.1;
                    navigator->setFinishPoint(mouseEvent->scenePos());
                }
                else
                {
                    item = new Item(myItemType, myItemMenu, nullptr,this);
                }
                item->setZValue(zValue);
                //item->setBrush(myItemColor);
                addItem(item);
                item->setPos(mouseEvent->scenePos());
                //qDebug() << "Pos: " << mouseEvent->scenePos();
                emit itemInserted(item);
            }
            else
            {
                QMessageBox::information(nullptr, "Info", "Only one Robot or Target allowed", QMessageBox::Ok);
            }
            break;
        }

        case MoveItem:
            if( selectedItems().size() == 0 )
            {
                myMode = NoItem;
                startPoint = mouseEvent->scenePos();
                dynamic_cast<MainWindow*>(parentWindow)->getView()->setDragMode(QGraphicsView::ScrollHandDrag);
                qDebug() << "Scroll Hand Drag";
            }
            break;

        case NoItem:
            if( selectedItems().size() == 0 )
            {
                startPoint = mouseEvent->scenePos();
                dynamic_cast<MainWindow*>(parentWindow)->getView()->setDragMode(QGraphicsView::ScrollHandDrag);
                qDebug() << "Scroll Hand Drag";
            }
            else
            {
                foreach(QGraphicsItem *item, selectedItems())
                {
                    if( qgraphicsitem_cast<Item *>(item) != nullptr )
                    {
                        if( qgraphicsitem_cast<Item *>(item)->itemType() == Item::Roboter )
                        {
                            myMode = MoveItem;
                            break;
                        }
                        else
                        if( qgraphicsitem_cast<Item *>(item)->itemType() == Item::Target )
                        {
                            myMode = MoveItem;
                            break;
                        }
                    }
                }
            }
            break;

        default:
            break;
    }
    qDebug() << "Scene Item Mode: " << myMode;

}

#if 0
        view->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
        //QMatrix oldMatrix = view->matrix();
        //view->resetMatrix();
        //view->translate(oldMatrix.dx(), oldMatrix.dy());
        //QPointF oldPos = view->mapToScene(originPos.x(),originPos.y());
        //view->scale(actScale, actScale);
        //QPointF newPos = view->mapToScene(originPos.x()+100, originPos.y()+100);
        //QPointF deltaPos = newPos - oldPos;
        view->translate(100, 100);
#endif

void Scene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (mouseEvent->buttons() == Qt::LeftButton)
    {
        if( robot->getMode() == Robot::DragDrop )
        {
            //qDebug() << "selected items: " << selectedItems().size();
            if( selectedItems().size() == 0 )
            {
                myMode = NoItem;
            }
            else
            {
                foreach (QGraphicsItem *item, selectedItems())
                {
                    if( qgraphicsitem_cast<Item *>(item) != nullptr )
                    {
                        if( qgraphicsitem_cast<Item *>(item)->itemType() == Item::Roboter )
                        {
                            //if (myMode == MoveItem) myItemType = Item::Roboter;
                            myItemType = Item::Roboter;
                            myMode = MoveItem;
                        }
                        else
                        if( qgraphicsitem_cast<Item *>(item)->itemType() == Item::Target )
                        {
                            //if (myMode == MoveItem) myItemType = Item::Target;
                            myItemType = Item::Target;
                            myMode = MoveItem;
                        }
                    }
                }

                if (myMode == MoveItem)
                {
                    if( myItemType == Item::Roboter )
                    {
                       //qDebug() << "Start Pos: " << mouseEvent->scenePos();
                       navigator->setStartPoint(mouseEvent->scenePos());
                       foreach (QGraphicsItem *item, items())
                       {
                          if( qgraphicsitem_cast<QGraphicsLineItem*>(item) != nullptr )
                          {
                             if( dynamic_cast<Door*>(item) == nullptr )
                             {
                                removeItem(item);
                             }
                          }
                       }
                    }
                    else
                    if( myItemType == Item::Target )
                    {
                       //qDebug() << "Target Pos: " << mouseEvent->scenePos();
                       navigator->setFinishPoint(mouseEvent->scenePos());
                    }
                }
            }
        }
    }
    QGraphicsScene::mouseMoveEvent(mouseEvent);
}

void Scene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    dynamic_cast<MainWindow*>(parentWindow)->getView()->setDragMode(QGraphicsView::NoDrag);
    qDebug() << "No Drag";

    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}


void Scene::keyPressEvent(QKeyEvent *event)
{
    //qDebug() << "Key pressed: " << event->key();
    if( event->key() == Qt::Key_4 )
    {
        robot->setRotation(-1);
    }
    else
    if( event->key() == Qt::Key_6 )
    {
        robot->setRotation(1);
    }
    else
    if( event->key() == Qt::Key_8 )
    {
        kalman->activeProcessRange();
        robot->setDrive(1);
    }
    else
    if( event->key() == Qt::Key_2 )
    {
        kalman->inactiveProcessRange();
        robot->setDrive(-1);
    }

    QGraphicsScene::keyPressEvent(event);
}


void Scene::keyReleaseEvent(QKeyEvent *event)
{
    QGraphicsScene::keyReleaseEvent(event);
}


void Scene::wheelEvent(QGraphicsSceneWheelEvent *wheelEvent)
{
    if (wheelEvent->modifiers().testFlag(Qt::ControlModifier))
    {
        if( wheelEvent->delta() < 0 )
        {
            dynamic_cast<MainWindow*>(parentWindow)->getView()->scale(0.9, 0.9);
        }
        else
        {
            dynamic_cast<MainWindow*>(parentWindow)->getView()->scale(1.1, 1.1);
        }
        wheelEvent->accept();
    }
}


bool Scene::isItemChange(int type)
{
    foreach (QGraphicsItem *item, selectedItems()) {
        if (item->type() == type)
            return true;
    }
    return false;
}

void Scene::process()
{
    if( startMaze == true )
    {
        switch(mazeState)
        {
            case InitMaze:
            {
                // Delete all first
                foreach (QGraphicsItem *item, items())
                {
                    removeItem(item);
                    if( qgraphicsitem_cast<QGraphicsTextItem*>(item) != nullptr )
                    {
                        delete item;
                    }
                }
                qDebug() << "Generate Maze";
                mazeState = GenerateMaze;
            }
            break;

            case GenerateMaze:
            {
                // Hall dimension
                QRectF rect = sceneRect();
                qDebug() << "Scene rect: " << rect;
                QPointF offset;
                rect.setHeight( rect.width()/10 );
                rect.setWidth( rect.width()/10 );
                qDebug() << "Hall rect: " << rect;
                offset.setX((sceneRect().width()/2)-(rect.width()/2));
                offset.setY((sceneRect().height()/2)-(rect.height()/2));
                qDebug() << "Hall offset: " << offset;
                // Generate all rooms
                hall->generateMaze(rect, offset );
                qDebug() << "Draw room grid";
                mazeState = DrawRoomGrid;
            }
            break;

            case DrawRoomGrid:
            {
                // Add all rooms
                for( int i=0; i<hall->getRooms(); i++ )
                {
                    Room *room = hall->getRoom(i+1);
                    room->setZValue(-0.1);
                    room->setBrush(Qt::white);
                    room->setPen(QPen(Qt::blue, 20, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
                    addItem(room);
                }
                qDebug() << "Draw room numbers";
                mazeState = DrawRoomNumbers;
                /*
                if( mazeStateCounter < hall->getRooms() )
                {
                    Room *room = hall->getRoom(mazeStateCounter+1);
                    room->setZValue(-0.1);
                    room->setBrush(Qt::white);
                    room->setPen(QPen(Qt::blue, 20, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
                    addItem(room);
                    mazeStateCounter++;
                }
                else
                {
                    qDebug() << "Maze finished";
                    mazeState = MazeFinished;
                }
                */
            }
            break;

            case DrawRoomNumbers:
            {
                // Add room numbers
                for( int i=0; i<hall->getRooms(); i++ )
                {
                    Room *room = hall->getRoom(i+1);

                    textRoom = new QGraphicsTextItem(QString("%1").arg(int(room->getNo())));
                    textRoom->setFont(QFont("Courier", 50));
                    textRoom->setZValue(-0.1);
                    textRoom->setDefaultTextColor(Qt::gray);
                    QPointF textPos;
                    textPos.setX(room->get(0).x()+30);
                    textPos.setY(room->get(2).y()+30);
                    textRoom->setPos(textPos);
                    addItem(textRoom);
                }
                qDebug() << "Update Maze";
                mazeState = UpdateMaze;
                mazeStateCounter = 0;
            }
            break;

            case UpdateMaze:
            {
                //hall->update();
                if( hall->openDoorsMaze() == 1 )
                {
                    // Create open doors
                    for( int i=0; i<hall->getDoorCount(); i++ )
                    {
                        Door *door = hall->getDoor(i);
                        if( door->open == true )
                        {
                            bool addedDoor = false;
                            foreach (QGraphicsItem *item, items())
                            {
                                if( qgraphicsitem_cast<QGraphicsLineItem*>(item) != nullptr )
                                {
                                    if( dynamic_cast<Door*>(item) == door )
                                    {
                                        //Door already drawn
                                        addedDoor = true;
                                    }
                                }
                            }
                            if( addedDoor == false )
                            {
                                door->setZValue(-0.05);
                                door->setPen(QPen(Qt::white, 22, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
                                addItem(door);
                            }
                        }
                    }
                }
                else
                {
                    qDebug() << "Maze finished";
                    mazeState = MazeFinished;
                }
            }
            break;

            case MazeFinished:
                // Tell navigator
                navigator->setRoomList(hall->getRoom(1), hall->getRooms());
                navigator->setDoorList(hall->getDoorList());
                mazeState = InitMaze;
                startMaze = false;
                break;

            default:
                break;
        }
    }
}

