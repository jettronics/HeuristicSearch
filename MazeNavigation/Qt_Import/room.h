#ifndef ROOM_H
#define ROOM_H

#include <QtGui>
#include <QGraphicsPolygonItem>


class Room : public QGraphicsPolygonItem
{
public:
    Room();

    int getNo() {return roomNo;};
    void setNo( int no ) { roomNo = no; };

    void add( QPointF point );
    void add( int y, int x );
    void set( int pos, int y, int x );
    QPointF get( int pos ) { return points[pos]; };
    void remove( int pos );
    int size() { return points.size(); };
    void clear() {points.clear(); };

    void setPointsToPolygon();

private:
    int roomNo;
    QVector<QPointF> points;
    QPolygonF myPolygon;
};

#endif // ROOM_H
