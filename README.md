# Sarduino
Décodeur balise Sarsat 406 avec un Arduino par F4GMU
https://www.youtube.com/watch?v=O7L0IGT8fTQ

Voici le code que j'ai créé avec mes maigres connaissance en programmation et traitement des octets.
Ca risque de faire peur à ceux qui maitrise le sujet, et moi même, je me dis que j'aurai pu faire beaucoup mieux (mais ce code à déjà 3 ans, et entre temps j'ai appris beaucoup en programmation mais je n'ai jamais pris le temps de me repenché dessus).

# 1: PARTAGE

Le but de ce partage est de donner une base de départ pour les dev compétant souhaitant redonner une jeunesse à ce projet.
Ne faisant plus parti de l'ADRASEC, je n'ai plus l'utilité d'un décodeur (en passant, je pense que l'utilité de ce genre d'outil est minime du fait que l'info sera reçu bien avant nous par les stations de réception sat.).
Néanmoins, je souhaite suivre de près les propositions, améliorations que vous pourrez apporter et c'est pourquoi je vous demande de m'envoyer vos modifications TESTEES via Github.
(Pour ceux qui ne maitrise pas Github, merci de me contacter via Twitter @f4gmu)

# 2: SOFTWARE

Rien de particulier à dire ici.
Il suffit de prendre son courage à 2 mains et ouvrir le .ino !
Certaines parties sont commentés : Dans la boucle principale loop() par exemple, la trame décodée peut être affichée sur la console série (m'a été très utile pour debug...)
J'avais également testé l'ajout d'un buzzer sur l'idée de F5RZU qui sonne 45 secondes après avoir décodé une trame (pour avoir une idée de quand écouter la trame suivante, les émissions étant espacés de 50 secondes). Mais pour cette fonction j'ai utilisé un delay() qui "bloque" le décodeur le temps d'attente et ça ne me plaisait pas trop. C'est pourquoi je l'ai commenté (dans ledblink() )

# 3: HARDWARE

 *Plus d'informations dans le dossier Hardware
Un Arduino UNO suffit pour ce code.
Le signal, provenant d'une sortie discri, est mis en forme pour être digérable par l'ATmega328 de l'Arduino.
Pour cette mise en forme, je me suis appuyé sur le travail de F1LVT : http://f1lvt.com/files/333-CarteDECTRA-V6P1.66.pdf
Ce signal mis en forme est branché sur D2 de l'Arduino (interruption). 
Pour le reste, rien de plus qu'un écran Oled et une LED.
La LED étant sur D13 bien sûr ;)

 
Jérôme.