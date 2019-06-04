#ifndef MONITOR_H
#define MONITOR_H

#include "airplug.h"
#include <QGraphicsScene>
#include <vector>
#include <map>

class QLineEdit;
class QCheckBox;
class QPushButton;
class QSpinBox;
class QLabel;


class CustomQGraphicsScene : public QGraphicsScene
{
public:
    CustomQGraphicsScene(QObject* parent);

protected:
    void drawBackground(QPainter* painter, const QRectF& rect) override;

};

class Monitor : public Application
{
private:
    CustomQGraphicsScene* _map;
    QGraphicsView* _mapView;
    std::vector<std::pair<double,double>> _obstacles;
    QCheckBox* _localhost;
    QLineEdit* _ipAdress;
    QSpinBox* _port;
    QPushButton* _connectButton;
    QPushButton* _front;
    QPushButton* _back;
    QPushButton* _left;
    QPushButton* _right;
    QSpinBox* _distance;
    QLabel* _connectedRobotInfo;
    QLabel* _ipLabel;
    double _XconnectedRobot;
    double _YconnectedRobot;
    std::string _connectedRobotId;
    QGraphicsRectItem* _robot;

public:
    Monitor();
    void receive(std::string const& msg, std::string  const& who) override;

public slots:
    void tryToConnect();
    void goBack();
    void goFront();
    void goRight();
    void goLeft();
    void changeLocalHostState(int state);
};




#endif // MONITOR_H
