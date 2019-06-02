#ifndef AIRPLUG_H
#define AIRPLUG_H
#include <string>
#include <vector>
#include <QWidget>
#include <QUdpSocket>

// réimplémentation en C++ des fonctions de la librairie AIRPLUG permettant de construire des messages et en envoyer/recevoir
// ces fonctions respectent parfaitement la syntaxe de l'intergiciel AIRPLUG.


//### FONCTIONS DE LA BIBLIOTHEQUE #############################################

//-- Procedure APG_msg_split --------------------------------------------------#
// Action : decoupe la chaine en prenant le premier caractere comme delimiteur #
// Entree : chaine                                                             #
// Retour : tableau constitue par les champs trouves dans la chaine            #
//-----------------------------------------------------------------------------#
std::vector<std::string> APG_msg_split (std::string chaine);


//-- Procedure APG_msg_splitstr (split string) --------------------------------#
// Action : retrouve la valeur associee a un mnemonique dans une chaine (msg)  #
//          Permet d'exploiter le format des contenus de messages a l'arrivee. #
// Entree : message, mnemonique                                       #
// Retour : valeur associee au mnemonique                                      #
//-----------------------------------------------------------------------------#
std::string APG_msg_splitstr (std::string msg, std::string const& mnemonique);



//-- fonction APG_msg_createmsg ----------------------------------------------#
//Action : formate un contenu de message de la forme /mnemonique~valeur       #
// Entree : mnemonique, valeur                                                 #
// Retour : chaine contenant le message formate avec les jockers standard      #
//-----------------------------------------------------------------------------#
std::string APG_msg_createmsg(std::string const& mnemonique, std::string const& value);


//-- Procedure APG_msg_addmsg -------------------------------------------------#
// Action : ajoute un couple /mnemonique~valeur a la fin d'un message existant #
// Entree : message,mnemonique et valeur a ajouter                             #
//-----------------------------------------------------------------------------#
void APG_msg_addmsg (std::string& msg, std::string const& mnemonique, std::string const& value);


//-- Procedure APG_msg_concatemsg ---------------------------------------------#
// Action : concatene deux messges en evitant deux delimiteurs accoles.        #
// Entree : message 1, message 2                                               #
// Retour : message forme de la concatenation du message 1 et du message 2     #
//-----------------------------------------------------------------------------#
std::string APG_msg_concatemsg (std::string const& firstMsg, std::string const& secondMsg);


//-- Procedure APG_msg_replaceval ---------------------------------------------#
//Action : remplace la valeur associee a un mnemonique dans un message         #
// Entree :  message, mnemonique, valeur                                       #
//-----------------------------------------------------------------------------#




//-- Class Application --------------------------------------------------------#
// Classe dont hérite toutes les applications qui vont recevoir ou envoyer des #
// messages (le fonctionnement de la réception de messages est décrit dans le  #
// readme du programme asynchrone)                                             #
//-----------------------------------------------------------------------------#
class Application : public QWidget {
protected:
    std::string _appName;
    QUdpSocket* _socket;
public:
    Application(std::string const& appName);
    ~Application() = default;

    /**
     * @brief receive: fonction surchargée permettant de gérer la réception d'un message provenant de l'application who
     */
    virtual void receive(std::string const& msg, std::string  const& who) = 0;

    /**
     * @brief sendCom : permetet d'envoyer la concatenation de from, header et payload sur stdout
     */
    void sendCom(std::string const& from, std::string const& header, std::string const& payload);
    /**
     * @brief send: construit le from, header, payload du message qui va être envoyé sur stdout : from est le nom d'application le header contient l'application visée par le message
     * ,c'est-à-dire who et payload qui contient le contenu du message c'est à dire what
     */
    void send(std::string const& what, std::string const& who);

public slots:
    /**
     * @brief receiveCom: (cf Readme du programme asynchrone : permet de récupérer les messages sur stdin et d'appeler la méthode receive
     */
    void receiveCom();
};

#endif // AIRPLUG_H
