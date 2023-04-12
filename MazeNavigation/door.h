#ifndef DOOR_H
#define DOOR_H

#include <QtGui>
#include <QGraphicsLineItem>

class Door : public QGraphicsLineItem
{
public:
   Door();

public:
   void setLine();

public:
   int room1;
   int room2;
   QPointF center;
   QPointF outa;
   QPointF outb;
   bool open;
};


#endif // DOOR_H
