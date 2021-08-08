# HARDWARE

Pour mon décodeur, j'avais créé un PCB "all-in-one" plutôt que d'utiliser un Arduino UNO.
Il permet de gagner un peu de place, mais nécessite une clé USB FTDI pour la programmation de l'ATmega328P.

Dans ce dossier :

-Alimentation.jpg
Un simple 78L05 (a mettre dans le bon sens...... )

-AT328P.jpg
L'ATmega avec quartz 16MHz, LED, connecteur pour l'écran Oled et connecteur pour programmation avec la clé FTDI (RTS sur la broche 1 dans cette image).

-MiseEnForme.jpg
J'ai rien inventé ici : se réferer au site de F1LVT dont le lien est dans le README.