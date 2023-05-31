using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using static Maze_generator.Models.Route;

namespace Maze_generator.Models
{
    abstract class Route
    {
        public struct pos_t
        {
            public int room;
            public PointF pos;
        }

        public struct cost_t
        {
            public double value;
            public double sum;
            public pos_t room;
        }

        public class search_route_t
        {
            public pos_t actualRoom;
            public double actualCost;
            public bool done;
            public List<search_route_t> childrenRooms;
        }

        protected PointF actPos;
        protected PointF tarPos;
        protected int actRoom;
        protected int tarRoom;
        protected List<Door> doorList;
        protected List<pos_t> route_list;
        protected List<List<pos_t>> routes;
        protected List<search_route_t> search_route_list;
        protected double route_cost;
        protected bool finished;
        protected Size rooms;
        protected pos_t tarRoomPos;
        protected float roomWidthLen;
        protected float roomHeightLen;
        protected List<cost_t> cost_list;

        public List<pos_t> getRoute() { return route_list; }
        public double getRouteCost() { return route_cost; }
        public bool getFinished() { return finished; }

        protected PointF calcRoomPos(int room)
        {
            PointF ret = new PointF();
            ret.X = (((float)(room % rooms.Width)) * roomWidthLen) + (roomWidthLen / 2.0f);
            ret.Y = (((float)(room / rooms.Height)) * roomHeightLen) + (roomHeightLen / 2.0f);
            return ret;
        }

        protected double distance(pos_t room1, pos_t room2)
        {
            double ret;

            double x1 = room1.pos.X;
            double y1 = room1.pos.Y;

            double x2 = room2.pos.X;
            double y2 = room2.pos.Y;

            ret = Math.Sqrt(((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2)));

            return ret;
        }

        protected int compareCostBySum(cost_t a, cost_t b)
        {
            int ascend = -1;
            if (a.sum > b.sum)
            {
                ascend = 1;
            }
            else
            if (a.sum == b.sum)
            {
                //ascend = 0;
                ascend = 1;
                var random = new Random(Guid.NewGuid().GetHashCode());
                if (random.Next(2) == 0)
                {
                    ascend = -1;
                }
            }
            return ascend;
        }

        protected int applicable(pos_t start, ref pos_t next, pos_t destination, ref double cost, ref double heuristic)
        {
            int i;
            int size = doorList.Count;
            if (start.room == next.room)
            {
                return 0;
            }
            for (i = 0; i < size; i++)
            {
                if (((doorList.ElementAt(i).Cell1 == start.room) || (doorList.ElementAt(i).Cell2 == start.room))
                    &&
                    ((doorList.ElementAt(i).Cell1 == next.room) || (doorList.ElementAt(i).Cell2 == next.room)))
                {
                    next.pos = calcRoomPos(next.room);
                    cost = distance(start, next);
                    heuristic = distance(next, destination);
                    return 1;
                }
            }
            return 0;
        }

        protected abstract int find_children(pos_t start, pos_t destination, int index);

        public Route()
        {
            finished = false;
            actRoom = 0;
            tarRoom = 0;
            rooms.Width = 0;
            rooms.Height = 0;
            roomWidthLen = 0.0f;
            roomHeightLen = 0.0f;
            //doorList = new List<Door>();
            route_list = new List<pos_t>();
            routes = new List<List<pos_t>>();
            search_route_list = new List<search_route_t>();
            //route_list.Clear();
            //search_route_list.Clear();
            cost_list = new List<cost_t>();
        }

        public void clearRoute()
        {
            route_cost = 0;

            route_list.Clear();
            routes.Clear();
            search_route_list.Clear();
            cost_list.Clear();

            return;
        }

