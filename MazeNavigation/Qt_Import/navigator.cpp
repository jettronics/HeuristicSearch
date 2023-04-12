#include "navigator.h"

Navigator::Navigator(QMainWindow *window)
: mainWindow(window)
, router(0)
, routerSelect(Router::Method_No)
, p_roomlist(0)
, rooms(0)
, startRoom(0)
, finishRoom(0)
, startRoomNew(0)
, finishRoomNew(0)
, routeState(NoRoute)
, navilistCnt(0)
{
   navilist.clear();
   doorlist.clear();

   resize(330, 160);

   gridLayout = new QGridLayout;

   aStarBox = new QCheckBox(tr("A-Star routing"));
   leastCostBox = new QCheckBox(tr("Least Cost routing"));
   bestFirstBox = new QCheckBox(tr("Best First routing"));
   gridLayout->addWidget(aStarBox, 0, 0, 1, 1);
   gridLayout->addWidget(leastCostBox, 1, 0, 1, 1);
   gridLayout->addWidget(bestFirstBox, 2, 0, 1, 1);

   resultAStar = new QLineEdit("0");
   resultAStar->setReadOnly(true);
   resultAStar->setAlignment(Qt::AlignRight);
   resultAStar->setMaxLength(6);

   resultBestFirst = new QLineEdit("0");
   resultBestFirst->setReadOnly(true);
   resultBestFirst->setAlignment(Qt::AlignRight);
   resultBestFirst->setMaxLength(6);

   resultLeastCost = new QLineEdit("0");
   resultLeastCost->setReadOnly(true);
   resultLeastCost->setAlignment(Qt::AlignRight);
   resultLeastCost->setMaxLength(6);

   gridLayout->addWidget(resultAStar, 0, 1, 1, 1);
   gridLayout->addWidget(resultLeastCost, 1, 1, 1, 1);
   gridLayout->addWidget(resultBestFirst, 2, 1, 1, 1);

   useAStar = new QRadioButton("A-Star Navigator", this);
   useBestFirst = new QRadioButton("Best First Navigator", this);
   useLeastCost = new QRadioButton("Least Cost Navigator", this);
   useNothing = new QRadioButton("No Navigator", this);
   useNothing->setChecked(true);

   gridLayout->addWidget(useAStar, 0, 2, 1, 1);
   gridLayout->addWidget(useLeastCost, 1, 2, 1, 1);
   gridLayout->addWidget(useBestFirst, 2, 2, 1, 1);
   gridLayout->addWidget(useNothing, 3, 2, 1, 1);

   cancelBox = new QDialogButtonBox();
   cancelBox->setOrientation(Qt::Horizontal);
   cancelBox->setStandardButtons(QDialogButtonBox::Cancel);
   okBox = new QDialogButtonBox();
   okBox->setOrientation(Qt::Horizontal);
   okBox->setStandardButtons(QDialogButtonBox::Ok);
   //okBox->setEnabled(false);

   planBox = new QDialogButtonBox();
   planBox->setOrientation(Qt::Horizontal);
   planBox->addButton("Plan", QDialogButtonBox::AcceptRole);

   gridLayout->addWidget(cancelBox, 4, 2, 1, 1);
   gridLayout->addWidget(planBox, 4, 1, 1, 1);
   gridLayout->addWidget(okBox, 4, 0, 1, 1);

   setLayout(gridLayout);

   connect(this, SIGNAL(acceptNaviBox()), mainWindow, SLOT(acceptNaviBox()));
   connect(this, SIGNAL(planNaviBox()), mainWindow, SLOT(planNaviBox()));
   connect(this, SIGNAL(rejectNaviBox()), mainWindow, SLOT(rejectNaviBox()));
   connect(okBox, SIGNAL(accepted()), this, SLOT(acceptOkBox()));
   connect(cancelBox, SIGNAL(rejected()), this, SLOT(rejectBox()));
   connect(planBox, SIGNAL(accepted()), this, SLOT(acceptPlanBox()));

   setWindowTitle(tr("Navigator Settings"));
}

Navigator::~Navigator()
{
   doorlist.clear();
}

void Navigator::setDoorList(QVector<Door *> *p)
{
   doorlist.clear();
   for( int i=0; i<p->size(); i++ )
   {
      if( p->at(i)->open == true )
      {
         doorlist.push_back(p->at(i));
      }
   }
}

