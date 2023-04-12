#ifndef HALL_H
#define HALL_H

#include <QtGui>
#include <QDialog>
#include <QSpinBox>
#include <QMainWindow>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QLabel>

#include "door.h"
#include "room.h"

class Hall : public QDialog
{
    Q_OBJECT

public:
    const static int gridSize = 20;
    const static int maxRooms = 100;

    Hall(QMainWindow *window, QMenu *itemMenu);
    virtual ~Hall();

    int getGrid() { return grid; };
    int getRooms() { return rooms; };
    QRectF getDimension() { return hallDimension; };
    void generate( QRectF rect, QPointF offset );
    void generateMaze( QRectF rect, QPointF offset );
    int openDoorsMaze();
    void update();
    Room* getRoom(int roomindex);
    Door* getDoor(int doorindex) { return doorPoints.at(doorindex); };
    int getDoorCount() { return doorPoints.size(); };
    QVector<Door*>* getDoorList() { return &doorPoints; };

private:
    enum MoveType {right, left, up, down};

    void findRoom( int roomindex, int row, int col );
    void findPolygon( int roomindex, int gridwidth, int gridlength, QPointF offset );
    void findDoor( int row, int col, int gridwidth, int gridlength, QPointF offset );
    void sortDoor();
    void closeDoors( int doors );
    void openDoors();
    void doorCoordinates();

private:
    QMainWindow *mainWindow;
    QMenu *itemMenu;
    int hallGrid[gridSize][gridSize];
    Room room[maxRooms];
    int roomVisitedMaze[maxRooms];
    QVector<Door*> doorPoints;
    int rooms;
    int grid;
    int doors;
    QRectF hallDimension;

signals:
    void acceptHallBox();
    void updateHallBox();

private slots:
    void rejectBox();
    void acceptBox();
    void updateDoorBox();

public slots:
    void reject();

private:
    QGridLayout *gridLayout;
    QDialogButtonBox *buttonBox;
    QDialogButtonBox *updateBox;
    QLabel *roomsLabel;
    QSpinBox *roomsBox;
    QLabel *gridLabel;
    QSpinBox *gridBox;
    QLabel *doorLabel;
    QSpinBox *doorBox;
};

#endif // HALL_H
