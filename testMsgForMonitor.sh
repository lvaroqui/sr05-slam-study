

# MAPCO = infos : IP + port de la map  => message à envoyer au rob
# $LCH$MAP$NET$^typemsg~MAPCO^ip~....^port~...^ on peut remplacer LCH par AIR si on monitor sur un autre pc et pas en LCH



#"$LCH$NET$MAP$^typemsg~ROBCO^xpos~0^ypos~0^heading~90^obs~100,200 100,201 100,202^" 

#msg vers ROB cf lib $AIR$MAP$ROB$...... join / turn / move / curr

#msg reçu du robot
#$AIR$ROB$MAP$^roback~curr:100,100,200^robcoll~1 #doute pour ça le rob m'envoie juste qu'il a rencontré un obstacle ou il m'envoie direct la liste des obstacles correspondants (les points représentants la longueur du robot) ?


#TEST pour MAP

echo "/LCH/NET/MAP/^typemsg~ROBCO^xpos~0^ypos~0^heading~40^obs~100,200 100,201 100,202^" > /dev/udp/127.0.0.1/65535

echo "/LCH/ROB/MAP/^roback~curr:100,100,45^robcoll~1" > /dev/udp/127.0.0.1/65535

