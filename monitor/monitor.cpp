#include "monitor.h"
#include <string>


#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QSpinBox>
#include <QLabel>
#include <QCheckBox>

#include <QPainter>
#include <QGraphicsView>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QSpacerItem>
#include <QCommonStyle>
#include <QMessageBox>
#include <iostream>


#define GRID_STEP 10


using namespace std;
//ALGO POUR LES SAUVEGARDES = LESTAGE => Quand on fait une demande de sauvegarde il faut faire une action qui envoie un message pour que cela soit pris en compte
// cela entraine que l'horloge vectorielle du noeud i ayant déclenché la snapshot aura une valeur plus faible de 1 pour l'emplacement i => à corriger
//application par défaut pour la section critique
static const string MAP_defaultapp = "NET" ;

/*
   Mnémoniques globales
   - contient le nom de l'app qui a émit le msg : MAP_mnemoapp = "appmsg"
   - contient le type de msg : MAP_mnemotype = "typemsg"
   - contient l'horloge estampillée : MAP_mnemohlg = "hlgmsg"
   - contient le numéro du site distant : MAP_mnemosite = "sitemsg"
   - contient le payload (contenu du msg) : MAP_mnemopl = "plmsg"
   - contient l'horloge vectorielle : MAP_mnemoclck = "clkmsg"
   - contient l'état de snapshot = équivalent de la couleur dans l'algorithme lestage : MAP_mnemosnap = "snapmsg"
*/
static const string MAP_mnemoapp = "appmsg";
static const string MAP_mnemotype = "typemsg";

/*types de msgs*/

static const string MAP_obsmsg = "OBSMSG";
static const string MAP_connect = "MAPCO";


static const string ROB_connect = "ROBCO";
static const string ROB_ackord = "ROBACK";
static const string ROB_ord = "robord";
/* informations msgs*/

static const string ROB_join = "join";
static const string ROB_xpos = "xpos";
static const string ROB_ypos = "ypos";


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

    if(who == MAP_defaultapp) {//si message reçu depuis une appli NET
           //récupération du type de msg
           string type = APG_msg_splitstr(msg,MAP_mnemotype);
           if(type == MAP_obsmsg)
           {
               string xpos = APG_msg_splitstr(msg, ROB_xpos);
               string ypos = APG_msg_splitstr(msg, ROB_ypos);
               double x = atof(xpos.c_str());
               double y = atof(ypos.c_str());
               _obstacles.push_back(pair<double,double>(x,y));
               _map->addRect(QRectF(x*GRID_STEP,y*GRID_STEP,GRID_STEP,GRID_STEP),QPen((QColor(200, 200, 255))),QBrush((QColor(200, 200, 255))));
               cout << "I will draw a rect at (" << x << "," <<y <<") of 10px*10px"<<endl;
           }
           else if (type == ROB_connect || type == ROB_ackord)
           {
               string xpos = APG_msg_splitstr(msg, ROB_xpos);
               string ypos = APG_msg_splitstr(msg, ROB_ypos);
               _XconnectedRobot = atof(xpos.c_str());
               _YconnectedRobot = atof(ypos.c_str());

           }


    }
}


void Monitor::tryToConnect()
{

    _adressToSend.setAddress(_ipAdress->text());
    _portToSend = _port->value();
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


    string msg = APG_msg_createmsg(MAP_mnemotype,MAP_connect);
    send(msg,MAP_defaultapp);

    string info = "Information : You are connected with ";
    if(_local)
        info += "localhost";
    else
        info += _ipAdress->text().toStdString();


    info += " on port ";
    info += to_string(_portToSend);

    _connectedRobotInfo->setText(info.c_str());

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
    string msg = APG_msg_createmsg(MAP_mnemotype,ROB_ord);
    string move = to_string(_XconnectedRobot);
    move += ",";
    move += to_string(newY);
    APG_msg_addmsg(msg,ROB_join,move);
    send(msg,MAP_defaultapp);
}
void Monitor::goFront()
{
    double newY = _YconnectedRobot + _distance->value();
    string msg = APG_msg_createmsg(MAP_mnemotype,ROB_ord);
    string move = to_string(_XconnectedRobot);
    move += ",";
    move += to_string(newY);
    APG_msg_addmsg(msg,ROB_join,move);
    send(msg,MAP_defaultapp);
}

void Monitor::goLeft()
{
    double newX = _XconnectedRobot - _distance->value();
    string msg = APG_msg_createmsg(MAP_mnemotype,ROB_ord);
    string move = to_string(newX);
    move += ",";
    move += to_string(_YconnectedRobot);
    APG_msg_addmsg(msg,ROB_join,move);
    send(msg,MAP_defaultapp);
}

void Monitor::goRight()
{
    double newX = _XconnectedRobot + _distance->value();
    string msg = APG_msg_createmsg(MAP_mnemotype,ROB_ord);
    string move = to_string(newX);
    move += ",";
    move += to_string(_YconnectedRobot);
    APG_msg_addmsg(msg,ROB_join,move);
    send(msg,MAP_defaultapp);
}
