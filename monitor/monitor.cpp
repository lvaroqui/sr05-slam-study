#include "monitor.h"
#include <string>


#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QSpinBox>
#include <QLabel>
#include <QCheckBox>
#include <QGraphicsRectItem>

#include <QPainter>
#include <QGraphicsView>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QSpacerItem>
#include <QCommonStyle>
#include <QMessageBox>
#include <iostream>
#include "RobAck.h"

#include <QNetworkInterface>

#include <utils.h>

#define GRID_STEP 50


using namespace std;

//valeur d'un champ non rempli
static string APG_msg_unknown ="-";


static const string MAP_defaultapp = "NET" ;

const string MAP_mnemoapp = "appmsg";
static const string MAP_mnemotype = "typemsg";
static const string ROB_ord = "robord";
static const string ROB_ack = "roback";
/*types de msgs*/

static const string MAP_obsmsg = "OBSMSG";
static const string MAP_connect = "MAPCO";
static const string ROB_connect = "ROBCO";
static const string ROB_msg = "ROBMSG";
static const string END_connexion = "ENDCO";
/* informations msgs*/

static const string MAP_IP = "ip";
static const string MAP_PORT = "port";

static const string xpos = "xpos";
static const string ypos = "ypos";
static const string heading = "heading";
static const string ROB_sender = "snd";
static const string ROB_obs = "obs";



qreal round(qreal val, int step)
{
    int tmp = int(val) + step/2;
    tmp -= tmp % step;
    return qreal(tmp);
}

CustomQGraphicsScene::CustomQGraphicsScene(QObject* parent): QGraphicsScene (parent)
{

}

void CustomQGraphicsScene::drawBackground(QPainter *painter, const QRectF &rect)
{

    int step = GRID_STEP;
       painter->setPen(QPen(QColor(200, 200, 255, 125)));
       // draw horizontal grid
       qreal start = round(rect.top(), step);
       if (start > rect.top()) {
          start -= step;
       }
       for (qreal y = start - step; y < rect.bottom(); ) {
          y += step;
          painter->drawLine(rect.left(), y, rect.right(), y);
       }
       // now draw vertical grid
       start = round(rect.left(), step);
       if (start > rect.left()) {
          start -= step;
       }
       for (qreal x = start - step; x < rect.right(); ) {
          x += step;
          painter->drawLine(x, rect.top(), x, rect.bottom());
       }
}

