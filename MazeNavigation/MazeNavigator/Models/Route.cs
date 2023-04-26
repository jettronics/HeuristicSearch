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
            pos_t actualRoom;
            double actualCost;
            bool done;
            List<search_route_t> childrenRooms;
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

        public void setActualRoom(int room) { actRoom = room; }
        public void setTargetRoom(int room) { tarRoom = room; }
        public void setRooms(Size count) { rooms = count; }
        public void setDoorList(List<Door> p) { doorList = p; }
        public List<pos_t> getRoute() { return route_list; }
        public double getRouteCost() { return route_cost; }
        public bool getFinished() { return finished; }

        public Route()
        {
            finished = false;
            actRoom = 0;
            tarRoom = 0;
            rooms.Width = 0;
            rooms.Height = 0;
            roomWidthLen = 0.0f;
            //route_list.Clear();
            //search_route_list.Clear();
        }

        public virtual void route()
        {
            route_cost = 0;
            if( route_list.Count > 0 )
            {
                route_list.Clear();
            }
            
            finished = false;

            int actRoom = 0;
            int tarRoom = (rooms.Width * rooms.Height) - 1;
            
            pos_t startpos;
            startpos.room = actRoom;
            roomWidthLen = (1.0f / ((float)rooms.Width));
            //actPos.X = ((float)(actRoom % rooms.Width)) * ;
            startpos.pos = actPos;
            pos_t endpos;
            endpos.room = tarRoom;
            endpos.pos = tarPos;
            /*

            route_list.push_back(startpos);

            search(route_list[0], endpos, 1, 0.0);

            if (route_list.size() <= 1)
            {
                route_list.clear();
            }*/
            
        }

        public virtual void routeStart()
        {

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
