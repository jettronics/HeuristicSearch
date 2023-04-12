
#ifndef SCENE_H
#define SCENE_H

#include <QGraphicsScene>
#include "robot.h"
#include "kalman.h"
#include "item.h"
#include "textitem.h"
#include "hall.h"
#include "navigator.h"

QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
class QMenu;
class QPointF;
class QGraphicsLineItem;
class QFont;
class QGraphicsTextItem;
class QColor;
QT_END_NAMESPACE

class Scene : public QGraphicsScene
{
    Q_OBJECT

public:
    enum Mode { InsertItem, MoveItem, NoItem };
    enum MazeStateType { InitMaze, GenerateMaze, DrawRoomGrid, DrawRoomNumbers, UpdateMaze, MazeFinished };

    Scene(QMenu *itemMenu, QObject *parent = nullptr, Robot *robot = nullptr, Kalman *kalman = nullptr,
          Hall *hall = nullptr, Navigator *navigator = nullptr);
    virtual ~Scene();

    QFont font() const
        { return myFont; }
    QColor textColor() const
        { return myTextColor; }
    QColor lineColor() const
        { return myLineColor; }
    void setLineColor(const QColor &color);
    void setTextColor(const QColor &color);
    void setFont(const QFont &font);
    void createHall(int rooms, int grid);
    void updateHall();
    Hall* getHall() { return hall; }
    void createNavigation();
    void removeNavigation();
    void triggerMaze() { startMaze = true; }

public slots:
    void setMode(Mode mode);
    void setItemType(Item::ItemType type);
    void editorLostFocus(TextItem *item);
    void process();

signals:
    void itemInserted(Item *item);
    void textInserted(QGraphicsTextItem *item);
    void itemSelected(QGraphicsItem *item);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void wheelEvent(QGraphicsSceneWheelEvent *wheelEvent);


private:
    bool isItemChange(int type);

    Item::ItemType myItemType;
    QMenu *myItemMenu;
    Mode myMode;
    bool leftButtonDown;
    QPointF startPoint;
    QFont myFont;
    QColor myTextColor;
    QColor myLineColor;
    Robot *robot;
    Kalman *kalman;
    Hall *hall;
    Navigator *navigator;
    QGraphicsTextItem *textRoom;
    bool startMaze;
    MazeStateType mazeState;
    int mazeStateCounter;
    QObject *parentWindow;
};

#endif