Monitor::Monitor() : Application("MAP"), _robot(nullptr)
{
    _myPort = 65535;

    QList<QHostAddress> list = QNetworkInterface::allAddresses();

    for(int i = 0; i<list.count();i++)
    {
        if(!list[i].isLoopback())
            if(list[i].protocol() == QAbstractSocket::IPv4Protocol)
                _myIp = list[i].toString().toStdString();
    }
    //emplacement sur lequel le socket va lire => port 65535 pour tout sender
    _socket->disconnectFromHost();
    _socket->bind(QHostAddress::AnyIPv4,_myPort);

    _map = new CustomQGraphicsScene(this);
    _mapView = new QGraphicsView(_map,this);

    QLabel* _localHostLabel = new QLabel("LocalHost: ");
    _localhost = new QCheckBox();

    _ipLabel = new QLabel("IP address: ");
    //_ipLabel->setVisible(false);
    _ipAdress = new QLineEdit();
    _ipAdress->setInputMask("000.000.000.000;_");
   // _ipAdress->setVisible(false);

    QLabel* portLabel = new QLabel("Port: ");
    _port = new QSpinBox();
    _port->setRange(0,65535);

    _connectButton = new QPushButton("Connect to this robot");
    _connectedRobotInfo = new QLabel("Information : You are not connected to any robots.");



    _moveButton = new QPushButton("move");
    _moveDistance = new QSpinBox();
    _moveDistance->setSuffix(" cm");
    _moveDistance->setPrefix("move: ");
    _moveDistance->setMinimum(-5000);
    _moveDistance->setMaximum(5000);
    _moveDistance->setValue(0);
    _moveDistance->setSingleStep(50);

    _turnButton = new QPushButton("turn");
    _turnAngle = new QSpinBox();
    _turnAngle->setSuffix("°");
    _turnAngle->setPrefix("turn: ");
    _turnAngle->setMinimum(-360);
    _turnAngle->setValue(0);
    _turnAngle->setMaximum(360);

    _joinButton = new QPushButton("join");
    _xJoin = new QSpinBox();
    _xJoin->setMinimum(-5000);
    _xJoin->setValue(0);
    _xJoin->setMaximum(5000);
    _xJoin->setSuffix(" cm");
    _xJoin->setPrefix("x: ");
    _xJoin->setSingleStep(50);

    _yJoin = new QSpinBox();
    _yJoin->setMinimum(-5000);
    _yJoin->setValue(0);
    _yJoin->setMaximum(5000);
    _yJoin->setSingleStep(50);
    _yJoin->setSuffix(" cm");
    _yJoin->setPrefix("y: ");

    QGridLayout* monitorLayout = new QGridLayout();

    monitorLayout->addWidget(_localHostLabel,0,0,1,2);
    monitorLayout->addWidget(_localhost,0,2,1,1);
    monitorLayout->addWidget(_ipLabel,1,0,1,1);
    monitorLayout->addWidget(_ipAdress,1,1,1,2);
    monitorLayout->addWidget(portLabel,2,0,1,1);
    monitorLayout->addWidget(_port,2,1,1,2);
    monitorLayout->addWidget(_connectButton,3,0,1,3);
    monitorLayout->addWidget(_connectedRobotInfo,7,0,1,3);
    monitorLayout->addWidget(_moveDistance,4,0,1,2);
    monitorLayout->addWidget(_moveButton,4,2,1,1);
    monitorLayout->addWidget(_turnAngle,5,0,1,2);
    monitorLayout->addWidget(_turnButton,5,2,1,1);
    monitorLayout->addWidget(_xJoin,6,0,1,1);
    monitorLayout->addWidget(_yJoin,6,1,1,1);
    monitorLayout->addWidget(_joinButton,6,2,1,1);

    monitorLayout->setObjectName("Monitor");

    QSpacerItem* space = new QSpacerItem(40,50,QSizePolicy::Minimum,QSizePolicy::Maximum);
    monitorLayout->addItem(space,7,0,3);

    QSizePolicy spLeft(QSizePolicy::Preferred,QSizePolicy::Preferred);
    spLeft.setHorizontalStretch(1);
    QWidget* monitor = new QWidget();

    monitor->setLayout(monitorLayout);
    monitor->setSizePolicy(spLeft);

    QSizePolicy spRight(QSizePolicy::Preferred,QSizePolicy::Preferred);
    spRight.setHorizontalStretch(3);

    _mapView->setSizePolicy(spRight);
    _mapView->setObjectName("Map");

    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(monitor);
    mainLayout->addWidget(_mapView);

    connect(_connectButton,&QPushButton::clicked, this, &Monitor::tryToConnect);
    connect(_localhost,&QCheckBox::stateChanged,this, &Monitor::changeLocalHostState);
    connect(_moveButton,&QCheckBox::clicked,this,&Monitor::move);
    connect(_turnButton,&QCheckBox::clicked,this,&Monitor::turn);
    connect(_joinButton,&QCheckBox::clicked,this,&Monitor::join);

}

