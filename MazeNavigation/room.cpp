#include "room.h"

Room::Room()
    : QGraphicsPolygonItem(0)
    , roomNo(0)
{
    points.clear();
}


void Room::add( QPointF point )
{
    points.push_back(point);
}

void Room::add( int y, int x )
{
    QPointF p;
    p.setY(y);
    p.setX(x);
    points.push_back(p);
}

void Room::setPointsToPolygon()
{
    myPolygon = points;
    setPolygon(myPolygon);
}


void Room::set( int pos, int y, int x )
{
   points[pos].setX(x);
   points[pos].setY(y);
}

void Room::remove( int pos )
{
   points.remove(pos);
}
