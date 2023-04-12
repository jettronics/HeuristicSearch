
#include <QtGui>
#include <QtCore/QtMath>
#include <QGridLayout>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <math.h>

#include "robot.h"

#define PI 3.14159265

Robot::Robot()
    : maxSpeed(20.0)
    , oldMaxSpeed(maxSpeed)
    , acceleration(10.0)
    , deceleration(-10.0)
    , position(QPointF(0,0))
    , oldpos(QPointF(0,0))
    , collstr("")
    , collstrold("")
    , mode(DragDrop)
    , s_x(0)
    , s_y(0)
    , v_x(0)
    , v_y(0)
    , a_x(0)
    , a_y(0)
    , phi(0)
    , finish(false)
    , sensorPosAvailable(false)
    , sensorPosRange(100.0)
    , driveUp(0.0)
    , rotAngle(0.0)
    , angleRadOld(0.0)
    , sensorLost(false)
{
    QTime t = QTime::currentTime();
    resize(330, 230);

    destination.clear();

    gridLayout = new QGridLayout;
    maxSpeedLabel = new QLabel(tr("Max. speed:"));

    gridLayout->addWidget(maxSpeedLabel, 0, 0, 1, 1);

    speedMaxBox = new QDoubleSpinBox();
    speedMaxBox->setValue(maxSpeed);

    gridLayout->addWidget(speedMaxBox, 0, 1, 1, 1);

    maxSpeedUnitLabel = new QLabel(tr("pixel/s"));

    gridLayout->addWidget(maxSpeedUnitLabel, 0, 2, 1, 1);

    accLabel = new QLabel(tr("Acceleration:"));

    gridLayout->addWidget(accLabel, 1, 0, 1, 1);

    accBox = new QDoubleSpinBox;
    accBox->setValue(acceleration);

    gridLayout->addWidget(accBox, 1, 1, 1, 1);

    accUnitLabel = new QLabel(tr("pixel/s^2"));

    gridLayout->addWidget(accUnitLabel, 1, 2, 1, 1);

    decLabel = new QLabel(tr("Deceleration:"));

    gridLayout->addWidget(decLabel, 2, 0, 1, 1);

    decBox = new QDoubleSpinBox;
    decBox->setMinimum(-100);
    decBox->setValue(deceleration);

    gridLayout->addWidget(decBox, 2, 1, 1, 1);

    decUnitLabel = new QLabel(tr("pixel/s^2"));

    gridLayout->addWidget(decUnitLabel, 2, 2, 1, 1);

    senAvailBox = new QCheckBox(tr("Sensor"));

    gridLayout->addWidget(senAvailBox, 3, 0, 1, 1);

    senBox = new QDoubleSpinBox;
    senBox->setMaximum(1000);
    senBox->setValue(sensorPosRange);

    gridLayout->addWidget(senBox, 3, 1, 1, 1);

    senUnitLabel = new QLabel(tr("pixel"));

    gridLayout->addWidget(senUnitLabel, 3, 2, 1, 1);

    senLostBox = new QCheckBox(tr("Lost Signal"));

    gridLayout->addWidget(senLostBox, 4, 0, 1, 1);

    buttonBox = new QDialogButtonBox();
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

    gridLayout->addWidget(buttonBox, 5, 1, 1, 2);
    setLayout(gridLayout);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(acceptBox()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(rejectBox()));

    setWindowTitle(tr("Robot Settings"));
    qsrand((unsigned int)t.msec());

}


void Robot::acceptBox()
{
    maxSpeed = speedMaxBox->value();
    acceleration = accBox->value();
    deceleration = decBox->value();
    if( sensorPosRange != senBox->value() )
    {
        emit changedSensorRange(sensorPosRange);
    }
    sensorPosRange = senBox->value();
    sensorPosAvailable = senAvailBox->isChecked();
    sensorLost = senLostBox->isChecked();
    hide();
}

void Robot::rejectBox()
{
    hide();
}

void Robot::reject()
{
    hide();
}

void Robot::setRotation( int rotation )
{
    if( rotation > 0)
    {
        rotAngle += 2.0;
        if( rotAngle >= 360.0) rotAngle = 0.0;
    }
    else
    if( rotation < 0)
    {
        rotAngle -= 2.0;
        if( rotAngle <= -360.0) rotAngle = 0.0;
    }
    //qDebug() << "Set rotation: " << rotAngle;
}

void Robot::setDrive( int drive )
{

    if( drive > 0)
    {
        driveUp = 1.0;
        mode = Manual;
    }
    else
    if( drive < 0)
    {
        driveUp = 0.0;
    }
    //qDebug() << "Set drive: " << driveUp;
}