void Monitor::receive(std::string const& msg, std::string  const& who)
{

    cout << " DEBUT RECEIVE MONITORRRRR" << endl;
    if(who == "ROB") {//si message reçu depuis une appli NET
           //récupération du type de msg

           string robackMsg = APG_msg_splitstr(msg,ROB_ack);
           cout << "RECUPERE LE TYPE robackMsg : " <<robackMsg <<  endl;

           if(robackMsg != APG_msg_unknown)
           {

               RobAck ackVal(robackMsg);
               if(ackVal.getType() == RobAck::curr)
               {
                   cout << "MODIF POS ROBOT" << endl;
                   cout << "ackVal.getCommand().size =" << ackVal.getCommand().size() << endl;
                   _XconnectedRobot = ackVal.getCommand()[0];
                   _YconnectedRobot = ackVal.getCommand()[1];
                   _robotAngle = ackVal.getCommand()[2];

                   string mapRob = APG_msg_splitstr(msg,ROB_obs);
                   if(mapRob != APG_msg_unknown)
                   {
                       Map newPoints = fromStringToMap(mapRob);
                       for (auto point : newPoints)
                       {
                           double x = point.first.first;
                           double y = point.first.second;
                           if (point.second == pointType::wall) {
                               _obstacles.push_back(point.first);
                               _map->addRect(QRectF(x*GRID_STEP,y*GRID_STEP,GRID_STEP,GRID_STEP), QPen(QColor(0,0,0)),QBrush(QColor(0,0,0)))->setZValue(-1); //draw obstacle
                           }
                           else if (point.second == pointType::explored) {
                               _explored.push_back(point.first);
                                _map->addRect(QRectF(x*GRID_STEP,y*GRID_STEP,GRID_STEP,GRID_STEP), QPen(QColor(0,100,0)),QBrush(QColor(0,100,0)))->setZValue(-1); //draw explored
                           }
                       }
                   }

                   cout << "_robotAngle = " << _robotAngle << endl;
                  _robot->setRotation(_robotAngle);

                  _robot->setX(_XconnectedRobot*GRID_STEP);
                  _robot->setY(_YconnectedRobot*GRID_STEP);
               }
           }
    }
    else if(who == MAP_defaultapp) {     /*else if(type == MAP_obsmsg) //detection d'obstacles on print un obstacle
               // A MODIFIER APRES REU AVEC LUC
           {
               string xString = APG_msg_splitstr(msg,xpos);
               string yString = APG_msg_splitstr(msg,ypos);
               double x = atof(xString.c_str());
               double y = atof(yString.c_str());
               _obstacles.push_back(pair<double,double>(x,y));
               _map->addRect(QRectF(x*GRID_STEP,y*GRID_STEP,GRID_STEP,GRID_STEP), QPen(QColor(0,0,0)),QBrush(QColor(0,0,0))); //draw obstacle
               cout << "I will draw an obstacle at (" << x << "," << y << ") " << endl;
           }
           */
           string type = APG_msg_splitstr(msg,MAP_mnemotype);
           if(type == ROB_connect)
           {
                _map->clear();
                _obstacles.clear();
                _robot =nullptr;
                cout << "CONNEXION EN COURS" << endl;
                string xString = APG_msg_splitstr(msg,xpos);
                string yString = APG_msg_splitstr(msg,ypos);
                string rotation = APG_msg_splitstr(msg,heading);
                _XconnectedRobot = stoi(xString.c_str());
                _YconnectedRobot = stoi(yString.c_str());
                _robotAngle = stoi(rotation.c_str());

                cout << "xpos = " << xString << " ypos = " << yString << " robot angle = " << rotation <<  endl;

                if(_robot == nullptr){
                    cout << "create robot" << endl;
                    _robot = _map->addRect(QRect(0.1*GRID_STEP,0.1*GRID_STEP,GRID_STEP*0.8,GRID_STEP*0.8),QPen(QColor(255,0,0)),QBrush(QColor(255,0,0)));
                    _robot->setTransformOriginPoint(GRID_STEP*0.9/2, GRID_STEP*0.9/2);
                    //_robot->setOpacity(0.8);
                    _robot->setRotation(_robotAngle);
                   _robot->setX(_XconnectedRobot*GRID_STEP);
                   _robot->setY(_YconnectedRobot*GRID_STEP);
                }
                else
                {
                    cout << "modify robot" <<endl;
                    _robot->setRotation(_robotAngle);
                   _robot->setX(_XconnectedRobot*GRID_STEP);
                   _robot->setY(_YconnectedRobot*GRID_STEP);
                }


                string mapRob = APG_msg_splitstr(msg,ROB_obs);
                if(mapRob != APG_msg_unknown)
                {
                    Map newPoints = fromStringToMap(mapRob);
                    for (auto point : newPoints)
                    {
                        double x = point.first.first;
                        double y = point.first.second;
                        if (point.second == pointType::wall) {
                            _obstacles.push_back(point.first);
                            _map->addRect(QRectF(x*GRID_STEP,y*GRID_STEP,GRID_STEP,GRID_STEP), QPen(QColor(0,0,0)),QBrush(QColor(0,0,0)))->setZValue(-1); //draw obstacle
                        }
                        else if (point.second == pointType::explored) {
                            _explored.push_back(point.first);
                             _map->addRect(QRectF(x*GRID_STEP,y*GRID_STEP,GRID_STEP,GRID_STEP), QPen(QColor(0,100,0)),QBrush(QColor(0,100,0)))->setZValue(-1); //draw explored
                        }
                    }
                }


           }


    }
}

