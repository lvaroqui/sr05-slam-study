#include "monitor.h"
#include <string>


#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QSpinBox>
#include <QLabel>

#include <QPainter>
#include <QGraphicsView>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QSpacerItem>
#include <QCommonStyle>
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

/* informations msgs*/

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
    _map = new CustomQGraphicsScene(this);
    _mapView = new QGraphicsView(_map,this);

    QLabel* ipLabel = new QLabel("IP address: ");
    _ipAdress = new QLineEdit();
    _ipAdress->setInputMask("000.000.000.000;_");

    QLabel* portLabel = new QLabel("Port: ");
    _port = new QComboBox();
    _port->addItem("4646");
    _port->addItem("4848");

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
    _distance->setMinimum(0);



    QGridLayout* monitorLayout = new QGridLayout();


    monitorLayout->addWidget(ipLabel,0,0,1,1);
    monitorLayout->addWidget(_ipAdress,0,1,1,2);
    monitorLayout->addWidget(portLabel,1,0,1,1);
    monitorLayout->addWidget(_port,1,1,1,2);
    monitorLayout->addWidget(_connectButton,2,0,1,3);
    monitorLayout->addWidget(_connectedRobotInfo,6,0,1,3);
    monitorLayout->addWidget(_front,3,1,1,1);
    monitorLayout->addWidget(_left,4,0,1,1);
    monitorLayout->addWidget(_distance,4,1,1,1);
    monitorLayout->addWidget(_right,4,2,1,1);
    monitorLayout->addWidget(_back,5,1,1,1);

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


    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(monitor);
    mainLayout->addWidget(_mapView);

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

    }
}
