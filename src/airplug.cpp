#include "airplug.h"
#include <iostream>
#include <stdio.h>
#include <QEvent>
#include <QString>
#include <unistd.h>
#include <poll.h>
#include <fcntl.h>
using namespace std;


//VARIABLES

//TAILLE MAX D'UN MESSAGE INTER-SITES en octets (nb char)
// /!\ compte le caractère '\0'
static int APG_msg_nbc_all = 1440;
static int APG_msg_nbc_app = 4;
static int APG_msg_nbc_hst = 10;

//taille max du champ controle dans les msg inter-sites
static int APG_msg_nbc_ctl = 256;

//taille max du champ argument. Calcul dans le cas d'un msg inter-sites car plus court que dans le cas d'un msg emis par une app locale
static int APG_msg_nbc_arg = APG_msg_nbc_all - 2* APG_msg_nbc_app - 2*APG_msg_nbc_hst - APG_msg_nbc_ctl;

//symbole égalité qui n'est pas = pour ne pas géner les conditions de HOP
static char APG_msg_eq = '~';

//délimiteur de champs
static char APG_msg_delim ='^';

//valeur d'un champ non rempli
static string APG_msg_unknown ="-";

//delimiteur pour les arguments composés comme a,b,c
static char APG_msg_delimarg = ',';

//sert de séparateur entre le sender le receiver et le msg
static char apg_com_delim = '$';


vector<string> APG_msg_split(string chaine) {
    vector<string> results;

    if(chaine.length() <= 0 || chaine.length() > APG_msg_nbc_all)
        throw invalid_argument("FATAL : /airplug.cpp/APG_msg_split -- invalid message length");

    char delim = chaine[0];
    int pos = static_cast<int>(chaine.find(delim));

    while(pos != string::npos) {
        string substr = chaine.substr(0,pos);
        results.push_back(substr);

        chaine.erase(0, static_cast<int>(pos+1));
        pos = static_cast<int>(chaine.find(delim));
    }
    results.push_back(chaine);
    return results;
}


string APG_msg_splitstr (string msg, string const& mnemonique) {
    vector<string> champs = APG_msg_split(msg);

    // Parcours du tableau des champs mnemonique~valeur dans le message
    for(auto itr = champs.begin();itr != champs.end(); itr++) {
        string name = itr->substr(0,itr->find(APG_msg_eq));

        if (name.length() > APG_msg_nbc_arg)
            throw out_of_range("FATAL : /airplug.cpp/APG_msg_splitstr -- Argument length out of bounds");

        if (name == mnemonique)
            return itr->substr((itr->find(APG_msg_eq)+1));
    }

    //mnemonique non trouvé
    return APG_msg_unknown;
}

string APG_msg_createmsg(string const& mnemonique, string const& value) {
    return APG_msg_delim + mnemonique + APG_msg_eq + value + APG_msg_delim;
}

void APG_msg_addmsg (string& msg, string const& mnemonique, string const& value) {
    if((mnemonique + APG_msg_eq + value).length() > APG_msg_nbc_arg)
        throw invalid_argument("FATAL : /airplug.cpp/APG_msg_addmsg -- invalid arg length");


    //ajout d'un délimiteur à la fin du msg actuel si necessaire
    if (msg[msg.length() - 1] != APG_msg_delim) msg += APG_msg_delim;
    msg += mnemonique + APG_msg_eq + value;
    //ajout d'un délimiteur à la fin du msg si necessaire : cela peut arriver si value est un msg
    if (msg[msg.length() - 1] != APG_msg_delim) msg += APG_msg_delim;
}


string APG_msg_concatemsg (string const& firstMsg, string const& secondMsg) {
    string msgResult;


    if(firstMsg.length() <= 0 || secondMsg.length() <= 0 || firstMsg.length()+secondMsg.length() > APG_msg_nbc_all)
        throw invalid_argument("FATAL : /airplug.cpp/APG_msg_concatemsg -- invalid message length");

    //suppression d'un délimiteur de trop si nécessaire
    if(firstMsg[firstMsg.length()-1] == APG_msg_delim && secondMsg[0] == APG_msg_delim)
        msgResult = firstMsg.substr(( 0 ,firstMsg.length()-1))+secondMsg;
    else msgResult = firstMsg + secondMsg;
    return msgResult;
}


Application::Application(string const& appName):_appName(appName) {
    _stdinSocket = new QSocketNotifier(fileno(stdin),QSocketNotifier::Read,this);
    //_stdinSocket->installEventFilter(this);
    connect(_stdinSocket, &QSocketNotifier::activated,this,&Application::receiveCom);
}


void Application::receiveCom() {
    _stdinSocket->setEnabled(false); //disconnect la socket pour traiter le message et mettre les autres en attente
    string msg;


    int ch;
    do
    {
        //traitement du message
        getline(cin,msg);
        vector<string> args = APG_msg_split(msg);


        if(args[2] == _appName || args[2] == "broadcast") //si on est bien concerné par le msg
        {
            receive(args[3], args[1]);
        }


        /* Ce slot s'exécute quand des nouvelles données ont été ajoutées dans stdin. Cependant pendant le traitement du message il
         * est possible de recevoir d'autres messages qui eux n'auront pas déclencher le signal nous indiquant que de nouvelles données
         * ont été reçues puisque receiveCom ne peut être bloqué (tout le programme se déroule dans un seul thread et l'action est donc atomique) il faut donc vérifier
         * le contenu de stdin au cas où il reste des messages à traiter
         */

        int flag;
        flag = fcntl(STDIN_FILENO, F_GETFL,0); // on récupère les drapeaux de stdin
        fcntl(STDIN_FILENO,F_SETFL,flag|O_NONBLOCK); //on ajoute aux drapeaux le drapeau O_NONBLOCK pour faire des lectures non bloquantes
        ch = ungetc(getc(stdin),stdin); //on récupère un caractère de stdin (qu'on réinsère directement à sa place une fois récupéré
        fcntl(STDIN_FILENO,F_SETFL,flag); // on remet les flags par défaut

    }while(ch != EOF); // si il n'y avait rien dans STDIN alors ch = EOF donc on n'a plus de messages à traiter pour l'instant sinon on traite le message qui est actuellement dans stdin
    _stdinSocket->setEnabled(true);
}

void Application::sendCom(std::string const& from, std::string const& header, std::string const& msg) {
    string msgToSend = from + header + msg ;
    cout << msgToSend << endl;
}

void Application::send(std::string const& what, std::string const& who) {
    if(what[0]!= APG_msg_delim) return; //erreur formatage msg

    string from = apg_com_delim + _appName;
    string header = apg_com_delim + who;
    string payload = apg_com_delim + what;
    sendCom(from,header,payload);
}

bool Application::eventFilter(QObject *object, QEvent *event)
{
    if (object == _stdinSocket && event->type() == QEvent::SockAct)
    {
        cerr << "UN MESSAGE EST RECU" << endl;
    }
    if (object == _stdinSocket && event->type() == QEvent::SockClose)
    {
        cerr << " CANAL VIDE" << endl;
    }
    return true;
}
