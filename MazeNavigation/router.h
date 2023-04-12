#ifndef ROUTER_H
#define ROUTER_H

#include <QtGui>
#include <QDialog>
#include "room.h"
#include "door.h"


class Router
{
public:
    Router();
    virtual ~Router();

    typedef struct
    {
       int room;
       QPointF pos;
    }
    pos_t;
/*
    typedef struct
    {
       pos_t start;
       double actual_cost;
    }
    search_t;
*/
    typedef struct search_route_t
    {
        pos_t actualRoom;
        double actualCost;
        bool done;
        QVector<search_route_t> childrenRooms;
    }
    search_route_t;

    typedef enum { Method_No = -1, AStar = 0, LeastCost, BestFirst, Method_Max } Method_t;
    static const Method_t Number = Method_Max;

    void setActualPos( QPointF p ) { actPos = p; }
    void setTargetPos( QPointF p ) { tarPos = p; }
    void setActualRoom( int room ) { actRoom = room; }
    void setTargetRoom( int room ) { tarRoom = room; }
    void setRoomList( Room *p, int size ) { p_roomlist = p; rooms = size; }
    void setDoorList( QVector<Door*> *p ) { p_doorlist = p; }
    void addActualStart();

    virtual void route();
    virtual void routeStart();
    virtual void routeProcess();
    QVector<pos_t>* getRoute() { return &route_list; }

    double getRouteCost() { return route_cost; }
    int getFinished() { return finished; }

    static Router* getRouter(Method_t Id);

protected:
   virtual void search( pos_t start, pos_t destination, int index, double actual_cost )=0;

protected:
   // can be inherited
   QPointF actPos;
   QPointF tarPos;
   Room *p_roomlist;
   int rooms;
   QVector<Door*> *p_doorlist;
   QVector<pos_t> route_list;
   double route_cost;
   bool finished;
   int actRoom;
   int tarRoom;
   //QVector<search_t> search_list;
   //QVector<double> actualcost_list;
   QVector<search_route_t> search_route_list;
   pos_t tarRoomPos;
};


class AStarRouter : public Router
{
public:
   AStarRouter();

private:
   typedef struct
   {
      double value;
      double sum;
      pos_t room;
   }
   cost_t;
   
   static int compare( const void *a, const void *b );
   double cost( pos_t room1, pos_t room2 );
   int applicable( pos_t start, pos_t *p_next, pos_t destination, double *p_cost, double *p_heuristic );
   int find_children( pos_t start, pos_t destination, int index, cost_t *p_cost_list );
   void search( pos_t start, pos_t destination, int index, double actual_cost );
   void routeProcess();
};

class LeastCostRouter : public Router
{
public:
   LeastCostRouter();

private:
   typedef struct
   {
      double value;
      pos_t room;
   }
   cost_t;
   
   static int compare( const void *a, const void *b );
   double cost( pos_t room1, pos_t room2 );
   int applicable( pos_t start, pos_t *p_next, double *p_cost );
   int find_children( pos_t start, int index, cost_t *p_cost_list );
   void search( pos_t start, pos_t destination, int index, double actual_cost );

};

class BestFirstRouter : public Router
{
public:
   BestFirstRouter();

private:
   typedef struct
   {
      double value;
      double heuristic;
      pos_t room;
   }
   cost_t;
   
   static int compare( const void *a, const void *b );
   double cost( pos_t room1, pos_t room2 );
   int applicable( pos_t start, pos_t *p_next, pos_t destination, double *p_cost, double *p_heuristic );
   int find_children( pos_t start, pos_t destination, int index, cost_t *p_cost_list );
   void search( pos_t start, pos_t destination, int index, double actual_cost );

};


#endif // ROUTER_H