void Monitor::tryToConnect()
{
    //préviens déconnexion avec l'autre robot
    string msg = APG_msg_createmsg(MAP_mnemotype, END_connexion);
    APG_msg_addmsg(msg,MAP_IP,_myIp);
    APG_msg_addmsg(msg,MAP_PORT,to_string(_myPort));
    send(msg,"NET");


    if(!_local)
    {
        if(_adressToSend == QHostAddress::Null)
        {
            QMessageBox::critical(this,"Error","Wrong IP Address");
            _connectedRobotInfo->setText("Information : You are not connected to any robots.");
            return;
        }



    }
    cout << "TRY TO CONNECT BEGIN" << endl;



    //mise à jour des informations de connection
    string info = "Information : You are connected with ";
    if(_local)
        info += "localhost";
    else
        info += _ipAdress->text().toStdString();

    info += " on port ";
    info += to_string(_portToSend);

    _connectedRobotInfo->setText(info.c_str());

    //envoyer msg au robot souhaité pour lui dire qu'on souhaite se connecter : si en localhost alors la modification de l'adresse n'a aucun impact
    _adressToSend.setAddress(_ipAdress->text());
    _portToSend = _port->value();
    string msgCo = APG_msg_createmsg(MAP_mnemotype,MAP_connect);
    APG_msg_addmsg(msgCo,MAP_IP,_myIp);
    APG_msg_addmsg(msgCo,MAP_PORT,to_string(_myPort));
    send(msgCo,"NET");

}

void Monitor::changeLocalHostState(int state)
{
    if(state == Qt::Checked)
    {
        _local = true;
        _ipAdress->setVisible(false);
        _ipLabel->setVisible(false);
    }
    else {
        _local = false;
        _ipAdress->setVisible(true);
        _ipLabel->setVisible(true);
    }
}

void Monitor::move()
{
    string msg = APG_msg_createmsg(MAP_mnemotype,ROB_msg);
    string move = "move:";
    move += to_string(_moveDistance->value());
    APG_msg_addmsg(msg,ROB_ord,move);
    send(msg,"ROB");

}

void Monitor::turn()
{
    string msg = APG_msg_createmsg(MAP_mnemotype,ROB_msg);
    string turn = "turn:";
    turn += to_string(_turnAngle->value());
    APG_msg_addmsg(msg,ROB_ord,turn);
    send(msg,"ROB");
}

void Monitor::join()
{
    string msg = APG_msg_createmsg(MAP_mnemotype,ROB_msg);
    string join = "join:";
    join += to_string(_xJoin->value()) + "," + to_string(_yJoin->value());
    APG_msg_addmsg(msg,ROB_ord,join);
    send(msg,"ROB");
}