        public virtual void routeStart(int actR, int tarR, Size roomC, List<Door> doorL)
        {
            route_cost = 0;
            
            route_list.Clear();
            routes.Clear();
            search_route_list.Clear();
            cost_list.Clear();
            
            if (tarR == 0)
            {
                return;
            }

            actRoom = actR;
            tarRoom = tarR;
            rooms = roomC;
            doorList = doorL;


            pos_t startpos;
            startpos.room = actRoom;
            roomWidthLen = (100.0f / ((float)rooms.Width));
            roomHeightLen = (100.0f / ((float)rooms.Height));
            actPos = calcRoomPos(actRoom);
            startpos.pos = actPos;

            tarRoomPos.room = tarRoom;
            tarPos = calcRoomPos(tarRoom);
            tarRoomPos.pos = tarPos;

            route_list.Add(startpos);
            search_route_t knotRoom = new search_route_t();
            knotRoom.actualRoom = route_list.Last();
            knotRoom.actualCost = 0.0f;
            knotRoom.done = false;
            knotRoom.childrenRooms = new List<search_route_t>();
            search_route_list.Add(knotRoom);

            finished = false;
        }

        public virtual void routeProcess()
        {
            if (search_route_list.Last().done == true)
            {
                Debug.WriteLine("Route list last done\n");

                search_route_list.RemoveAt(search_route_list.Count - 1);
                search_route_list.Last().childrenRooms.RemoveAt(search_route_list.Last().childrenRooms.Count - 1);
                route_list.RemoveAt(route_list.Count - 1);

                if (search_route_list.Last().childrenRooms.Count > 0)
                {
                    //qDebug() << "Last knot already done with children(s)";
                    search_route_t knotRoom = new search_route_t();
                    knotRoom.actualRoom = search_route_list.Last().childrenRooms.Last().actualRoom;
                    knotRoom.actualCost = search_route_list.Last().childrenRooms.Last().actualCost;
                    knotRoom.done = false;
                    knotRoom.childrenRooms = new List<search_route_t>();
                    // New active knot
                    search_route_list.Add(knotRoom);
                    search_route_list.Last().done = false;
                    route_list.Add(search_route_list.Last().actualRoom);
                }
                else
                {
                    //qDebug() << "Last knot already done without children(s)";
                    // No more children
                    search_route_list.Last().done = true;
                    if (search_route_list.Last().actualRoom.room == 0)
                    {
                        // All solutions found
                        Debug.WriteLine("All Routes found\n");
                        finished = true;
                    }
                }
            }
            else
            if (search_route_list.Last().actualRoom.room != tarRoom)
            {
                int n;
                int found = 0;
                cost_list.Clear();

                found = find_children(search_route_list.Last().actualRoom, tarRoomPos, 0);
                //qDebug() << "Found " << found << "rooms";
                if (found > 0)
                {
                    cost_list.Sort(compareCostBySum);
                    //for (n = 0; n < found; n++)
                    for (n = found - 1; n >= 0; n--)
                    {
                        search_route_t foundroom = new search_route_t();
                        foundroom.actualRoom = cost_list[n].room;
                        foundroom.actualCost = cost_list[n].value + search_route_list.Last().actualCost;
                        foundroom.childrenRooms = new List<search_route_t>();
                        search_route_list.Last().childrenRooms.Add(foundroom);
                    }
                    search_route_t knotRoom = new search_route_t();
                    knotRoom.actualRoom = search_route_list.Last().childrenRooms.Last().actualRoom;
                    knotRoom.actualCost = search_route_list.Last().childrenRooms.Last().actualCost;
                    knotRoom.childrenRooms = new List<search_route_t>();
                    // New active knot
                    search_route_list.Add(knotRoom);
                    search_route_list.Last().done = false;
                    route_list.Add(search_route_list.Last().actualRoom);
                    //printf("\nChildren, actual cost: %f to room: %d\n", search_route_list.last().actualCost, search_route_list.last().actualRoom.room);
                }
                else
                {
                    // New active knot has no children

                    // Remove knot
                    search_route_list.RemoveAt(search_route_list.Count - 1);
                    search_route_list.Last().childrenRooms.RemoveAt(search_route_list.Last().childrenRooms.Count - 1);
                    route_list.RemoveAt(route_list.Count - 1);

                    if (search_route_list.Last().childrenRooms.Count > 0)
                    {
                        //qDebug() << "Backtracking with children(s) of room: " << search_route_list.last().actualRoom.room;
                        search_route_t knotRoom = new search_route_t();
                        knotRoom.actualRoom = search_route_list.Last().childrenRooms.Last().actualRoom;
                        knotRoom.actualCost = search_route_list.Last().childrenRooms.Last().actualCost;
                        knotRoom.childrenRooms = new List<search_route_t>();
                        // New active knot
                        search_route_list.Add(knotRoom);
                        search_route_list.Last().done = false;
                        route_list.Add(search_route_list.Last().actualRoom);
                    }
                    else
                    {
                        //qDebug() << "Backtracking without childrens";
                        // No more children
                        search_route_list.Last().done = true;
                    }
                    //printf("\nNo Children, actual cost: %f to room: %d\n", search_route_list.last().actualCost, search_route_list.last().actualRoom.room);
                }

                //printf("\nRoute list: ");
                /*
                for (n = 0; n < route_list.size(); n++)
                {
                    printf("%d ", route_list[n].room);
                }
                printf("\n");

                printf("\nSearch list from parent: %d", search_route_list.last().actualRoom.room);
                for (n = 0; n < search_route_list.last().childrenRooms.size(); n++)
                {
                    printf("%d ", search_route_list.last().childrenRooms[n].actualRoom.room);
                }
                printf("\n");
                */
            }
            else
            {
                //int n;
                route_list.Add(tarRoomPos);
                routes.Add(route_list);
                route_cost = search_route_list.Last().actualCost + distance(search_route_list.ElementAt(search_route_list.Count - 1).actualRoom, tarRoomPos);
                //finished = true;
                Debug.WriteLine("Route cost: " + route_cost.ToString());
                for (int i = 0; i < route_list.Count; i++)
                {
                    Debug.Write(route_list.ElementAt(i).room.ToString() + ' ');
                }
                Debug.WriteLine("\n");
                search_route_list.RemoveAt(search_route_list.Count - 1);
                search_route_list.Last().childrenRooms.RemoveAt(search_route_list.Last().childrenRooms.Count - 1);
                route_list.RemoveAt(route_list.Count - 1);
                search_route_list.Last().done = true;
                /*
                printf("\nRoute list: ");
                for (n = 0; n < route_list.size(); n++)
                {
                    printf("%d ", route_list[n].room);
                }
                printf("\n");
                printf("\nRoute cost: %f\n", route_cost);
                */
                //search_route_list.Clear();
            }
        }

    }

