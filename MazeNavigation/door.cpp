
#include "door.h"

Door::Door()
 : QGraphicsLineItem(0)
 , open(true)
{
}

void Door::setLine()
{
   QLineF line = QLineF(outa,outb);
   QGraphicsLineItem::setLine(line);
}

