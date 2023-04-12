
#include <QtGui>
#include <QTime>
#include <QDebug>
#include "string.h"

#include "room.h"
#include "hall.h"


Hall::Hall(QMainWindow *window, QMenu *itemMenu)
    : mainWindow(window)
    , itemMenu(itemMenu)
    , rooms(100)
    , grid(10)
    , doors(0)
    , hallDimension(18000,8000,4000,4000)
{
    resize(330, 160);

    gridLayout = new QGridLayout;
    roomsLabel = new QLabel(tr("Number of rooms:"));

    gridLayout->addWidget(roomsLabel, 0, 0, 1, 1);

    roomsBox = new QSpinBox();
    roomsBox->setMaximum(maxRooms);
    roomsBox->setMinimum(0);
    roomsBox->setValue(rooms);

    gridLayout->addWidget(roomsBox, 0, 1, 1, 1);

    gridLabel = new QLabel(tr("Gridsize:"));

    gridLayout->addWidget(gridLabel, 1, 0, 1, 1);

    gridBox = new QSpinBox();
    gridBox->setValue(grid);

    gridLayout->addWidget(gridBox, 1, 1, 1, 1);

    doorLabel = new QLabel(tr("Percentage of Doors:"));

    gridLayout->addWidget(doorLabel, 2, 0, 1, 1);

    doorBox = new QSpinBox();
    doorBox->setMaximum(100);
    doorBox->setMinimum(0);
    doorBox->setValue(doors);

    gridLayout->addWidget(doorBox, 2, 1, 1, 1);

    buttonBox = new QDialogButtonBox();
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->addButton("Create", QDialogButtonBox::AcceptRole);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel);

    updateBox = new QDialogButtonBox();
    updateBox->setOrientation(Qt::Horizontal);
    updateBox->addButton("Update", QDialogButtonBox::AcceptRole);
    updateBox->setEnabled(false);

    gridLayout->addWidget(buttonBox, 3, 0, 1, 2);
    gridLayout->addWidget(updateBox, 2, 2, 1, 1);
    setLayout(gridLayout);

    connect(this, SIGNAL(acceptHallBox()), mainWindow, SLOT(acceptHallBox()));
    connect(this, SIGNAL(updateHallBox()), mainWindow, SLOT(updateHallBox()));
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(acceptBox()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(rejectBox()));
    connect(updateBox, SIGNAL(accepted()), this, SLOT(updateDoorBox()));

    setWindowTitle(tr("Hall creation"));
}

Hall::~Hall()
{
   for( int i=0; doorPoints.size(); i++ )
   {
       Door* door = doorPoints[i];
       delete door;
   }
   doorPoints.clear();
}

void Hall::rejectBox()
{
    hide();
}

void Hall::reject()
{
    hide();
}

void Hall::acceptBox()
{
    grid = gridBox->value();
    rooms = roomsBox->value();
    doors = doorBox->value();
    updateBox->setEnabled(true);
    emit acceptHallBox();
}

void Hall::updateDoorBox()
{
   doors = doorBox->value();
   emit updateHallBox();
}

