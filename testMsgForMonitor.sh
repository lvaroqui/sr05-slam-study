#msg attendu par le monitor
#msg : $LCH$NET$MAP$^typemsg~MAPCO^ on peut remplacer LCH par AIR si on monitor sur un autre pc et pas en LCH

#msg reçu pour indiquer qu'on est bien co


#Pour les msg suivants on remplace version normale avec des $ par des / pour que les echo marchent



#msg de demande de connexion

echo "/LCH/NET/MAP/^typemsg~ROBCO^xpos~0^ypos~0^obs~100,200 100,201 100,202^" > /dev/udp/127.0.0.1/65535

#msg de déplacement robot 



#msg obstacles

=



# MAPCO = infos : IP + port de la map  => message à envoyer au rob
# $LCH$MAP$NET$^typemsg~MAPCO^ip~....^port~...^ on peut remplacer LCH par AIR si on monitor sur un autre pc et pas en LCH



echo "$LCH$NET$MAP$^typemsg~ROBCO^xpos~0^ypos~^rotate~...^obs~100,200 100,201 100,202^" > /dev/udp/127.0.0.1/65535

#msg vers ROB cf lib $AIR$MAP$ROB$...... join / turn / move / curr

#msg reçu du robot
# $AIR$ROB$MAP$^roback~curr:x,y,heading~robcoll:1


