# Sarduino
D�codeur balise Sarsat 406 avec un Arduino.

Voici le code que j'ai cr�� avec mes maigres connaissance en programmation et traitement des octets.
Ca risque de faire peur � ceux qui maitrise le sujet, et moi m�me, je me dis que j'aurai pu faire beaucoup mieux (mais ce code � d�j� 3 ans, et entre temps j'ai appris beaucoup en programmation mais je n'ai jamais pris le temps de me repench� dessus).

# 1: PARTAGE

Le but de ce partage est de donner une base de d�part pour les dev comp�tant souhaitant redonner une jeunesse � ce projet.
Ne faisant plus parti de l'ADRASEC, je n'ai plus l'utilit� d'un d�codeur (en passant, je pense que l'utilit� de ce genre d'outil est minime du fait que l'info sera re�u bien avant nous par les stations de r�ception sat.).
N�anmoins, je souhaite suivre de pr�s les propositions, am�liorations que vous pourrez apporter et c'est pourquoi je vous demande de m'envoyer vos modifications TESTEES via Github.
(Pour ceux qui ne maitrise pas Github, merci de me contacter via Twitter @f4gmu)

# 2: SOFTWARE

Rien de particulier � dire ici.
Il suffit de prendre son courage � 2 mains et ouvrir le .ino !
Certaines parties sont comment�s : Dans la boucle principale loop() par exemple, la trame d�cod�e peut �tre affich�e sur la console s�rie (m'a �t� tr�s utile pour debug...)
J'avais �galement test� l'ajout d'un buzzer sur l'id�e de F5RZU qui sonne 45 secondes apr�s avoir d�cod� une trame (pour avoir une id�e de quand �couter la trame suivante, les �missions �tant espac�s de 50 secondes). Mais pour cette fonction j'ai utilis� un delay() qui "bloque" le d�codeur le temps d'attente et �a ne me plaisait pas trop. C'est pourquoi je l'ai comment� (dans ledblink() )

# 3: HARDWARE

 *Plus d'informations dans le dossier Hardware
Un Arduino UNO suffit pour ce code.
Le signal, provenant d'une sortie discri, est mis en forme pour �tre dig�rable par l'ATmega328 de l'Arduino.
Pour cette mise en forme, je me suis appuy� sur le travail de F1LVT : http://f1lvt.com/files/333-CarteDECTRA-V6P1.66.pdf
Ce signal mis en forme est branch� sur D2 de l'Arduino (interruption). 
Pour le reste, rien de plus qu'un �cran Oled et une LED.
La LED �tant sur D13 bien s�r ;)

 