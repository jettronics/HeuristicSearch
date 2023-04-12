#include "router.h"

Router::Router()
: finished(false)
, actRoom(0)
, tarRoom(0)
{
   route_list.clear();
   //search_list.clear();
   //actualcost_list.clear();
   search_route_list.clear();
}

Router::~Router()
{
   //getRouter(-1);
   route_list.clear();
   //search_list.clear();
   //actualcost_list.clear();
   search_route_list.clear();
}

Router* Router::getRouter(Method_t Id)
{
   static Router* router[Router::Number] = {0,0,0};
   if( Id == Method_No )
   {
      for( int i=0; i<Router::Number; i++ )
      {
         Router *r = router[i];
         delete r;
         router[i] = 0;
      }  
   }
   else
   if( router[Id] == 0 )
   {
      if( Id == AStar )
      {
         router[Id] = new AStarRouter;
      }
      else
      if( Id == LeastCost )
      {
         router[Id] = new LeastCostRouter;
      }
      else
      {
         router[Id] = new BestFirstRouter;
      }
   }
   //qDebug() << "Get Router: " << Id;
   return router[Id];
}

void Router::route()
{
   route_cost = 0;
   route_list.clear();
   finished = false;

   int actRoom = 0;
   int tarRoom = 0;
   // Get Act and Target Room Number
   for( int i=0; i<rooms; i++ )
   {
      QPointF pld = p_roomlist[i].get(0);
      QPointF prd = p_roomlist[i].get(1);
      QPointF pru = p_roomlist[i].get(2);
      //QPointF plu = p_roomlist[i].get(3);
      if( (actPos.x() < prd.x()) && (actPos.x() > pld.x()) &&
          (actPos.y() < prd.y()) && (actPos.y() > pru.y()) )
      {
         actRoom = i+1;
      }
      if( (tarPos.x() < prd.x()) && (tarPos.x() > pld.x()) &&
          (tarPos.y() < prd.y()) && (tarPos.y() > pru.y()) )
      {
         tarRoom = i+1;
      }
   }
   if( actRoom == 0 )
   {
      return;
   }
   if( tarRoom == 0 )
   {
      return;
   }
   
   printf("Start : %d (%f %f)\n", actRoom, actPos.y(), actPos.x());
   printf("Target: %d (%f %f)\n", tarRoom, tarPos.y(), tarPos.x());
   
   pos_t startpos;
   startpos.room = actRoom;
   startpos.pos = actPos;
   pos_t endpos;
   endpos.room = tarRoom;
   endpos.pos = tarPos;

   route_list.push_back(startpos);

   search( route_list[0], endpos, 1, 0.0 );

   if( route_list.size() <= 1 )
   {
      route_list.clear();
   }
}

void Router::routeStart()
{
   route_list.clear();
   //search_list.clear();
   //actualcost_list.clear();
   search_route_list.clear();

   if( actRoom == 0 )
   {
      return;
   }
   if( tarRoom == 0 )
   {
      return;
   }

   qDebug() << "Start : " << actRoom;
   qDebug() << "Target: " << tarRoom;

   pos_t startpos;
   startpos.room = actRoom;
   startpos.pos = actPos;

   tarRoomPos.room = tarRoom;
   tarRoomPos.pos = tarPos;
/*
   search_t startsearch;
   startsearch.start = startpos;
   startsearch.end = endpos;
   startsearch.actual_cost = 0.0;
*/
   route_list.push_back(startpos);
   search_route_t knotRoom;
   knotRoom.actualRoom = route_list.last();
   knotRoom.actualCost = 0.0;
   knotRoom.done = false;
   search_route_list.push_back(knotRoom);
   //search_list.push_back(startsearch);
   //actualcost_list.push_back(0.0);
   finished = false;
}

void Router::addActualStart()
{
    pos_t startpos;
    startpos.room = actRoom;
    startpos.pos = actPos;
    route_list.push_front(startpos);
}

void Router::routeProcess()
{

}


BestFirstRouter::BestFirstRouter()
{
}


int BestFirstRouter::compare( const void *a, const void *b )
{
   int ascend = -1;
   if( ((cost_t*)a)->heuristic > ((cost_t*)b)->heuristic )
   {
      ascend = 1;
   }
   else
   if( ((cost_t*)a)->heuristic == ((cost_t*)b)->heuristic )
   {
      ascend = 0;
   }
   return ascend;
}

