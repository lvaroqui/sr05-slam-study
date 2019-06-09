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
    std::vector<std::pair<int,int>> _obstacles;
    std::vector<std::pair<int,int>> _explored;
    std::string _myIp;
    int _myPort;
    QCheckBox* _localhost;
    QLineEdit* _ipAdress;
    QSpinBox* _port;
    QPushButton* _connectButton;


    QSpinBox* _moveDistance;
    QPushButton* _moveButton;

    QSpinBox* _turnAngle;
    QPushButton* _turnButton;

    QSpinBox* _xJoin;
    QSpinBox* _yJoin;
    QPushButton* _joinButton;

    QLabel* _connectedRobotInfo;
    QLabel* _ipLabel;
    int _XconnectedRobot;
    int _YconnectedRobot;
    int _robotAngle;

    QGraphicsRectItem* _robot;

public:
    Monitor();
    void receive(std::string const& msg, std::string  const& who) override;

public slots:
    void tryToConnect();
    void join();
    void move();
    void turn();
    void changeLocalHostState(int state);
};




#endif // MONITOR_H
