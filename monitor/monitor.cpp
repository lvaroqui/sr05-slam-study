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

#include <utils.h>

#define GRID_STEP 5
#define ROB_LENGTH 25.4 * 5
#define ROB_WIDTH 15.24 * 5

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

/* informations msgs*/

static const string xpos = "xpos";
static const string ypos = "ypos";
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

Monitor::Monitor() : Application("MAP")
{
    //emplacement sur lequel le socket va lire => port 65535 pour tout sender
    _socket->disconnectFromHost();
    _socket->bind(QHostAddress::LocalHost,65535);

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


    _front = new QPushButton();
    QCommonStyle style;
    _front->setIcon(style.standardIcon(QStyle::SP_ArrowUp));

    _back = new QPushButton();
    _back->setIcon(style.standardIcon(QStyle::SP_ArrowDown));

    _left = new QPushButton();
    _left->setIcon(style.standardIcon(QStyle::SP_ArrowLeft));

    _right = new QPushButton();
    _right->setIcon(style.standardIcon(QStyle::SP_ArrowRight));

    _distance = new QSpinBox();
    _distance->setSuffix(" cm");
    _distance->setMinimum(1);



    QGridLayout* monitorLayout = new QGridLayout();

    monitorLayout->addWidget(_localHostLabel,0,0,1,2);
    monitorLayout->addWidget(_localhost,0,2,1,1);
    monitorLayout->addWidget(_ipLabel,1,0,1,1);
    monitorLayout->addWidget(_ipAdress,1,1,1,2);
    monitorLayout->addWidget(portLabel,2,0,1,1);
    monitorLayout->addWidget(_port,2,1,1,2);
    monitorLayout->addWidget(_connectButton,3,0,1,3);
    monitorLayout->addWidget(_connectedRobotInfo,7,0,1,3);
    monitorLayout->addWidget(_front,4,1,1,1);
    monitorLayout->addWidget(_left,5,0,1,1);
    monitorLayout->addWidget(_distance,5,1,1,1);
    monitorLayout->addWidget(_right,5,2,1,1);
    monitorLayout->addWidget(_back,6,1,1,1);

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
    connect(_back,&QCheckBox::clicked,this,&Monitor::goBack);
    connect(_front,&QCheckBox::clicked,this,&Monitor::goFront);
    connect(_right,&QCheckBox::clicked,this,&Monitor::goRight);
    connect(_left,&QCheckBox::clicked,this,&Monitor::goLeft);
    //resize(500,500);
}

void Monitor::receive(std::string const& msg, std::string  const& who)
{

    cout << " DEBUT RECEIVE MONITORRRRR" << endl;
    if(who == MAP_defaultapp) {//si message reçu depuis une appli NET
           //récupération du type de msg
           string type = APG_msg_splitstr(msg,MAP_mnemotype);
           string robackMsg = APG_msg_splitstr(msg,ROB_ack);
           cout << "RECUPERE LE TYPE robackMsg : " <<robackMsg <<  endl;

           if(robackMsg != APG_msg_unknown)
           {

               RobAck ackVal(robackMsg);
               if(ackVal.getType() == RobAck::joined || ackVal.getType() == RobAck::curr)
               {
                   cout << "MODIF POS ROBOT" << endl;
                   _XconnectedRobot = ackVal.getCommand()[0];
                   _YconnectedRobot = ackVal.getCommand()[1];


                   /* à faire print le rectangle correspondant au robot */
                   string sender = APG_msg_splitstr(msg,ROB_sender);
                   _connectedRobotId = sender;


                   _robot->setX(_XconnectedRobot*GRID_STEP);
                   _robot->setY(_YconnectedRobot*GRID_STEP);
               }
           }
           else if(type == MAP_obsmsg) //detection d'obstacles on print un obstacle
               /* A MODIFIER APRES REU AVEC LUC */
           {
               string xString = APG_msg_splitstr(msg,xpos);
               string yString = APG_msg_splitstr(msg,ypos);
               double x = atof(xString.c_str());
               double y = atof(yString.c_str());
               _obstacles.push_back(pair<double,double>(x,y));
               _map->addRect(QRectF(x*GRID_STEP,y*GRID_STEP,GRID_STEP,GRID_STEP), QPen(QColor(0,0,0)),QBrush(QColor(0,0,0))); //draw obstacle
               cout << "I will draw an obstacle at (" << x << "," << y << ") " << endl;
           }
           else if(type == ROB_connect)
               /* A MODIFIER APRES REU AVEC LUC*/
           {
               _map->clear();
               cout << "CONNEXION EN COURS" << endl;
               string xString = APG_msg_splitstr(msg,xpos);
               string yString = APG_msg_splitstr(msg,ypos);
               cout << "xpos = " << xString << " ypos = " << yString << endl;
               _XconnectedRobot = stof(xString.c_str());
               _YconnectedRobot = stof(yString.c_str());
               _robot = _map->addRect(QRect(_XconnectedRobot*GRID_STEP + ROB_LENGTH/2,_YconnectedRobot*GRID_STEP + ROB_WIDTH/2,ROB_LENGTH,ROB_WIDTH),QPen(QColor(255,0,0)),QBrush(QColor(255,0,0)));


               string sender = APG_msg_splitstr(msg,ROB_sender);
               _connectedRobotId = sender;

               string obstacles = APG_msg_splitstr(msg,ROB_obs);
               _obstacles = fromStringToVectorOfPairs(obstacles);
               for (auto obstacle : _obstacles)
               {
                    double x = obstacle.first;
                    double y = obstacle.second;
                    _map->addRect(QRectF(x*GRID_STEP,y*GRID_STEP,GRID_STEP,GRID_STEP), QPen(QColor(0,0,0)),QBrush(QColor(0,0,0))); //draw obstacle
               }


           }


    }
}


void Monitor::tryToConnect()
{


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
    string msg = APG_msg_createmsg(MAP_mnemotype,MAP_connect);
    send(msg,MAP_defaultapp);

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

void Monitor::goBack()
{
    double newY = _YconnectedRobot - _distance->value();
    string msg = APG_msg_createmsg(MAP_mnemotype,ROB_msg);
    string move = "join:";
    move += to_string(_XconnectedRobot);
    move += ",";
    move += to_string(newY);
    APG_msg_addmsg(msg,ROB_ord,move);
    send(msg,MAP_defaultapp);
}
void Monitor::goFront()
{
    double newY = _YconnectedRobot + _distance->value();
    string msg = APG_msg_createmsg(MAP_mnemotype,ROB_msg);
    string move = "join:";
    move += to_string(_XconnectedRobot);
    move += ",";
    move += to_string(newY);
    APG_msg_addmsg(msg,ROB_ord,move);
    send(msg,MAP_defaultapp);
}

void Monitor::goLeft()
{
    double newX = _XconnectedRobot - _distance->value();

    string msg = APG_msg_createmsg(MAP_mnemotype,ROB_msg);
    string move = "join:";
    move += to_string(newX);
    move += ",";
    move += to_string(_YconnectedRobot);
    APG_msg_addmsg(msg,ROB_ord,move);
    send(msg,MAP_defaultapp);
}

void Monitor::goRight()
{
    double newX = _XconnectedRobot + _distance->value();
    string msg = APG_msg_createmsg(MAP_mnemotype,ROB_msg);
    string move = "join:";
    move += to_string(newX);
    move += ",";
    move += to_string(_YconnectedRobot);
    APG_msg_addmsg(msg,ROB_ord,move);
    send(msg,MAP_defaultapp);
}