double BestFirstRouter::cost( pos_t room1, pos_t room2 )
{
   int x1 = 0, x2 = 0, y1 = 0, y2 = 0;
   double ret;
   
   x1 = room1.pos.x();
   y1 = room1.pos.y();

   x2 = room2.pos.x();
   y2 = room2.pos.y();
   
   ret = sqrt((double)((x1-x2)*(x1-x2)) + ((y1-y2)*(y1-y2)));
   return ret;
}

int BestFirstRouter::applicable( pos_t start, pos_t *p_next, pos_t destination, double *p_cost, double *p_heuristic )
{
   int i;
   int applicable = 0;
   int size = p_doorlist->size();
   if( start.room == p_next->room )
   {
      return 0;
   }
   for( i=0; i<size; i++ )
   {
      if( ((p_doorlist->at(i)->room1 == start.room) || (p_doorlist->at(i)->room2 == start.room))
          &&
          ((p_doorlist->at(i)->room1 == p_next->room) || (p_doorlist->at(i)->room2 == p_next->room)) )
      {
         applicable = 1;
         p_next->pos = p_doorlist->at(i)->center;
         *p_cost = cost( start, *p_next );
         *p_heuristic = cost( *p_next, destination );
         return 1;
      }
   }
   return 0;
}

int BestFirstRouter::find_children( pos_t start, pos_t destination, int index, cost_t *p_cost_list )
{
   int i;
   int n = 0;
   double effort;
   double guess;
   int size = rooms;
   for( i=0; i<size; i++ )
   {
      pos_t next;
      next.room = i+1;
      if( applicable( start, &next, destination, &effort, &guess ) )
      {
         int found = 0;
         //if( NULL == memchr( (int*)result_list, next, index) )
         for( int j=0; j<(int)route_list.size(); j++ )
         {
            if( route_list[j].room == next.room )
            {
               found = 1;
               break;
            }
         }
         if( found == 0 )
         {
            p_cost_list[n].room.room = next.room;
            p_cost_list[n].room.pos = next.pos;
            p_cost_list[n].value = effort;
            p_cost_list[n].heuristic = guess;
            n++;
         }
      }
   }
   return n;
}

void BestFirstRouter::search( pos_t start, pos_t destination, int index, double actual_cost )
{
   if( start.room != destination.room )
   {
      int n;
      int found = 0;
      cost_t cost_list[20];
      memset( cost_list, 0, sizeof(cost_t)*20 );

      found = find_children( start, destination, index, cost_list );
      if( found > 0 )
      {
         qsort( cost_list, found, sizeof(cost_t), compare );
         /*printf( "\nRoute: (index: %d, length: %f)\n", index, actual_cost);
         for( n=0; n<index; n++ )
         {
            printf( "%d (%d %d) \n", route_list[n].room, route_list[n].pos.y, route_list[n].pos.x );
         }*/

         /*printf( "\nFound: \n");
         for( n=0; n<found; n++ )
         {
            printf( "%f %d (%d %d) \n", cost_list[n].sum, cost_list[n].room.room, cost_list[n].room.pos.y, cost_list[n].room.pos.x );
         }*/
         for( n=0; n<found; n++ )
         {
            double new_cost;
            new_cost = actual_cost + cost_list[n].value;
            //route_list[index] = cost_list[n].room;
            route_list.push_back(cost_list[n].room);
            search( cost_list[n].room, destination, index+1, new_cost);
            if( finished == false )
            {
               route_list.pop_back();
            }
            else
            {
               return;
            }
         }
      }
   }
   else
   {
      int n;
      actual_cost += cost( route_list[index-1], destination );
      route_list.push_back(destination);
      index++;
      printf("\nRoute: %d, Length: %f \n", index, actual_cost);
      for(n=0; n<index; n++)
      {
         printf("%d (%f %f)\n", route_list[n].room, route_list[n].pos.y(), route_list[n].pos.x());
      }
      printf("\n\n");
      /* Find only the best solution */
      route_cost = actual_cost;
      finished = true;
   }
}

LeastCostRouter::LeastCostRouter()
{
}