void Hall::findRoom(int roomindex, int row, int col)
{
    int k,l;
    for( int i=0; i<row; i++ )
    {
        for( int j=0; j<col; j++ )
        {
            if( hallGrid[i][j] == roomindex )
            {
                k = i;
                l = j;
                break;
            }
        }
    }
    // x->
    for( int j=1; (l+j)<=col; j++ )
    {
        if( l+j == col )
        {
            room[roomindex-1].add(k, l+j-1);
        }
        else
        if( hallGrid[k][l+j] == -1 )
        {
            hallGrid[k][l+j] = roomindex;
        }
        else
        {
            room[roomindex-1].add(k, l+j-1);
            break;
        }
    }
    // <-x
    for( int j=1; (l-j)>=-1; j++ )
    {
        if( l-j == -1 )
        {
            room[roomindex-1].add(k, l-j+1);
        }
        else
        if( hallGrid[k][l-j] == -1 )
        {
           hallGrid[k][l-j] = roomindex;
        }
        else
        {
            room[roomindex-1].add(k, l-j+1);
            break;
        }
    }

    QPointF pointr = room[roomindex-1].get(0);
    QPointF pointl = room[roomindex-1].get(1);
    bool possible = true;
    // y
    // |
    for( int i=1; (k+i)<=row; i++ )
    {
        int last = -1;
        if( (k+i) == row )
        {
            last = k+i-1;
        }
        else
        {
            for( int j=pointl.x(); j<=pointr.x(); j++ )
            {
                if( hallGrid[k+i][j] != -1 )
                {
                   possible = false;
                   break;
                }
            }
            if( possible == true )
            {
                for( int j=pointl.x(); j<=pointr.x(); j++ )
                {
                    hallGrid[k+i][j] = roomindex;
                }
            }
            else
            {
                last = k+i-1;
            }
        }
        if( last != -1 )
        {
            room[roomindex-1].add(last, pointl.x());
            room[roomindex-1].add(last, pointr.x());
            break;
        }
    }
    possible = true;
    // |
    // y
    for( int i=1; (k-i)>=-1; i++ )
    {
        int last = -1;
        if( (k-i) == -1 )
        {
            last = k-i+1;
        }
        else
        {
            for( int j=pointl.x(); j<=pointr.x(); j++ )
            {
                if( hallGrid[k-i][j] != -1 )
                {
                    possible = false;
                    break;
                }
            }
            if( possible == true )
            {
                for( int j=pointl.x(); j<=pointr.x(); j++ )
                {
                    hallGrid[k-i][j] = roomindex;
                }
            }
            else
            {
                last = k-i+1;
            }
        }
        if( last != -1 )
        {
            if(room[roomindex-1].size() >= 4 )
            {
               room[roomindex-1].remove(0);
               room[roomindex-1].remove(0);
            }
            room[roomindex-1].add(last, pointr.x());
            room[roomindex-1].add(last, pointl.x());
            break;
        }
    }
}

void Hall::findPolygon( int roomindex, int gridwidth, int gridlength, QPointF offset )
{
    QPointF p;
    for( int i=0; i<4; i++ )
    {
        p = room[roomindex-1].get(i);
        room[roomindex-1].set(i, (p.y()*gridwidth)+(gridwidth/2), (p.x()*gridlength)+(gridlength/2));
    }

    // 1. left/down, 2. right/down, 3. right/up, 4. left/up
    p = room[roomindex-1].get(0);
    p += offset;
    room[roomindex-1].set(0, (p.y())+(gridwidth/2), (p.x())-(gridlength/2));
    p = room[roomindex-1].get(1);
    p += offset;
    room[roomindex-1].set(1, (p.y())+(gridwidth/2), (p.x())+(gridlength/2));
    p = room[roomindex-1].get(2);
    p += offset;
    room[roomindex-1].set(2, (p.y())-(gridwidth/2), (p.x())+(gridlength/2));
    p = room[roomindex-1].get(3);
    p += offset;
    room[roomindex-1].set(3, (p.y())-(gridwidth/2), (p.x())-(gridlength/2));
}

