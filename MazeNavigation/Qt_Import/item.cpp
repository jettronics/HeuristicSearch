#include <QtGui>
#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>

#include "hall.h"
#include "robot.h"
#include "kalman.h"
#include "door.h"
#include "room.h"
#include "item.h"
#include "scene.h"


Item::Item(ItemType itemType, QMenu *contextMenu,
             QGraphicsItem *parent, QGraphicsScene *scene,
             Robot *robot, Kalman *kalman, Navigator *navigator)
    : QGraphicsPolygonItem(parent),
      scene(scene),
      roomNo(0),
      roomcoll(nullptr),
      room(nullptr),
      navigator(navigator)
{
    myItemType = itemType;
    myContextMenu = contextMenu;

    QPainterPath path;
    QBrush filling = QBrush(Qt::gray);
    switch (myItemType) {
        case Roboter:
            //path.addEllipse(-50, -50, 100, 100);
            //myPolygon = path.toFillPolygon();
            //myPolygon << QPointF(-50, 75) << QPointF(0, -75)
            //          << QPointF(50, 75) << QPointF(-50, 75);
            myPolygon << QPointF(-25, 30) << QPointF(-25, -15) << QPointF(-15, -30) << QPointF(15, -30)
                      << QPointF(25, -15) << QPointF(25, 30) << QPointF(-25, 30);

            if( (robot != nullptr) && (kalman != nullptr) )
            {
                this->robot = robot;
                this->kalman = kalman;
                QObject::connect(this->robot, SIGNAL(changedSensorRange(double)),
                                 this->kalman, SLOT(setSensorRange(double)));
            }
            break;
        case Target:
            /*myPolygon << QPointF(-100, 0) << QPointF(0, 100)
                      << QPointF(100, 0) << QPointF(0, -100)
                      << QPointF(-100, 0);
            */
            myPolygon << QPointF(-30, 50) << QPointF(-30, -50)
                              << QPointF(30, -50) << QPointF(30, -10)
                              << QPointF(-30, -10);
            this->robot = robot;
            break;
        case Barrier:
           filling = QBrush(Qt::gray);
           myPolygon << QPointF(-50, -50) << QPointF(50, -50)
                      << QPointF(50, 50) << QPointF(-50, 50)
                      << QPointF(-50, -50);
            break;
        case Cover:
        default:
            filling = QBrush(Qt::darkGray);
            myPolygon << QPointF(-100, -75) << QPointF(100, -75)
                      << QPointF(100, 75) << QPointF(-100, 75)
                      << QPointF(-100, -75);
            break;
    }
    setPolygon(myPolygon);
    setBrush(filling);
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
}


QPixmap Item::image() const
{
    QPixmap pixmap(250, 250);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setPen(QPen(Qt::black, 8));
    painter.translate(125, 125);
    painter.drawPolyline(myPolygon);

    return pixmap;
}

void Item::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    scene->clearSelection();
    setSelected(true);
    myContextMenu->exec(event->screenPos());
}


QVariant Item::itemChange(GraphicsItemChange change,
                     const QVariant &value)
{
    if (change == QGraphicsItem::ItemPositionChange)
    {

    }

    return value;
}