int LeastCostRouter::compare( const void *a, const void *b )
{
   int ascend = -1;
   if( ((cost_t*)a)->value > ((cost_t*)b)->value )
   {
      ascend = 1;
   }
   else
   if( ((cost_t*)a)->value == ((cost_t*)b)->value )
   {
      ascend = 0;
   }
   return ascend;
}

double LeastCostRouter::cost( pos_t room1, pos_t room2 )
{
   int x1 = 0, x2 = 0, y1 = 0, y2 = 0;
   double ret;
   
   x1 = room1.pos.x();
   y1 = room1.pos.y();

   x2 = room2.pos.x();
   y2 = room2.pos.y();
   
   ret = sqrt((double)((x1-x2)*(x1-x2)) + ((y1-y2)*(y1-y2)));
   return ret;
}

int LeastCostRouter::applicable( pos_t start, pos_t *p_next, double *p_cost )
{
   int i;
   int applicable = 0;
   int size = p_doorlist->size();
   if( start.room == p_next->room )
   {
      return 0;
   }
   for( i=0; i<size; i++ )
   {
      if( ((p_doorlist->at(i)->room1 == start.room) || (p_doorlist->at(i)->room2 == start.room))
          &&
          ((p_doorlist->at(i)->room1 == p_next->room) || (p_doorlist->at(i)->room2 == p_next->room)) )
      {
         applicable = 1;
         p_next->pos = p_doorlist->at(i)->center;
         *p_cost = cost( start, *p_next );
         return 1;
      }
   }
   return 0;
}

int LeastCostRouter::find_children( pos_t start, int index, cost_t *p_cost_list )
{
   int i;
   int n = 0;
   double effort;
   int size = rooms;
   for( i=0; i<size; i++ )
   {
      pos_t next;
      next.room = i+1;
      if( applicable( start, &next, &effort ) )
      {
         int found = 0;
         //if( NULL == memchr( (int*)result_list, next, index) )
         for( int j=0; j<(int)route_list.size(); j++ )
         {
            if( route_list[j].room == next.room )
            {
               found = 1;
               break;
            }
         }
         if( found == 0 )
         {
            p_cost_list[n].room.room = next.room;
            p_cost_list[n].room.pos = next.pos;
            p_cost_list[n].value = effort;
            n++;
         }
      }
   }
   return n;
}

void LeastCostRouter::search( pos_t start, pos_t destination, int index, double actual_cost )
{
   if( start.room != destination.room )
   {
      int n;
      int found = 0;
      cost_t cost_list[20];
      memset( cost_list, 0, sizeof(cost_t)*20 );

      found = find_children( start, index, cost_list );
      if( found > 0 )
      {
         qsort( cost_list, found, sizeof(cost_t), compare );
         //printf( "\nRoute: (index: %d, length: %f)\n", index, actual_cost);
         /*for( n=0; n<index; n++ )
         {
            printf( "%2d ", route_list[n].room );
         }
         printf( "\n");*/
         /*printf( "\nFound: \n");
         for( n=0; n<found; n++ )
         {
            printf( "%f %d (%d %d) \n", cost_list[n].sum, cost_list[n].room.room, cost_list[n].room.pos.y, cost_list[n].room.pos.x );
         }*/
         for( n=0; n<found; n++ )
         {
            double new_cost;
            new_cost = actual_cost + cost_list[n].value;
            //route_list[index] = cost_list[n].room;
            route_list.push_back(cost_list[n].room);
            search( cost_list[n].room, destination, index+1, new_cost);
            if( finished == false )
            {
               route_list.pop_back();
            }
            else
            {
               return;
            }
         }
      }
   }
   else
   {
      int n;
      actual_cost += cost( route_list[index-1], destination );
      route_list.push_back(destination);
      index++;
      printf("\nRoute: %d, Length: %f \n", index, actual_cost);
      for(n=0; n<index; n++)
      {
         printf("%d (%f %f)\n", route_list[n].room, route_list[n].pos.y(), route_list[n].pos.x());
      }
      printf("\n\n");
      /* Find only the best solution */
      route_cost = actual_cost;
      finished = true;
   }
}

AStarRouter::AStarRouter()
{
}