void Hall::findDoor( int row, int col, int gridwidth, int gridlength, QPointF offset )
{
    // Find neighbour
    for( int i=0; i<row; i++ )
    {
        for( int j=0; j<col; j++ )
        {
            if( i < row-1 )
            {
                if( hallGrid[i][j] != hallGrid[i+1][j] )
                {
                    Door *door = new Door();
                    door->room1 = hallGrid[i][j];
                    door->room2 = hallGrid[i+1][j];
                    door->center.setX((j*gridlength)+(gridlength/2)+offset.x());
                    door->outa.setX(door->center.x());
                    door->outb.setX(door->center.x());
                    door->outa.setY((i*gridwidth)+(gridwidth/2)+offset.y());
                    door->outb.setY(((i+1)*gridwidth)+(gridwidth/2)+offset.y());
                    door->center.setY((door->outa.y() + door->outb.y())/2);
                    doorPoints.push_back(door);
                }
            }
            if( i > 0 )
            {
                if( hallGrid[i][j] != hallGrid[i-1][j] )
                {
                    Door *door = new Door();
                    door->room1 = hallGrid[i][j];
                    door->room2 = hallGrid[i-1][j];
                    door->center.setX((j*gridlength)+(gridlength/2)+offset.x());
                    door->outa.setX(door->center.x());
                    door->outb.setX(door->center.x());
                    door->outa.setY((i*gridwidth)+(gridwidth/2)+offset.y());
                    door->outb.setY(((i-1)*gridwidth)-(gridwidth/2)+offset.y());
                    door->center.setY((door->outa.y() + door->outb.y())/2);
                    doorPoints.push_back(door);
                }
            }
            if( j < col-1 )
            {
                if( hallGrid[i][j] != hallGrid[i][j+1] )
                {
                    Door *door = new Door();
                    door->room1 = hallGrid[i][j];
                    door->room2 = hallGrid[i][j+1];
                    door->center.setY((i*gridwidth)+(gridwidth/2)+offset.y());
                    door->outa.setY(door->center.y());
                    door->outb.setY(door->center.y());
                    door->outa.setX((j*gridlength)+(gridlength/2)+offset.x());
                    door->outb.setX(((j+1)*gridlength)+(gridlength/2)+offset.x());
                    door->center.setX((door->outa.x() + door->outb.x())/2);
                    doorPoints.push_back(door);
                }
            }
            if( j > 0 )
            {
                if( hallGrid[i][j] != hallGrid[i][j-1] )
                {
                    Door *door = new Door();
                    door->room1 = hallGrid[i][j];
                    door->room2 = hallGrid[i][j-1];
                    door->center.setY((i*gridwidth)+(gridwidth/2)+offset.y());
                    door->outa.setY(door->center.y());
                    door->outb.setY(door->center.y());
                    door->outa.setX((j*gridlength)+(gridlength/2)+offset.x());
                    door->outb.setX(((j-1)*gridlength)-(gridlength/2)+offset.x());
                    door->center.setX((door->outa.x() + door->outb.x())/2);
                    doorPoints.push_back(door);
                }
            }
        }
    }
}

void Hall::sortDoor( void )
{
    for( int i=0; i<(doorPoints.size()-1); i++)
    {
        for( int j=i+1; j<doorPoints.size(); j++)
        {
            if( ((doorPoints[i]->room1 == doorPoints[j]->room1) &&
                 (doorPoints[i]->room2 == doorPoints[j]->room2)) ||
                ((doorPoints[i]->room1 == doorPoints[j]->room2) &&
                 (doorPoints[i]->room2 == doorPoints[j]->room1)) )
            {
               Door *door = doorPoints[j];
               doorPoints.remove(j);
               delete door;
               j--;
            }
        }
    }
}

void Hall::doorCoordinates(void)
{
   for( int i=0; i<doorPoints.size(); i++)
   {
      if( doorPoints[i]->outa.x() == doorPoints[i]->outb.x() )
      {
         int mean_x = 0;
         // vertical
         QPointF r1p1 = room[(doorPoints[i]->room1)-1].get(1);
         QPointF r1p0 = room[(doorPoints[i]->room1)-1].get(0);
         QPointF r2p1 = room[(doorPoints[i]->room2)-1].get(1);
         QPointF r2p0 = room[(doorPoints[i]->room2)-1].get(0);

         if( (r1p1.x() >= r2p1.x()) && (r1p0.x() <= r2p0.x()) )
         {
            mean_x = (r2p1.x() + r2p0.x())/2;
         }
         else
         if( (r1p1.x() <= r2p1.x()) && (r1p0.x() >= r2p0.x()) )
         {
            mean_x = (r1p1.x() + r1p0.x())/2;
         }
         else
         if( (r1p1.x() <= r2p1.x()) && (r1p0.x() <= r2p0.x()) )
         {
            mean_x = (r1p1.x() + r2p0.x())/2;
         }
         else
         {
            mean_x = (r2p1.x() + r1p0.x())/2;
         }

         int mean_y = (doorPoints[i]->outa.y() + doorPoints[i]->outb.y())/2;
         doorPoints[i]->outa.setY(mean_y);
         doorPoints[i]->outb.setY(mean_y);
         doorPoints[i]->center.setY(mean_y);
         doorPoints[i]->center.setX(mean_x);
         doorPoints[i]->outa.setX(mean_x - 75);
         doorPoints[i]->outb.setX(mean_x + 75);
      }
      else
      if( doorPoints[i]->outa.y() == doorPoints[i]->outb.y() )
      {
         int mean_y = 0;
         // horizontal
         QPointF r1p1 = room[doorPoints[i]->room1-1].get(1);
         QPointF r1p2 = room[doorPoints[i]->room1-1].get(2);
         QPointF r2p1 = room[doorPoints[i]->room2-1].get(1);
         QPointF r2p2 = room[doorPoints[i]->room2-1].get(2);

         if( (r1p1.y() >= r2p1.y()) && (r1p2.y() <= r2p2.y()) )
         {
            mean_y = (r2p1.y() + r2p2.y())/2;
         }
         else
         if( (r1p1.y() <= r2p1.y()) && (r1p2.y() >= r2p2.y()) )
         {
            mean_y = (r1p1.y() + r1p2.y())/2;
         }
         else
         if( (r1p1.y() <= r2p1.y()) && (r1p2.y() <= r2p2.y()) )
         {
            mean_y = (r1p1.y() + r2p2.y())/2;
         }
         else
         {
            mean_y = (r2p1.y() + r1p2.y())/2;
         }

         int mean_x = (doorPoints[i]->outa.x() + doorPoints[i]->outb.x())/2;
         doorPoints[i]->outa.setY(mean_y - 75);
         doorPoints[i]->outb.setY(mean_y + 75);
         doorPoints[i]->center.setY(mean_y);
         doorPoints[i]->center.setX(mean_x);
         doorPoints[i]->outa.setX(mean_x);
         doorPoints[i]->outb.setX(mean_x);
      }
      doorPoints[i]->setLine();
   }
}

