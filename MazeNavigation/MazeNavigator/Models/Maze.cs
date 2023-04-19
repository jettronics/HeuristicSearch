using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;

namespace Maze_generator.Models
{
    public class Maze
    {
        public Edge[] Edges { get; }
        public Size Size { get; }
        public Door[] Doors { get; }
 
        public Maze(Size size)
        {
            var edgesToCheck = Edge.Generate(size);
            //var sets = new DisjointSets(size.Height*size.Width);
            var mazeEdges = new List<Edge>();
            var mazeDoors = new List<Door>();

#if 0
            static int currentRoom = 0;
            static int unvisitedRooms = 1;
            static QVector<int> doorsOfRoom;
            static QVector<int> roomsWithNeighbours;
            static bool initOpen = true;

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
                qDebug() << "Init open doors Maze";
                currentRoom = 0;
                unvisitedRooms = 1;
                QTime t = QTime::currentTime();
                qsrand((unsigned int)t.msec());

                //Make the initial cell the current cell and mark it as visited
                currentRoom = (qrand() % maxRooms) + 1;
                roomVisitedMaze[currentRoom - 1] = 1;

                roomsWithNeighbours.clear();
                initOpen = false;
            }
            else
            {
                int chooseRoom = 0;

                //qDebug() << "Current room: " << currentRoom;

                doorsOfRoom.clear();

                for (int i = 0; i < doorPoints.size(); i++)
                {
                    //If the current cell has any neighbours which have not been visited
                    if (doorPoints.at(i)->room1 == currentRoom)
                    {
                        if (roomVisitedMaze[(doorPoints.at(i)->room2) - 1] == 0)
                        {
                            if (doorsOfRoom.contains(doorPoints.at(i)->room2) == false)
                            {
                                doorsOfRoom.push_back(doorPoints.at(i)->room2);
                                //qDebug() << "Push room neighbour: " << doorPoints.at(i)->room2;
                            }
                        }
                    }
                    else
                    if (doorPoints.at(i)->room2 == currentRoom)
                    {
                        if (roomVisitedMaze[(doorPoints.at(i)->room1) - 1] == 0)
                        {
                            if (doorsOfRoom.contains(doorPoints.at(i)->room1) == false)
                            {
                                doorsOfRoom.push_back(doorPoints.at(i)->room1);
                                //qDebug() << "Push room neighbour: " << doorPoints.at(i)->room1;
                            }
                        }
                    }
                }

                if (doorsOfRoom.size() >= 1)
                {
                    //Choose randomly one of the unvisited neighbours
                    if (doorsOfRoom.size() > 1)
                    {
                        int doorsOfRoomIndex = (qrand() % doorsOfRoom.size());
                        chooseRoom = doorsOfRoom.at(doorsOfRoomIndex);
                        //qDebug() << "Push current room: " << currentRoom;
                        roomsWithNeighbours.push_back(currentRoom);
                    }
                    else
                    {
                        chooseRoom = doorsOfRoom.takeFirst();
                    }

                    //qDebug() << "Choose room: " << chooseRoom;

                    if (chooseRoom > 0)
                    {
                        //Remove the wall between the current cell and the chosen cell
                        for (int i = 0; i < doorPoints.size(); i++)
                        {
                            if (((doorPoints.at(i)->room1 == currentRoom) && (doorPoints.at(i)->room2 == chooseRoom)) ||
                                ((doorPoints.at(i)->room2 == currentRoom) && (doorPoints.at(i)->room1 == chooseRoom)))
                            {
                                doorPoints.at(i)->open = true;
                                //qDebug() << "Open door: " << i;
                                currentRoom = chooseRoom;
                                roomVisitedMaze[currentRoom - 1] = 1;
                                break;
                            }
                        }
                    }
                }
                else
                {
                    if (roomsWithNeighbours.size() > 0)
                    {
                        currentRoom = roomsWithNeighbours.takeLast();
                        //qDebug() << "Pop current room: " << currentRoom;
                        roomVisitedMaze[currentRoom - 1] = 1;
                    }
                    else
                    {
                        qDebug() << "Maze finished";
                        unvisitedRooms = 0;
                        initOpen = true;
                    }
                }
                qDebug() << "Room list: " << roomsWithNeighbours;
            }

            return unvisitedRooms;
#endif


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
            Doors = mazeDoors.ToArray();
            Size = size;
        }
    }
}