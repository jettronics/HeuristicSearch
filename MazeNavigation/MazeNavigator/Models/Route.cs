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
            int room;
            Point pos;
        }

        public struct search_route_t
        {
            pos_t actualRoom;
            double actualCost;
            bool done;
            List<search_route_t> childrenRooms;
        }

        protected Point actPos;
        protected Point tarPos;
        protected int actRoom;
        protected int tarRoom;
        protected List<Door> doorList;
        protected List<pos_t> route_list;
        protected List<search_route_t> search_route_list;
        protected double route_cost;
        protected bool finished;
        protected int rooms;
        protected pos_t tarRoomPos;

        public void setActualPos(Point p) { actPos = p; }
        public void setTargetPos(Point p) { tarPos = p; }
        public void setActualRoom(int room) { actRoom = room; }
        public void setTargetRoom(int room) { tarRoom = room; }
        public void setRooms(int count) { rooms = count; }
        public void setDoorList(List<Door> p) { doorList = p; }
        public List<pos_t> getRoute() { return route_list; }
        public double getRouteCost() { return route_cost; }
        public bool getFinished() { return finished; }

        public Route()
        {
            finished = false;
            actRoom = 0;
            tarRoom = 0;
            //route_list.Clear();
            //search_route_list.Clear();
        }

        public virtual void route()
        {

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