QPointF Robot::newPosition()
{
    QPointF newPos = position;

    if( mode == DragDrop )
    {
        oldpos = position;
    }
    else
    if( mode == Manual )
    {
        double absSpeed;
        double angleRad = (rotAngle / 180.0) * PI;
        double diffAngleRad;
        double a_x_rot, a_y_rot;

        //angleRadOld = angleRad;

        // Measure speed
        v_x = (position.x()-oldpos.x())/0.1;
        v_y = (position.y()-oldpos.y())/0.1;

        absSpeed = fabs(sqrt((v_x*v_x) + (v_y*v_y)));

        //qDebug() << "Speed: " << absSpeed << " v_y: " << v_y;

        if( absSpeed > 0.0 )
        {
            if( v_y > 0.0 )
            {
                angleRadOld = PI-asin(v_x/absSpeed);
            }
            else
            {
                angleRadOld = asin(v_x/absSpeed);
            }
        }
        else
        {
            angleRadOld = angleRad;
        }

        diffAngleRad = -(angleRad - angleRadOld);

        //qDebug() << "Angle: " << angleRad << " Old angle: " << angleRadOld << " Diff angle: " << diffAngleRad;

        a_y_rot = 10*((-sin(diffAngleRad)*v_x)+(cos(diffAngleRad)*v_y)-v_y);
        a_x_rot = 10*((cos(diffAngleRad)*v_x)+(sin(diffAngleRad)*v_y)-v_x);


        if( driveUp >= 1.0 )
        {
            if( absSpeed > (maxSpeed+0.3) )
            {
                a_x = deceleration * v_x/absSpeed;
                a_y = deceleration * v_y/absSpeed;
            }
            else
            if( absSpeed < maxSpeed )
            {
                a_x = (acceleration * sin(angleRad));
                a_y = (acceleration * -cos(angleRad));
            }
        }
        else
        {
            if( absSpeed <= 2.0 )
            {
                a_x = 0.0;
                a_y = 0.0;
                v_x = 0.0;
                v_y = 0.0;
                mode = DragDrop;
            }
            else
            {
                a_x = deceleration * v_x/absSpeed;
                a_y = deceleration * v_y/absSpeed;
            }
        }

        if( collstr.contains("up") == true )
        {
            if( (fabs(angleRad) <= (PI/2)) || (fabs(angleRad) >= (3*PI/2)) )
            {
                a_y_rot = 0.0;
                a_y = 0.0;
                v_y = 0.0;
            }
        }
        else
        if(collstr.contains("down") == true)
        {
            if( (fabs(angleRad) >= (PI/2)) && (fabs(angleRad) <= (3*PI/2)) )
            {
                a_y_rot = 0.0;
                a_y = 0.0;
                v_y = 0.0;
            }
        }

        if( collstr.contains("right") == true )
        {
            if( (((-angleRad) >= (PI)) && ((-angleRad) <= (2*PI))) ||
                ((angleRad >= (0)) && (angleRad <= (PI))) )
            {
                a_x_rot = 0.0;
                a_x = 0.0;
                v_x = 0.0;
            }
        }
        else
        if(collstr.contains("left") == true)
        {
            if( (((-angleRad) >= (0)) && ((-angleRad) <= (PI))) ||
                ((angleRad >= (PI)) && (angleRad <= (2*PI))) )
            {
                a_x_rot = 0.0;
                a_x = 0.0;
                v_x = 0.0;
            }
        }

        //qDebug() << "Acc x: " << a_x << " y: " << a_y;
        s_x = position.x() + (0.1*v_x) + (0.5*0.1*0.1*(a_x+a_x_rot));
        s_y = position.y() + (0.1*v_y) + (0.5*0.1*0.1*(a_y+a_y_rot));
        newPos.setX(s_x);
        newPos.setY(s_y);

        oldpos = position;
    }
    else
    {

    }
    return newPos;
}



QPointF Robot::sensorPosition()
{
    static QPointF pos;

    if( sensorLost == false )
    {
        pos.setX(position.rx()+((double)((qrand()%(int)sensorPosRange)-(sensorPosRange/2.0))));
        pos.setY(position.ry()+((double)((qrand()%(int)sensorPosRange)-(sensorPosRange/2.0))));
    }

    return pos;
}

QVector<QPointF> Robot::distanceSensor()
{
    qreal sensorRadius = 60;
    double sensorAngle = (15.0/180.0)*PI;
    double angleRad = (rotAngle/180.0) * PI;
    QVector<QPointF> distancePoints;
    QPointF distancePoint;

    distancePoint.setX(position.rx()+(sensorRadius*sin(angleRad-(2.0*sensorAngle))));
    distancePoint.setY(position.ry()+(sensorRadius*(-cos(angleRad-(2.0*sensorAngle)))));
    distancePoints.push_back(distancePoint);

    distancePoint.setX(position.rx()+(sensorRadius*sin(angleRad-sensorAngle)));
    distancePoint.setY(position.ry()+(sensorRadius*(-cos(angleRad-sensorAngle))));
    distancePoints.push_back(distancePoint);

    distancePoint.setX(position.rx()+(sensorRadius*sin(angleRad+sensorAngle)));
    distancePoint.setY(position.ry()+(sensorRadius*(-cos(angleRad+sensorAngle))));
    distancePoints.push_back(distancePoint);

    distancePoint.setX(position.rx()+(sensorRadius*sin(angleRad+(2.0*sensorAngle))));
    distancePoint.setY(position.ry()+(sensorRadius*(-cos(angleRad+(2.0*sensorAngle)))));
    distancePoints.push_back(distancePoint);

    return distancePoints;
}


void Robot::reset()
{
    mode = DragDrop;
    destination.clear();
    rotAngle = 0.0;
    driveUp = 0.0;
    angleRadOld = 0.0;
}


void Robot::setDestination( QPointF dest )
{
   if( destination.size() > 0 )
   {
      destination.removeLast();
   }
   destination.push_back(dest);

}

void Robot::addDestination( QPointF dest )
{
   destination.insert(0, dest);
}

QPointF Robot::getDestination()
{
   QPointF dest = QPointF(0,0);
   if( destination.size() > 0 )
   {
      dest = destination.first();
   }

   return dest;
}

void Robot::setCollision( QString coll )
{
    if( coll != collstr )
    {
        qDebug() << coll;
    }

    //if( mode == Route )
    {
        collstr = coll;
    }
}

QPointF Robot::calcCollision()
{
   QPointF dest = QPointF(0,0);


   return dest;
}