    class AStarRoute : Route
    {
        public AStarRoute()
        {

        }

        protected override int find_children(pos_t start, pos_t destination, int index)
        {
            int i;
            double effort = 0.0;
            double guess = 0.0;
            int size = (rooms.Height * rooms.Width);
            for (i = 0; i < size; i++)
            {
                pos_t next;
                next.room = i + 1;
                next.pos = new PointF(0, 0);
                if( applicable(start, ref next, destination, ref effort, ref guess) != 0)
                {
                    int found = 0;
                    for (int j = 0; j < route_list.Count; j++)
                    {
                        if (route_list.ElementAt(j).room == next.room)
                        {
                            found = 1;
                            break;
                        }
                    }
                    if (found == 0)
                    {
                        cost_t cost;
                        cost.room.room = next.room;
                        cost.room.pos = next.pos;
                        cost.value = effort;
                        cost.sum = effort + guess;
                        cost_list.Add(cost);                 
                    }
                }
            }
            return cost_list.Count;
        }
    }

    class GreedyRoute : Route
    {
        public GreedyRoute()
        {

        }

        protected override int find_children(pos_t start, pos_t destination, int index)
        {
            int i;
            double effort = 0.0;
            double guess = 0.0;
            int size = (rooms.Height * rooms.Width);
            for (i = 0; i < size; i++)
            {
                pos_t next;
                next.room = i + 1;
                next.pos = new PointF(0, 0);
                if (applicable(start, ref next, destination, ref effort, ref guess) != 0)
                {
                    int found = 0;
                    for (int j = 0; j < route_list.Count; j++)
                    {
                        if (route_list.ElementAt(j).room == next.room)
                        {
                            found = 1;
                            break;
                        }
                    }
                    if (found == 0)
                    {
                        cost_t cost;
                        cost.room.room = next.room;
                        cost.room.pos = next.pos;
                        cost.value = effort;
                        cost.sum = guess;
                        cost_list.Add(cost);
                    }
                }
            }
            return cost_list.Count;
        }
    }
}
