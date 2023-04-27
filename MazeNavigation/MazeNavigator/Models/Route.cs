using System;
using System.Collections.Generic;
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

        public struct search_route_t
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
        protected List<search_route_t> search_route_list;
        protected double route_cost;
        protected bool finished;
        protected Size rooms;
        protected pos_t tarRoomPos;
        protected float roomWidthLen;
        protected float roomHeightLen;

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
            search_route_list = new List<search_route_t>();
            //route_list.Clear();
            //search_route_list.Clear();
        }

        public virtual void routeStart(int actR, int tarR, Size roomC, List<Door> doorL)
        {
            route_cost = 0;
            
            route_list.Clear();
            search_route_list.Clear();
            
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
            search_route_list.Add(knotRoom);

            finished = false;
        }

        public virtual void routeProcess()
        {

        }

        public abstract void search(pos_t start, pos_t destination, int index, double actual_cost);
    }

    class AStarRoute : Route
    {
        public AStarRoute()
        {

        }

        private struct cost_t
        {
            double value;
            double sum;
            pos_t room;
        }

        private double cost(pos_t room1, pos_t room2)
        {
            return 0.0;
        }
        private int applicable(pos_t start, pos_t next, pos_t destination, double cost, double heuristic)
        {
            return 0;
        }

        private int find_children(pos_t start, pos_t destination, int index, cost_t cost_list)
        {
            return 0;
        }
        
        public override void search(pos_t start, pos_t destination, int index, double actual_cost)
        {

        }
        
        public override void routeProcess()
        {

        }
    }
}
