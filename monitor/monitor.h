#ifndef MONITOR_H
#define MONITOR_H

#include "airplug.h"
#include <QGraphicsScene>
#include <vector>


class QLineEdit;
class QComboBox;
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
    QLineEdit* _ipAdress;
    QComboBox* _port;
    QPushButton* _connectButton;
    QPushButton* _front;
    QPushButton* _back;
    QPushButton* _left;
    QPushButton* _right;
    QSpinBox* _distance;
    QLabel* _connectedRobotInfo;

public:
    Monitor();
    void receive(std::string const& msg, std::string  const& who) override;
};




#endif // MONITOR_H
