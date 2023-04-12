
#ifndef ROBOT_H
#define ROBOT_H

#include <QDialog>
#include <QVector>

QT_BEGIN_NAMESPACE
class QLabel;
class QDoubleSpinBox;
class QGridLayout;
class QDialogButtonBox;
class QCheckBox;
QT_END_NAMESPACE

class Robot : public QDialog
{
    Q_OBJECT

public:
    enum Mode { Init, Manual, DragDrop, Route, Stopped, Finished };

public:
    Robot();

    double getAcceleration() { return acceleration; }
    double getMaxSpeed() { return maxSpeed; }
    void setDestination( QPointF dest );
    void addDestination( QPointF dest );
    QPointF getDestination();
    void setPosition( QPointF position ) { this->position = position; }
    void setFinished() { finish = true; }
    void setCollision( QString coll );
    QPointF newPosition();
    QPointF sensorPosition();
    bool getSensorPosAvailable() { return sensorPosAvailable; }
    void setSensorPosAvailable(bool avail) { sensorPosAvailable = avail; }
    Mode getMode() { return mode; }
    void reset();
    void setRotation( int rotation );
    void setDrive( int drive );
    double rotationAngle() { return rotAngle; }
    QCheckBox* getSensorCheckBox() { return senAvailBox;}
    QCheckBox* getLostCheckBox() { return senLostBox;}
    QVector<QPointF> distanceSensor();

private:
   QPointF calcCollision();

signals:
    void changedSensorRange(double range);

private slots:
    void acceptBox();
    void rejectBox();

public slots:
    void reject();

private:
    QGridLayout *gridLayout;
    QLabel *maxSpeedLabel;
    QDoubleSpinBox *speedMaxBox;
    QLabel *maxSpeedUnitLabel;
    QLabel *accLabel;
    QLabel *decLabel;
    QDoubleSpinBox *accBox;
    QDoubleSpinBox *decBox;
    QLabel *accUnitLabel;
    QLabel *decUnitLabel;
    QCheckBox *senAvailBox;
    QCheckBox *senLostBox;
    QDoubleSpinBox *senBox;
    QLabel *senUnitLabel;
    QDialogButtonBox *buttonBox;
    double maxSpeed;
    double oldMaxSpeed;
    double acceleration;
    double deceleration;
    QVector<QPointF> destination;
    QPointF position;
    QPointF oldpos;
    QString collstr;
    QString collstrold;
    Mode mode;
    double s_x;
    double s_y;
    double v_x;
    double v_y;
    double a_x;
    double a_y;
    double phi;
    bool finish;
    bool sensorPosAvailable;
    double sensorPosRange;
    double driveUp;
    double rotAngle;
    double angleRadOld;
    bool sensorLost;

};

#endif