void Item::advance(int phase)
{
    if(phase == 0) return;

    if( myItemType == Roboter )
    {
        QPolygonF colPoly;
        QPainterPath path;
        //QVector<Room*> rooms;
        Door *door = nullptr;

        //rooms.clear();
        bool bCollision = false;

        foreach (QGraphicsItem *item, scene->items())
        {
            if( qgraphicsitem_cast<QGraphicsLineItem*>(item) != nullptr )
            {
                QPen pen = qgraphicsitem_cast<QGraphicsLineItem*>(item)->pen();
                if( pen.color() == Qt::red )
                {
#if 0
                    QList<QGraphicsItem *> collisionItem = scene->collidingItems(item, Qt::IntersectsItemShape);
                    foreach (QGraphicsItem *itemColl, collisionItem)
                    {
                        if( qgraphicsitem_cast<Room*>(itemColl) != nullptr )
                        {
                            if( qgraphicsitem_cast<Room*>(itemColl)->getNo() != room->getNo() )
                            {
                                //bCollision = true;
                                qDebug() << "Distance Sensor Room collision warning ";
                            }
                        }
                    }
#endif
                    scene->removeItem(item);
                }
            }
            else
            if( qgraphicsitem_cast<QGraphicsEllipseItem*>(item) != nullptr )
            {
                scene->removeItem(item);
            }
        }

        // get distance sensor from robot
        //Robot *robot = dynamic_cast<Robot*>(robot);
        if( robot != nullptr )
        {
#if 0
            QVector<QPointF> distanceSensorPoints = robot->distanceSensor();
            QPen distSensorPen = QPen(Qt::red, 4, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
            for( int i=0; i<(distanceSensorPoints.size()-1); i++ )
            {
                scene->addLine(distanceSensorPoints.at(i).x(), distanceSensorPoints.at(i).y(), distanceSensorPoints.at(i+1).x(), distanceSensorPoints.at(i+1).y(), distSensorPen);
            }
#endif
        }


        QList<QGraphicsItem *> itemInRoom = scene->collidingItems(this, Qt::ContainsItemBoundingRect);
        foreach (QGraphicsItem *iteminroom, itemInRoom)
        {
            if( qgraphicsitem_cast<Room*>(iteminroom) != nullptr )
            {
                if( room != qgraphicsitem_cast<Room*>(iteminroom) )
                {
                    room = qgraphicsitem_cast<Room*>(iteminroom);
                    roomcoll = room;
                    roomNo = room->getNo();
                    navigator->setStartRoom(room->getNo());
                    qDebug() << "Robot in room: " << qgraphicsitem_cast<Room*>(iteminroom)->getNo();
                }
            }
        }

        QList<QGraphicsItem *> collisionItem = scene->collidingItems(this, Qt::IntersectsItemBoundingRect);
        foreach (QGraphicsItem *item, collisionItem)
        {
            if (item == this)
                continue;

            if( qgraphicsitem_cast<Item*>(item) != nullptr )
            {
                if( qgraphicsitem_cast<Item*>(item)->itemType() == Item::Barrier )
                {
                   QPointF collPoint = QPointF(0,0);

                   if( ((pos().rx()+50) > (item->pos().rx()-50)) &&
                        (pos().rx() < item->pos().rx()) &&
                        (pos().ry() < item->pos().ry()+75) &&
                        (pos().ry() > item->pos().ry()-75) )
                   {
                      qDebug() << "Right collision";
                      collPoint.setX(1);
                   }
                   else
                   if( ((pos().rx()-50) < (item->pos().rx()+50)) &&
                        (pos().rx() > item->pos().rx()) &&
                        (pos().ry() < item->pos().ry()+75) &&
                        (pos().ry() > item->pos().ry()-75) )
                   {
                      qDebug() << "Left collision";
                      collPoint.setX(-1);
                   }
                   else
                   if( ((pos().ry()+50) > (item->pos().ry()-50)) &&
                        (pos().ry() < item->pos().ry()) &&
                        (pos().rx() < item->pos().rx()+75) &&
                        (pos().rx() > item->pos().rx()-75) )
                   {
                      qDebug() << "Down collision";
                      collPoint.setY(1);
                   }
                   else
                   if( ((pos().ry()-50) < (item->pos().ry()+50)) &&
                        (pos().ry() > item->pos().ry()) &&
                        (pos().rx() < item->pos().rx()+75) &&
                        (pos().rx() > item->pos().rx()-75) )
                   {
                      qDebug() << "Up collision";
                      collPoint.setY(-1);
                   }

                   //Robot *robot = dynamic_cast<Robot*>(settings);
                   if( robot != nullptr)
                   {
                      // Tell navigation about collision
                      //robot->setCollision(item->pos());
                      //robot->setCollision(collPoint);
                   }
                }
            }
            else
            if( qgraphicsitem_cast<Room*>(item) != nullptr )
            {
                if( qgraphicsitem_cast<Room*>(item)->getNo() != room->getNo() )
                {
                    bCollision = true;
                    //qDebug() << "Room collision";
                }
            }
            else
            if( qgraphicsitem_cast<Door*>(item) != nullptr )
            {
                door = qgraphicsitem_cast<Door*>(item);
                //bCollision = true;
                //qDebug() << "Door collision";
            }
        }

        //bool bCollision = true;
        QString strCollision = "";

        if( door != nullptr )
        {
            if( door->open == true )
            {
                bCollision = false;

                if( (int)door->line().p1().rx() == (int)door->line().p2().rx() )
                {
                   // Vertical door
                   if( door->line().p1().ry() < door->line().p2().ry())
                   {
                       // p1 up

                       if( (pos().ry()+fabs(polygon().first().rx())) > (door->line().p2().ry()+10) )
                       {
                           //qDebug() << "Vertical door down collision: " << pos().ry() << ", " << door->line().p2().ry();
                           //strCollision.append(" door down");
                           bCollision = true;
                       }
                       else
                       if( (pos().ry()-fabs(polygon().first().rx())) < (door->line().p1().ry()-10) )
                       {
                           //qDebug() << "Vertical door up collision: " << pos().ry() << ", " << door->line().p1().ry();
                           //strCollision.append(" door up");
                           bCollision = true;
                       }
                   }
                   else
                   {
                       // p2 up
                       if( (pos().ry()+fabs(polygon().first().rx())) > (door->line().p1().ry()+10) )
                       {
                           //qDebug() << "Vertical door down collision: " << pos().ry() << ", " << door->line().p1().ry();
                           //strCollision.append(" door down");
                           bCollision = true;
                       }
                       else
                       if( (pos().ry()-fabs(polygon().first().rx())) < (door->line().p2().ry()-10) )
                       {
                           //qDebug() << "Vertical door up collision: " << pos().ry() << ", " << door->line().p2().ry();
                           //strCollision.append(" door up");
                           bCollision = true;
                       }
                   }
                }
                else
                {
                   // Horizontal door
                   if( door->line().p1().rx() < door->line().p2().rx())
                   {
                       // p1 left
                       if( (pos().rx()-fabs(polygon().first().rx())) < (door->line().p1().rx()-10) )
                       {
                           //qDebug() << "Horizontal door left collision: " << pos().rx() << ", " << door->line().p1().rx();
                           //strCollision.append(" door left ");
                           bCollision = true;
                       }
                       else
                       if( (pos().rx()+fabs(polygon().first().rx())) > (door->line().p2().rx()+10) )
                       {
                           //qDebug() << "Horizontal door right collision: " << pos().rx() << ", " << door->line().p2().rx();
                           //strCollision.append(" door right ");
                           bCollision = true;
                       }
                   }
                   else
                   {
                       // p2 left
                       if( (pos().rx()-fabs(polygon().first().rx())) < (door->line().p2().rx()-10) )
                       {
                           //qDebug() << "Horizontal door left collision: " << pos().rx() << ", " << door->line().p2().rx();
                           //strCollision.append(" door left ");
                           bCollision = true;
                       }
                       else
                       if( (pos().rx()+fabs(polygon().first().rx())) > (door->line().p1().rx()+10) )
                       {
                           //qDebug() << "Horizontal door right collision: " << pos().rx() << ", " << door->line().p1().rx();
                           //strCollision.append(" door right ");
                           bCollision = true;
                       }
                   }
                }
            }
        }

        //if( !((rooms.size() == 2) && (door != nullptr) && (bDoorCollision == false)) )
        if( bCollision == true )
        {
            //qDebug() << "Down: pos ry: " << pos().ry() << "get ry: " << r->get(0).ry();
            if( (pos().rx()+fabs(polygon().first().ry())) > (room->get(1).rx()-10) )
            {
               //qDebug() << "Room right collision";
               strCollision.append(" room right ");
            }
            else
            if( (pos().rx()-fabs(polygon().first().ry())) < (room->get(0).rx()+10) )
            {
               //qDebug() << "Room left collision";
               strCollision.append(" room left ");
            }

            if( (pos().ry()+fabs(polygon().first().ry())) > (room->get(0).ry()-10) )
            {
               //qDebug() << "Room down collision";
               strCollision.append(" room down ");
            }
            else
            if( (pos().ry()-fabs(polygon().first().ry())) < (room->get(2).ry()+10) )
            {
               //qDebug() << "Room up collision";
               strCollision.append(" room up ");
            }
        }
        else
        if( (pos().x()-fabs(polygon().first().ry())) < (dynamic_cast<Scene*>(scene)->getHall()->getDimension().x()+10) )
        {
            //qDebug() << "Hall left collision";
            strCollision.append(" room left ");
        }
        else
        if( (pos().x()+fabs(polygon().first().ry())) >
            (dynamic_cast<Scene*>(scene)->getHall()->getDimension().x()+
             dynamic_cast<Scene*>(scene)->getHall()->getDimension().width()-10) )
        {
            qDebug() << "Hall right collision";
            strCollision.append(" room right ");
        }
        else
        if( (pos().y()-fabs(polygon().first().ry())) < (dynamic_cast<Scene*>(scene)->getHall()->getDimension().y()+10) )
        {
            //qDebug() << "Hall up collision";
            strCollision.append(" room up ");
        }
        else
        if( (pos().y()+fabs(polygon().first().ry())) >
            (dynamic_cast<Scene*>(scene)->getHall()->getDimension().y()+
             dynamic_cast<Scene*>(scene)->getHall()->getDimension().height()-10) )
        {
            //qDebug() << "Hall down collision";
            strCollision.append(" room down ");
        }


        //Robot *robot = dynamic_cast<Robot*>(settings);
        if( robot != nullptr )
        {
            robot->setCollision(strCollision);

            QPointF actPos = pos();
            // Tell robot the position
            robot->setPosition(actPos);

            double rotAngle = robot->rotationAngle();
            setRotation(rotAngle);

            // Calculate movement
            QPointF newPos = robot->newPosition();
            setPos( newPos );

            QPointF sensorPos = robot->sensorPosition();
            if( robot->getSensorPosAvailable() == true )
            {
                double sensorX = sensorPos.rx() - newPos.rx();
                double sensorY = sensorPos.ry() - newPos.ry();
                double sensorRadius = sqrt((sensorX*sensorX) + (sensorY*sensorY));
                QPen sensorPen = QPen(Qt::green, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
                scene->addEllipse(newPos.x()-sensorRadius, newPos.y()-sensorRadius, 2*sensorRadius, 2*sensorRadius, sensorPen);
                //Kalman *kalman = dynamic_cast<Kalman*>(positioning);
                if( kalman->getPositionAvailable() == true )
                {
                    QPointF filterPos = kalman->positioning(sensorPos);
                    QPen filterPen = QPen(Qt::blue, 8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
                    scene->addEllipse(filterPos.x()-10, filterPos.y()-10, 20, 20, filterPen);
                }
            }

            navigator->setStartPoint(newPos);
            if( navigator->getNaviList()->size() > 1 )
            {
                if( (newPos.x() <= (navigator->getNaviList()->at(1).x()+20.0)) &&
                    (newPos.x() >= (navigator->getNaviList()->at(1).x()-20.0)) &&
                    (newPos.y() <= (navigator->getNaviList()->at(1).y()+20.0)) &&
                    (newPos.y() >= (navigator->getNaviList()->at(1).y()-20.0)) )
                {
                    qDebug() << "Navi point reached";
                    //navigator->getNaviList()->remove(1);
                }
            }

            navigator->navigate();
            dynamic_cast<Scene*>(scene)->createNavigation();

            if( robot->getMode() == Robot::Route )
            {
                // ToDo
            }
        }
    }
    else
    if( myItemType == Target )
    {
        QList<QGraphicsItem *> itemInRoom = scene->collidingItems(this, Qt::ContainsItemBoundingRect);

        foreach (QGraphicsItem *iteminroom, itemInRoom)
        {
            if( qgraphicsitem_cast<Room*>(iteminroom) != nullptr )
            {
                if( room != qgraphicsitem_cast<Room*>(iteminroom) )
                {
                    room = qgraphicsitem_cast<Room*>(iteminroom);
                    roomcoll = room;
                    roomNo = room->getNo();
                    navigator->setFinishRoom(room->getNo());
                    qDebug() << "Target in room: " << qgraphicsitem_cast<Room*>(iteminroom)->getNo();
                }
            }

        }

    }
}
