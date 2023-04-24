using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Windows.Forms.VisualStyles;

namespace Maze_generator.Models
{
    public class Maze
    {
        public Edge[] Edges { get; }
        public Size Size { get; }
        public Door[] Doors { get; }
        private bool initOpen;
        private int unvisitedRooms;
        private int currentRoom;
        private int maxRooms;
        private int[] roomVisitedMaze;
        private List<int> roomsWithNeighbours;
        private List<int> doorsOfRoom;
        private List<Edge> edgesToCheck;
        //private List<Edge> mazeEdges;
        private List<Door> mazeDoors;

        public Maze(Size size)
        {
            edgesToCheck = Edge.Generate(size);
            //var sets = new DisjointSets(size.Height*size.Width);
            //mazeEdges = new List<Edge>();
            mazeDoors = new List<Door>();

            initOpen = true;
            maxRooms = size.Height * size.Width;

            roomVisitedMaze = new int[maxRooms];
            doorsOfRoom = new List<int>();
            roomsWithNeighbours = new List<int>();

            roomsWithNeighbours.Clear();

            for (int i = 0; i < maxRooms; i++)
            {
                roomVisitedMaze[i] = 0;
            }

            do
            {
                unvisitedRooms = CreateDoors(); 
            }
            while (unvisitedRooms == 1);

            /*
            var random = new Random();
            
            while (sets.Count > 1 && edgesToCheck.Count > 0)
            {
                // Pick a random edge
                var edgeIndex = random.Next(edgesToCheck.Count);
                var edge = edgesToCheck[edgeIndex];

                // Find the two sets seperated by the edge
                var set1 = sets.Find(edge.Cell1);
                var set2 = sets.Find(edge.Cell2);

                if (set1 != set2)
                {
                    sets.Union(set1, set2);
                }
                else
                {
                    mazeEdges.Add(edge);
                }

                // Remove edge, so it won't be checked again
                //edgesToCheck.RemoveAt(edgeIndex);

            }
            
            // Set maze properties
            Edges = edgesToCheck.Concat(mazeEdges).ToArray();
            */
            Edges = edgesToCheck.ToArray();
            //Edges = mazeEdges.ToArray();
            Doors = mazeDoors.ToArray();
            Size = size;
        }

        private int CreateDoors()
        {
            int ret = 1;

            /*
                The depth-first search algorithm of maze generation is frequently implemented using backtracking:

                Make the initial cell the current cell and mark it as visited
                While there are unvisited cells
                    If the current cell has any neighbours which have not been visited
                        Choose randomly one of the unvisited neighbours
                        Push the current cell to the stack if it has more than one unvisited neighbor
                        Remove the wall between the current cell and the chosen cell
                        Make the chosen cell the current cell and mark it as visited
                    Else if stack is not empty
                        Pop a cell from the stack while the stack is not empty and the popped cell has no unvisited neighbors
                        Make it the current cell
            */

            if (initOpen == true)
            {
                unvisitedRooms = 1;
                
                var random = new Random();
                currentRoom = random.Next(maxRooms);
                roomVisitedMaze[currentRoom] = 1;

                initOpen = false;
            }
            else
            {
                int chooseRoom = -1;

                doorsOfRoom.Clear();

                for( int i=0; i < edgesToCheck.Count; i++ )
                { 
                    if( edgesToCheck.ElementAt(i).Cell1 == currentRoom )
                    {
                        if( roomVisitedMaze[edgesToCheck.ElementAt(i).Cell2] == 0 )
                        {
                            if( doorsOfRoom.Contains(edgesToCheck.ElementAt(i).Cell2) == false )
                            {
                                doorsOfRoom.Add(edgesToCheck.ElementAt(i).Cell2);
                            }
                        }
                    }
                    else
                    if(edgesToCheck.ElementAt(i).Cell2 == currentRoom)
                    {
                        if( roomVisitedMaze[edgesToCheck.ElementAt(i).Cell1] == 0 )
                        {
                            if(doorsOfRoom.Contains(edgesToCheck.ElementAt(i).Cell1) == false)
                            {
                                doorsOfRoom.Add(edgesToCheck.ElementAt(i).Cell1);
                            }
                        }
                    }
                }

                if (doorsOfRoom.Count >= 1)
                {
                    //Choose randomly one of the unvisited neighbours
                    if (doorsOfRoom.Count > 1)
                    {
                        var randomCellIndex = new Random();
                        int doorsOfRoomIndex = randomCellIndex.Next(doorsOfRoom.Count);

                        chooseRoom = doorsOfRoom.ElementAt(doorsOfRoomIndex);
                        roomsWithNeighbours.Add(currentRoom);
                    }
                    else
                    {
                        chooseRoom = doorsOfRoom.First();
                        doorsOfRoom.RemoveAt(0);
                    }

                    if (chooseRoom >= 0)
                    {
                        //Remove the wall between the current cell and the chosen cell
                        for (int i = 0; i < edgesToCheck.Count; i++)
                        {
                            if (((edgesToCheck.ElementAt(i).Cell1 == currentRoom) && (edgesToCheck.ElementAt(i).Cell2 == chooseRoom)) ||
                                ((edgesToCheck.ElementAt(i).Cell2 == currentRoom) && (edgesToCheck.ElementAt(i).Cell1 == chooseRoom)))
                            {
                                //Open door
                                Door newDoor = new Door(edgesToCheck.ElementAt(i).Cell1, edgesToCheck.ElementAt(i).Cell2);
                                mazeDoors.Add(newDoor);
                                currentRoom = chooseRoom;
                                roomVisitedMaze[currentRoom] = 1;
                                edgesToCheck.RemoveAt(i);
                                break;
                            }
                        }
                    }
                }
                else
                {
                    if (roomsWithNeighbours.Count > 0)
                    {
                        currentRoom = roomsWithNeighbours.Last();
                        int last = roomsWithNeighbours.Count - 1;
                        roomsWithNeighbours.RemoveAt(last);
                        //Pop current room
                        roomVisitedMaze[currentRoom] = 1;
                    }
                    else
                    {
                        //Maze finished
                        ret = 0;
                        initOpen = true;
                    }
                }
            }

            return ret;

        }
    }
}