int AStarRouter::compare( const void *a, const void *b )
{
   int ascend = -1;
   if( ((cost_t*)a)->sum > ((cost_t*)b)->sum )
   {
      ascend = 1;
   }
   else
   if( ((cost_t*)a)->sum == ((cost_t*)b)->sum )
   {
      ascend = 0;
   }
   return ascend;
}

double AStarRouter::cost( pos_t room1, pos_t room2 )
{
   int x1 = 0, x2 = 0, y1 = 0, y2 = 0;
   double ret;
   
   x1 = room1.pos.x();
   y1 = room1.pos.y();

   x2 = room2.pos.x();
   y2 = room2.pos.y();
   
   ret = sqrt((double)((x1-x2)*(x1-x2)) + ((y1-y2)*(y1-y2)));
   return ret;
}

int AStarRouter::applicable( pos_t start, pos_t *p_next, pos_t destination, double *p_cost, double *p_heuristic )
{
   int i;
   int applicable = 0;
   int size = p_doorlist->size();
   if( start.room == p_next->room )
   {
      return 0;
   }
   for( i=0; i<size; i++ )
   {
      if( ((p_doorlist->at(i)->room1 == start.room) || (p_doorlist->at(i)->room2 == start.room))
          &&
          ((p_doorlist->at(i)->room1 == p_next->room) || (p_doorlist->at(i)->room2 == p_next->room)) )
      {
         applicable = 1;
         p_next->pos = p_doorlist->at(i)->center;
         *p_cost = cost( start, *p_next );
         *p_heuristic = cost( *p_next, destination );
         return 1;
      }
   }
   return 0;
}

int AStarRouter::find_children( pos_t start, pos_t destination, int index, cost_t *p_cost_list )
{
   int i;
   int n = 0;
   double effort;
   double guess;
   int size = rooms;
   for( i=0; i<size; i++ )
   {
      pos_t next;
      next.room = i+1;
      if( applicable( start, &next, destination, &effort, &guess ) )
      {
         int found = 0;
         //if( NULL == memchr( (int*)result_list, next, index) )
         for( int j=0; j<(int)route_list.size(); j++ )
         {
            if( route_list[j].room == next.room )
            {
               found = 1;
               break;
            }
         }
         if( found == 0 )
         {
            p_cost_list[n].room.room = next.room;
            p_cost_list[n].room.pos = next.pos;
            p_cost_list[n].value = effort;
            p_cost_list[n].sum = effort+guess;
            n++;
         }
      }
   }
   return n;
}

void AStarRouter::search( pos_t start, pos_t destination, int index, double actual_cost )
{
   if( start.room != destination.room )
   {
      int n;
      int found = 0;
      cost_t cost_list[20];
      memset( cost_list, 0, sizeof(cost_t)*20 );

      found = find_children( start, destination, index, cost_list );
      if( found > 0 )
      {
         qsort( cost_list, found, sizeof(cost_t), compare );
         /*printf( "\nRoute: (index: %d, length: %f)\n", index, actual_cost);
         for( n=0; n<index; n++ )
         {
            printf( "%d (%d %d) \n", route_list[n].room, route_list[n].pos.y, route_list[n].pos.x );
         }*/

         /*printf( "\nFound: \n");
         for( n=0; n<found; n++ )
         {
            printf( "%f %d (%d %d) \n", cost_list[n].sum, cost_list[n].room.room, cost_list[n].room.pos.y, cost_list[n].room.pos.x );
         }*/
         for( n=0; n<found; n++ )
         {
            double new_cost;
            new_cost = actual_cost + cost_list[n].value;
            //route_list[index] = cost_list[n].room;
            route_list.push_back(cost_list[n].room);
            search( cost_list[n].room, destination, index+1, new_cost);
            if( finished == false )
            {
               route_list.pop_back();
            }
            else
            {
               return;
            }
         }
      }
   }
   else
   {
      int n;
      actual_cost += cost( route_list[index-1], destination );
      route_list.push_back(destination);
      index++;
      printf("\nRoute: %d, Length: %f \n", index, actual_cost);
      for(n=0; n<index; n++)
      {
         printf("%d (%f %f)\n", route_list[n].room, route_list[n].pos.y(), route_list[n].pos.x());
      }
      printf("\n\n");
      /* Find only the best solution */
      route_cost = actual_cost;
      finished = true;
   }
}

