#ifndef NAVIGATOR_H
#define NAVIGATOR_H

#include <QtGui>
#include <QDialog>
#include <QGridLayout>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QMainWindow>
#include <QLineEdit>
#include <QRadioButton>
#include "room.h"
#include "door.h"
#include "router.h"

class Navigator : public QDialog
{
   Q_OBJECT

public:
    enum RouteStateType { NoRoute, UpdateRoute, FinalRoute, Routed };

   Navigator(QMainWindow *window);
   virtual ~Navigator();

   void setRoomList( Room *p, int size ) { p_roomlist = p; rooms = size; }
   void setDoorList( QVector<Door*> *p );
   void setStartPoint( QPointF start ) { startPos = start; }
   void setFinishPoint( QPointF finish ) { finishPos = finish; }
   void setStartRoom(int room ) { startRoomNew = room; }
   void setFinishRoom( int room ) { finishRoomNew = room; }
   void clearNavPoints();

   QVector<QPointF>* getNaviList() { return &navilist; }
   
   void plan(QVector<Router::Method_t> routerIds);
   void navigate();

signals:
    void planNaviBox();
    void acceptNaviBox();
    void rejectNaviBox();

private slots:
    void rejectBox();
    void acceptOkBox();
    void acceptPlanBox();

public slots:
    void reject();

private:
    void createNavPoints(Router* router, Router::Method_t routerSelect);
    void updateNavPoints(Router* router, int i);

private:
    QMainWindow *mainWindow;
    QGridLayout *gridLayout;
    QCheckBox *aStarBox;
    QCheckBox *leastCostBox;
    QCheckBox *bestFirstBox;
    QDialogButtonBox *cancelBox;
    QDialogButtonBox *okBox;
    QDialogButtonBox *planBox;
    QLineEdit *resultAStar;
    QLineEdit *resultBestFirst;
    QLineEdit *resultLeastCost;
    QRadioButton *useNothing;
    QRadioButton *useAStar;
    QRadioButton *useBestFirst;
    QRadioButton *useLeastCost;

protected:
   Router* router;
   Router::Method_t routerSelect;
   Room *p_roomlist;
   int rooms;
   QVector<Door*> doorlist;
   QPointF startPos;
   QPointF finishPos;
   QPointF startPosPrev;
   QPointF finishPosPrev;
   QVector<QPointF> navilist;
   int startRoom;
   int finishRoom;
   int startRoomNew;
   int finishRoomNew;
   RouteStateType routeState;
   int navilistCnt;
};

#endif // NAVIGATOR_H