void Navigator::plan(QVector<Router::Method_t> routerIds)
{
   double routeCost = 0;
   routerSelect = Router::Method_No;
   
   if( routerIds.size() > 0 )
   {
      for( int i=0; i<(int)routerIds.size(); i++ )
      {
         printf("Router %d\n", routerIds[i]);
         router = Router::getRouter(routerIds[i]);
         router->setRoomList( p_roomlist, rooms );
         router->setDoorList( &doorlist );
         router->setActualPos( startPos );
         router->setTargetPos( finishPos );
         router->route();
         routeCost = router->getRouteCost();
         if( routerIds[i] == Router::AStar )
         {
            resultAStar->setText(QString("%1").arg(int(routeCost+0.5f)));
         }
         else
         if( routerIds[i] == Router::BestFirst )
         {
            resultBestFirst->setText(QString("%1").arg(int(routeCost+0.5f)));
         }
         else
         if( routerIds[i] == Router::LeastCost )
         {
            resultLeastCost->setText(QString("%1").arg(int(routeCost+0.5f)));
         }
         /*
         if( ((routeCost > router->getRouteCost()) ||
              (routeCost == 0)) && 
             (router->getRoute()->size() > 0) )
         {
            routeCost = router->getRouteCost();
            routerSelect = routerIds[i];
         } */
      }

      if( useNothing->isChecked() == false )
      {
         if( useAStar->isChecked() == true )
         {
            routerSelect = Router::AStar;
         }
         else
         if( useLeastCost->isChecked() == true )
         {
            routerSelect = Router::LeastCost;
         }
         else
         {
            routerSelect = Router::BestFirst;
         }

         qDebug() << "Router selection: " << routerSelect;

         if( routerSelect != Router::Method_No )
         {
            createNavPoints(router, routerSelect);
         }
      }
   }
}

void Navigator::createNavPoints(Router* router, Router::Method_t routerSelect)
{
    if( router->getRouteCost() > 0 )
    {
       QVector<Router::pos_t> routeList = *router->getRouter(routerSelect)->getRoute();

       navilist.clear();
       navilist.push_back(routeList[0].pos);

       int routeListSize = routeList.size();

       for( int i=1; i<(int)routeListSize-1; i++ )
       {
          bool horizontal = false;
          for( int j=0; j<(int)doorlist.size(); j++ )
          {
             if( (routeList[i].pos.y() == doorlist[j]->center.y()) &&
                 (routeList[i].pos.x() == doorlist[j]->center.x()))
             {
                // Horizontal or Vertical
                if( doorlist[j]->outa.y() == doorlist[j]->outb.y() )
                {
                   horizontal = true;
                }
                break;
             }
          }
          QPointF p1, p2;
          if( horizontal == true )
          {
             p1.setX(routeList[i].pos.x());
             p2.setX(routeList[i].pos.x());
             if( navilist[navilist.size()-1].y() < routeList[i].pos.y() )
             {
                p1.setY(routeList[i].pos.y()-100);
                p2.setY(routeList[i].pos.y()+100);
             }
             else
             {
                p1.setY(routeList[i].pos.y()+100);
                p2.setY(routeList[i].pos.y()-100);
             }
          }
          else
          {
             p1.setY(routeList[i].pos.y());
             p2.setY(routeList[i].pos.y());
             if( navilist[navilist.size()-1].x() < routeList[i].pos.x() )
             {
                p1.setX(routeList[i].pos.x()-100);
                p2.setX(routeList[i].pos.x()+100);
             }
             else
             {
                p1.setX(routeList[i].pos.x()+100);
                p2.setX(routeList[i].pos.x()-100);
             }
          }
          navilist.push_back(p1);
          navilist.push_back(p2);
       }
       navilist.push_back( routeList[routeListSize-1].pos );
    }
}

void Navigator::updateNavPoints(Router* router, int i)
{
    if( i > 0 )
    {
        if( i > (navilist.size()-1) )
        {
            QPointF p1;
            p1.setX(router->getRoute()->at(i).pos.x());
            p1.setY(router->getRoute()->at(i).pos.y());
            navilist.push_back(p1);
            qDebug() << "Add Nav point: " << p1;
        }
        else
        if( i < (navilist.size()-1) )
        {
            navilist.pop_back();
            qDebug() << "Remove Nav point";
        }
        else
        {
            if( (navilist.at(i).x() != router->getRoute()->at(i).pos.x()) ||
                (navilist.at(i).y() != router->getRoute()->at(i).pos.y()) )
            {
                navilist.pop_back();
                QPointF p1;
                p1.setX(router->getRoute()->at(i).pos.x());
                p1.setY(router->getRoute()->at(i).pos.y());
                navilist.push_back(p1);
                qDebug() << "Replace Nav point";
            }
        }
    }
}

void Navigator::clearNavPoints()
{
    navilist.clear();
    if( router != nullptr )
    {
        router->getRoute()->clear();
    }
    routeState = NoRoute;
}