void AStarRouter::routeProcess()
{
    if( search_route_list.last().done == true )
    {
        search_route_list.pop_back();
        search_route_list.last().childrenRooms.pop_back();
        route_list.pop_back();

        if( search_route_list.last().childrenRooms.size() > 0 )
        {
            qDebug() << "Last knot already done with children(s)";
            search_route_t knotRoom;
            knotRoom.actualRoom = search_route_list.last().childrenRooms.last().actualRoom;
            knotRoom.actualCost = search_route_list.last().childrenRooms.last().actualCost;
            // New active knot
            search_route_list.push_back(knotRoom);
            search_route_list.last().done = false;
            route_list.push_back(search_route_list.last().actualRoom);
        }
        else
        {
            qDebug() << "Last knot already done without children(s)";
            // No more children
            search_route_list.last().done = true;
        }
    }
    else
    if( search_route_list.last().actualRoom.room != tarRoom )
    {
        int n;
        int found = 0;
        cost_t cost_list[20];
        memset( cost_list, 0, sizeof(cost_t)*20 );

        found = find_children( search_route_list.last().actualRoom, tarRoomPos, 0, cost_list );
        qDebug() << "Found " << found << "rooms";
        if( found > 0 )
        {
            qsort( cost_list, (size_t)found, sizeof(cost_t), compare );
            for( n=found-1; n>=0; n-- )
            {
                search_route_t foundroom;
                foundroom.actualRoom = cost_list[n].room;
                foundroom.actualCost = cost_list[n].value+search_route_list.last().actualCost;
                search_route_list.last().childrenRooms.push_back(foundroom);
            }
            search_route_t knotRoom;
            knotRoom.actualRoom = search_route_list.last().childrenRooms.last().actualRoom;
            knotRoom.actualCost = search_route_list.last().childrenRooms.last().actualCost;
            // New active knot
            search_route_list.push_back(knotRoom);
            search_route_list.last().done = false;
            route_list.push_back(search_route_list.last().actualRoom);
            printf("\nChildren, actual cost: %f to room: %d\n", search_route_list.last().actualCost, search_route_list.last().actualRoom.room);
        }
        else
        {
            // New active knot has no children

            // Remove knot
            search_route_list.pop_back();
            search_route_list.last().childrenRooms.pop_back();
            route_list.pop_back();

            if( search_route_list.last().childrenRooms.size() > 0 )
            {
                qDebug() << "Backtracking with children(s) of room: " << search_route_list.last().actualRoom.room;
                search_route_t knotRoom;
                knotRoom.actualRoom = search_route_list.last().childrenRooms.last().actualRoom;
                knotRoom.actualCost = search_route_list.last().childrenRooms.last().actualCost;
                // New active knot
                search_route_list.push_back(knotRoom);
                search_route_list.last().done = false;
                route_list.push_back(search_route_list.last().actualRoom);
            }
            else
            {
                qDebug() << "Backtracking without childrens";
                // No more children
                search_route_list.last().done = true;
            }
            printf("\nNo Children, actual cost: %f to room: %d\n", search_route_list.last().actualCost, search_route_list.last().actualRoom.room);
        }

        printf("\nRoute list: ");
        for(n=0; n<route_list.size(); n++)
        {
           printf("%d ", route_list[n].room);
        }
        printf("\n");

        printf("\nSearch list from parent: %d", search_route_list.last().actualRoom.room);
        for(n=0; n<search_route_list.last().childrenRooms.size(); n++)
        {
           printf("%d ", search_route_list.last().childrenRooms[n].actualRoom.room);
        }
        printf("\n");
    }
    else
    {
        int n;
        route_list.push_back(tarRoomPos);
        //actualcost_list.push_back(actualcost_list.last()+cost( route_list.at(route_list.size()-2), search_list.last().end ));
        //route_cost = actualcost_list.last();
        route_cost = search_route_list.last().actualCost+cost( search_route_list.at(search_route_list.size()-1).actualRoom, tarRoomPos );
        finished = true;
        printf("\nRoute list: ");
        for(n=0; n<route_list.size(); n++)
        {
           printf("%d ", route_list[n].room);
        }
        printf("\n");
        printf("\nRoute cost: %f\n", route_cost);
        search_route_list.clear();
        //actualcost_list.clear();
    }

}