void Hall::closeDoors( int doors )
{
   int doorsClose = doorPoints.size()-(doors*doorPoints.size()/100);
   qDebug() << "Close " << doorsClose << " doors of " << doorPoints.size();
   if( doors == 0 )
   {
       for( int i=0; i<doorsClose; i++ )
       {
          doorPoints[i]->open = false;
       }
   }
   else
   {
       for( int i=0; i<doorsClose; i++ )
       {
          int j = rand() % doorPoints.size();
          doorPoints[j]->open = false;
       }
   }
}

void Hall::openDoors()
{
   for( int i=0; i<doorPoints.size(); i++ )
   {
      doorPoints[i]->open = true;
   }
}

int Hall::openDoorsMaze()
{
    static int currentRoom = 0;
    static int unvisitedRooms = 1;
    static QVector<int> doorsOfRoom;
    static QVector<int> roomsWithNeighbours;
    static bool initOpen = true;

#if 0
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
#endif

    if( initOpen == true )
    {
        qDebug() << "Init open doors Maze";
        currentRoom = 0;
        unvisitedRooms = 1;
        QTime t = QTime::currentTime();
        qsrand((unsigned int)t.msec());

        //Make the initial cell the current cell and mark it as visited
        currentRoom = (qrand()%maxRooms)+1;
        roomVisitedMaze[currentRoom-1] = 1;

        roomsWithNeighbours.clear();
        initOpen = false;
    }
    else
    {
        int chooseRoom = 0;

        //qDebug() << "Current room: " << currentRoom;

        doorsOfRoom.clear();

        for( int i=0; i<doorPoints.size(); i++ )
        {
           //If the current cell has any neighbours which have not been visited
           if( doorPoints.at(i)->room1 == currentRoom )
           {
              if( roomVisitedMaze[(doorPoints.at(i)->room2)-1] == 0 )
              {
                 if( doorsOfRoom.contains(doorPoints.at(i)->room2) == false )
                 {
                    doorsOfRoom.push_back(doorPoints.at(i)->room2);
                    //qDebug() << "Push room neighbour: " << doorPoints.at(i)->room2;
                 }
              }
           }
           else
           if( doorPoints.at(i)->room2 == currentRoom )
           {
              if( roomVisitedMaze[(doorPoints.at(i)->room1)-1] == 0 )
              {
                 if( doorsOfRoom.contains(doorPoints.at(i)->room1) == false )
                 {
                    doorsOfRoom.push_back(doorPoints.at(i)->room1);
                    //qDebug() << "Push room neighbour: " << doorPoints.at(i)->room1;
                 }
              }
           }
        }

        if( doorsOfRoom.size() >= 1 )
        {
            //Choose randomly one of the unvisited neighbours
            if( doorsOfRoom.size() > 1 )
            {
                int doorsOfRoomIndex = (qrand()%doorsOfRoom.size());
                chooseRoom = doorsOfRoom.at(doorsOfRoomIndex);
                //qDebug() << "Push current room: " << currentRoom;
                roomsWithNeighbours.push_back(currentRoom);
            }
            else
            {
                chooseRoom = doorsOfRoom.takeFirst();
            }

            //qDebug() << "Choose room: " << chooseRoom;

            if( chooseRoom > 0 )
            {
                //Remove the wall between the current cell and the chosen cell
                for( int i=0; i<doorPoints.size(); i++ )
                {
                    if( ((doorPoints.at(i)->room1 == currentRoom) && (doorPoints.at(i)->room2 == chooseRoom)) ||
                        ((doorPoints.at(i)->room2 == currentRoom) && (doorPoints.at(i)->room1 == chooseRoom)) )
                    {
                        doorPoints.at(i)->open = true;
                        //qDebug() << "Open door: " << i;
                        currentRoom = chooseRoom;
                        roomVisitedMaze[currentRoom-1] = 1;
                        break;
                    }
                }
            }
        }
        else
        {
            if( roomsWithNeighbours.size() > 0 )
            {
                currentRoom = roomsWithNeighbours.takeLast();
                //qDebug() << "Pop current room: " << currentRoom;
                roomVisitedMaze[currentRoom-1] = 1;
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
}

void Hall::generate( QRectF rect, QPointF offset )
{
    int row = grid;
    int col = grid;

    hallDimension = rect; //.moveTo(offset));
    hallDimension.moveTo(offset);
    qDebug() << "Hall dimension: " << hallDimension;

    for( int i=0; i<maxRooms; i++ )
    {
        room[i].clear();
    }
    for( int i=0; i<doorPoints.size(); i++ )
    {
        Door* door = doorPoints[i];
        delete door;
    }
    doorPoints.clear();

    for( int i=0; i<maxRooms; i++ )
    {
        room[i].setNo(i+1);
    }

    for( int i=0; i<row; i++ )
    {
        for( int j=0; j<col; j++ )
        {
            hallGrid[i][j] = -1;
        }
    }

    QTime t = QTime::currentTime();
    qsrand((unsigned int)t.msec());

    int randRow, randCol;

    printf("\n\n");

    for( int i=0; i<rooms; i++ )
    {
        randRow = qrand()%row;
        randCol = qrand()%col;
        if(hallGrid[randRow][randCol] != -1)
        {
            i--;
        }
        else
        {
            hallGrid[randRow][randCol] = i+1;
            qDebug() << "Room: " << i+1 << " randRow: " << randRow << " randCol: " << randCol;
        }
    }

    for( int i=0; i<row; i++ )
    {
        printf("\n");
        for( int j=0; j<col; j++ )
        {
            if( (hallGrid[i][j] == -1) || (hallGrid[i][j] >= 10) )
            {
                printf(" %d", hallGrid[i][j]);
            }
            else
            {
                printf("  %d", hallGrid[i][j]);
            }
        }
    }

    int rasterw = rect.width()/col;
    int rasterh = rect.height()/row;
    printf("\n\nRaster height: %d, width: %d\n", rasterh, rasterw);

    printf("\n\n");

    for( int i=0; i<rooms; i++ )
    {
        findRoom(i+1, row, col);
        printf("\n Room %d: ", i+1);
        for( int j=0; j<room[i].size(); j++ )
        {
            QPointF p = room[i].get(j);
            printf("(%d %d) ", (int)p.y(), (int)p.x());
        }
        findPolygon(i+1, rasterh, rasterw, offset);
        printf("\n Polygon %d: ", i+1);
        for( int j=0; j<room[i].size(); j++ )
        {
            QPointF p = room[i].get(j);
            printf("(%d %d) ", (int)p.y(), (int)p.x());
        }
        room[i].setPointsToPolygon();
    }
    printf("\n");
    for( int i=0; i<row; i++ )
    {
       printf("\n");
       for( int j=0; j<col; j++ )
       {
          if( hallGrid[i][j] >= 10 )
          {
             printf(" %d", hallGrid[i][j]);
          }
          else
          {
             printf("  %d", hallGrid[i][j]);
          }
       }
    }
    printf("\n\n");
    findDoor(row, col, rasterh, rasterw, offset);
    sortDoor();
    doorCoordinates();
    closeDoors(doors);

    for( int i=0; i<doorPoints.size(); i++ )
    {
       printf("\nDoor %d %d (%d %d) (%d %d) (%d %d): Room: %d, %d",
         i,
         doorPoints[i]->open,
         (int)doorPoints[i]->center.y(), (int)doorPoints[i]->center.x(),
         (int)doorPoints[i]->outa.y(), (int)doorPoints[i]->outa.x(),
         (int)doorPoints[i]->outb.y(), (int)doorPoints[i]->outb.x(),
         doorPoints[i]->room1, doorPoints[i]->room2);
    }
    printf("\n");
}

void Hall::generateMaze( QRectF rect, QPointF offset )
{
    int row = grid;
    int col = grid;

    hallDimension = rect; //.moveTo(offset));
    hallDimension.moveTo(offset);
    qDebug() << "Hall dimension: " << hallDimension;

    for( int i=0; i<maxRooms; i++ )
    {
        room[i].clear();
        roomVisitedMaze[i] = 0;
    }
    for( int i=0; i<doorPoints.size(); i++ )
    {
        Door* door = doorPoints[i];
        delete door;
    }
    doorPoints.clear();

    for( int i=0; i<maxRooms; i++ )
    {
        room[i].setNo(i+1);
    }

    for( int i=0; i<row; i++ )
    {
        for( int j=0; j<col; j++ )
        {
            hallGrid[i][j] = -1;
        }
    }

    //printf("\n\n");

    int roomcounter = 0;

    for( int i=0; i<row; i++ )
    {
        //printf("\n");
        for( int j=0; j<col; j++ )
        {
            roomcounter++;
            hallGrid[i][j] = roomcounter;
            /*
            if( (hallGrid[i][j] == -1) || (hallGrid[i][j] >= 10) )
            {
                printf(" %d", hallGrid[i][j]);
            }
            else
            {
                printf("  %d", hallGrid[i][j]);
            }
            */
        }
    }

    int rasterw = rect.width()/col;
    int rasterh = rect.height()/row;
    qDebug() << "Raster height: " << rasterh << ", width: " << rasterw;

    //printf("\n\n");

    for( int i=0; i<rooms; i++ )
    {
        findRoom(i+1, row, col);
        //printf("\n Room %d: ", i+1);
        /*
        for( int j=0; j<room[i].size(); j++ )
        {
            QPointF p = room[i].get(j);
            printf("(%d %d) ", (int)p.y(), (int)p.x());
        }
        */
        findPolygon(i+1, rasterh, rasterw, offset);
        //printf("\n Polygon %d: ", i+1);
        /*
        for( int j=0; j<room[i].size(); j++ )
        {
            QPointF p = room[i].get(j);
            printf("(%d %d) ", (int)p.y(), (int)p.x());
        }
        */
        room[i].setPointsToPolygon();
    }
    /*
    printf("\n");
    for( int i=0; i<row; i++ )
    {
       printf("\n");
       for( int j=0; j<col; j++ )
       {
          if( hallGrid[i][j] >= 10 )
          {
             printf(" %d", hallGrid[i][j]);
          }
          else
          {
             printf("  %d", hallGrid[i][j]);
          }
       }
    }
    */
    //printf("\n\n");
    findDoor(row, col, rasterh, rasterw, offset);
    sortDoor();
    doorCoordinates();
    closeDoors(0);

    //openDoorsMaze();
    /*
    for( int i=0; i<doorPoints.size(); i++ )
    {
       printf("\nDoor %d %d (%d %d) (%d %d) (%d %d): Room: %d, %d",
         i,
         doorPoints[i]->open,
         (int)doorPoints[i]->center.y(), (int)doorPoints[i]->center.x(),
         (int)doorPoints[i]->outa.y(), (int)doorPoints[i]->outa.x(),
         (int)doorPoints[i]->outb.y(), (int)doorPoints[i]->outb.x(),
         doorPoints[i]->room1, doorPoints[i]->room2);
    }
    */
    //printf("\n");
}

void Hall::update()
{
    //openDoors();
    //closeDoors(doors);
    int unvisited = 1;
    do
    {
        unvisited = openDoorsMaze();
    }
    while( unvisited == 1);
}

Room* Hall::getRoom(int roomindex)
{
    return &room[roomindex-1];
}