void Navigator::navigate()
{
    switch(routeState)
    {
    case NoRoute:
        if( (startRoomNew != 0) && (finishRoomNew != 0) )
        {
            router = Router::getRouter(Router::AStar);
            router->setRoomList( p_roomlist, rooms );
            router->setDoorList( &doorlist );
            router->setActualPos( startPos );
            router->setTargetPos( finishPos );
            router->setActualRoom(startRoomNew);
            router->setTargetRoom(finishRoomNew);
            router->routeStart();
            startRoom = startRoomNew;
            finishRoom = finishRoomNew;
            startPosPrev = startPos;
            finishPosPrev = finishPos;
            navilist.clear();
            navilist.push_back(startPos);
            routeState = UpdateRoute;
            qDebug() << "Start Route";
        }
        break;
    case UpdateRoute:
        if( (startRoomNew == 0) || (finishRoomNew == 0) )
        {
            navilist.clear();
            router->getRoute()->clear();
            routeState = NoRoute;
            qDebug() << "Abort Route";
        }
        else
        {
            router->routeProcess();
            updateNavPoints(router, router->getRoute()->size()-1);
            if( router->getFinished() == true )
            {
                /*
                navilist.clear();
                navilist.push_back(startPos);
                navilistCnt = 0;
                routeState = FinalRoute;
                qDebug() << "Final Route";
                */
                //navilist.push_back(finishPos);
                qDebug() << "Nav points last: " << finishPos;
                routeState = Routed;
                qDebug() << "Route calculated";
            }
        }
        break;
    case FinalRoute:
        updateNavPoints(router, navilistCnt);
        navilistCnt++;
        if( (router->getRoute()->size()-1) <= navilistCnt )
        {
            //navilist.push_back(finishPos);
            qDebug() << "Nav points last: " << finishPos;
            routeState = Routed;
            qDebug() << "Route calculated";
        }
        break;
    case Routed:
        if( (startRoomNew == 0) || (finishRoomNew == 0) )
        {
            navilist.clear();
            router->getRoute()->clear();
            routeState = NoRoute;
            qDebug() << "Remove Route";
        }
        else
        if( (startRoomNew != startRoom) || (finishRoomNew != finishRoom) )
        {
            bool newRouteNeeded = true;
            router = Router::getRouter(Router::AStar);

            router->setActualPos( startPos );
            router->setTargetPos( finishPos );
            router->setActualRoom(startRoomNew);
            router->setTargetRoom(finishRoomNew);

            if( startRoomNew != startRoom )
            {
                newRouteNeeded = false;
                if( startRoomNew == router->getRoute()->at(1).room )
                {
                    navilist.removeFirst();
                    router->getRoute()->removeFirst();
                    qDebug() << "Remove start room: " << startRoomNew;
                }
                else
                {
                    router->addActualStart();
                    navilist.push_front(startPos);
                    qDebug() << "Add start room: " << startRoomNew;
                }
            }
            /*
            if( (finishRoomNew != finishRoom) &&
                (finishRoomNew == router->getRoute()->at(router->getRoute()->size()-2).room) )
            {
                navilist.removeLast();
                router->getRoute()->removeLast();
                newRouteNeeded = false;
                qDebug() << "New end room: " << finishRoomNew;
            }
            */
            if( newRouteNeeded == true )
            {
                router->routeStart();
                routeState = UpdateRoute;
                qDebug() << "Change Route";
            }
            if( startRoomNew != startRoom )
            {
                startRoom = startRoomNew;
                navilist.first() = startPos;
            }
            if( finishRoomNew != finishRoom )
            {
                finishRoom = finishRoomNew;
                navilist.last() = finishPos;
            }

            //navilist.clear();
            //navilist.push_back(startPos);
        }
        else
        {
            if( startPosPrev != startPos )
            {
                startPosPrev = startPos;
                navilist.first() = startPos;
            }
            if( finishPosPrev != finishPos )
            {
                finishPosPrev = finishPos;
                navilist.last() = finishPos;
            }
        }
        break;
    default:
        break;
    }
}

void Navigator::rejectBox()
{
   navilist.clear();
   Router::getRouter(Router::Method_No);
   emit rejectNaviBox();
   hide();
}

void Navigator::reject()
{
   navilist.clear();
   Router::getRouter(Router::Method_No);
   emit rejectNaviBox();
   hide();
}

void Navigator::acceptOkBox()
{
   emit acceptNaviBox();
}

void Navigator::acceptPlanBox()
{
   QVector<Router::Method_t> planList;
   planList.clear();
   if( aStarBox->isChecked() == true )
   {
      planList.push_back(Router::AStar);
   }
   if( leastCostBox->isChecked() == true )
   {
      planList.push_back(Router::LeastCost);
   }
   if( bestFirstBox->isChecked() == true )
   {
      planList.push_back(Router::BestFirst);
   }

   plan(planList);

   emit planNaviBox();
}
