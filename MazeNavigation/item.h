/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef ITEM_H
#define ITEM_H

#include <QGraphicsPixmapItem>
#include <QList>
#include <QTime>
#include "room.h"
#include "navigator.h"
#include "kalman.h"
#include "robot.h"

QT_BEGIN_NAMESPACE
class QPixmap;
class QGraphicsItem;
class QGraphicsScene;
class QTextEdit;
class QGraphicsSceneMouseEvent;
class QMenu;
class QGraphicsSceneContextMenuEvent;
class QPainter;
class QStyleOptionGraphicsItem;
class QWidget;
class QPolygonF;
QT_END_NAMESPACE


class Item : public QGraphicsPolygonItem
{
public:
    enum { Type = UserType + 15 };
    enum ItemType { Barrier, Target, Roboter, Cover };

    Item(ItemType diagramType, QMenu *contextMenu,
        QGraphicsItem *parent = nullptr, QGraphicsScene *scene = nullptr,
        Robot *robot = nullptr, Kalman *kalman = nullptr, Navigator *navigator = nullptr);

    ItemType itemType() const
        { return myItemType; }
    QPolygonF polygon() const
        { return myPolygon; }
    QPixmap image() const;
    int type() const
        { return Type;}
    int getNo() {return roomNo;};
    void setNo( int no ) { roomNo = no; };

protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    void advance(int phase);

private:
    ItemType myItemType;
    QPolygonF myPolygon;
    QMenu *myContextMenu;
    Robot *robot;
    Kalman *kalman;
    QTime measure;
    QGraphicsScene *scene;
    int roomNo;
    Room *roomcoll;
    Room *room;
    Navigator *navigator;
};


#endif
