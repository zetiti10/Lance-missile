# Lance missile

Ce programme permet la gestion complète d'un lance-missile. Il est exécuté par un Arduino Nano Every qui remplace le micro-processeur d'origine. Le but est de pouvoir contrôler aisément le lance-missile depuis un ordinateur (USB) ou tout autre appareils comme un Arduino connecté en UART à celui-ci. La conversion consiste dans un premier temps à remplacer l'ancienne électronique par les nouveaux composants, puis à programmer l'Arduino pour permettre un contrôle précis du lance-missile.

> Le lance-missile en question est un ancien appareil (aujourd'hui retiré de la vente) produit par Satzuma. Quelques images de celui-ci sont disponibles sur [ce site](https://www.mightyape.co.nz/product/satzuma-usb-missile-launcher/21393705/images).

Ce programme a été écrit en C++ pour l'Arduino Nano Every à l'aide de PlatformIO.

> Les informations renseignées ici ne suffisent pas à reproduire le projet. Si vous êtes intéressés, vous pouvez me contacter sur Discord : @zetiti10.

Le programme est terminé.

Une bibliothèque Arduino est disponible sur [ce dépôt](https://github.com/zetiti10/Bibliotheque-Arduino-lance-missile), elle permet de contrôler le lance-missile aisément avec des méthodes pratiques.

## Modifications du lance-missile original

Les moteurs et les capteurs du lance-missile ont été conservés, cependant, le reste de l'électrnoique a été retiré. Des connecteurs Dupont ont été mis pour rendre la maintenance plus facile.

Le boîtier des pilles a été retiré car in ne sert plus et deux trous ont été percés : l'un pour un connecteur USB-C et l'autre pour un connecteur Dupont 4 broches femelle.

Un circuit imprimé générique (quadrillage de trous) héberge l'Arduino Nano Every, tous les connecteurs pour les câbles et les deux L293D (ponts en H pour les moteurs).

## Connexions à l'Arduino Nano Every

| Broche  | Périphérique connecté                 | Nom dans le programme       |
| ------- | ------------------------------------- | --------------------------- |
| A1 / 15 | Capteur de l'orientation de la base 1 | `PIN_BASE_SENSOR_1`         |
| A0 / 14 | Capteur de l'orientation de la base 2 | `PIN_BASE_SENSOR_2`         |
| A2 / 16 | Capteur du missile 1                  | `PIN_MISSILE_1_SENSOR`      |
| A3 / 17 | Capteur du missile 2                  | `PIN_MISSILE_2_SENSOR`      |
| A4 / 18 | Capteur du missile 3                  | `PIN_MISSILE_3_SENSOR`      |
| A5 / 19 | Capteur de l'état du lanceur          | `PIN_LAUNCHER_MOTOR_SENSOR` |
| 8       | Capteur de l'inclinaison de la tête 1 | `PIN_ANGLE_SENSOR_1`        |
| 9       | Capteur de l'inclinaison de la tête 2 | `PIN_ANGLE_SENSOR_2`        |
| 3       | Moteur de la base 1                   | `PIN_BASE_MOTOR_1`          |
| 2       | Moteur de la base 2                   | `PIN_BASE_MOTOR_2`          |
| 4       | Moteur de l'inclinaison 1             | `PIN_ANGLE_MOTOR_1`         |
| 5       | Moteur de l'inclinaison 2             | `PIN_ANGLE_MOTOR_2`         |
| 6       | Moteur du lanceur 1                   | `PIN_LAUNCHER_MOTOR_1`      |
| 7       | Moteur du lanceur 2                   | `PIN_LAUNCHER_MOTOR_2`      |

Le port UART (connecteur Dupont 4 broches femelle) est relié au RX/TX de l'Arduino Nano Every.

## Fonctionnement du programme

Le programme commence par initialiser le lance-missile : si aucune valeur stockée dans la mémoire persistante (EEPROM) n'est trouvée, une calibration est lancée. Le programme calcule les vitesses angulaires des deux axes puis retourne au point initial. Les valeurs sont enregistrées dans la mémoire persistante. Cette opération prend un certain temps. C'est pour cela que si les valeurs ont déjà été calculées, le lance-missile retourne simplement à l'origine pour retrouver sa position précédente. Pendant cette phase de démarrage, seule la commande `52` (connaître l'état du lance-missile) est oppérationnelle.

Une fois l'initialisation terminée, le lance-missile est parfaitement opérationnel. Il attend de reçevoir un ordre depuis un ordinateur ou un Arduino (voir la partie communication en UART).

## Communication en UART

Le lance-missile est contrôlable de la même manière pour les deux moyens (USB et UART). Voici un tableau présentant le protocole utilisé pour effectuer des action et récupérer des informations :

| Position | 0                                      | 1                                                         | Positions suivantes / valeurs retournées                                                                                                                            |
| -------- | -------------------------------------- | --------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
|          | 0 : déplacer la tête pendant une durée | 0 : inclinaison vers le haut                              | 00001 à 99999 : temps en milliseconde                                                                                                                               |
|          |                                        | 1 : inclinaison vers le bas                               | 00001 à 99999 : temps en milliseconde                                                                                                                               |
|          |                                        | 2 : rotation vers la gauche                               | 00001 à 99999 : temps en milliseconde                                                                                                                               |
|          |                                        | 3 : rotation vers la droite                               | 00001 à 99999 : temps en milliseconde                                                                                                                               |
|          | 1 : déplacer la tête pour un angle     | 0 : rotation horizontale                                  | -180 à +180 : angle                                                                                                                                                 |
|          |                                        | 1 : inclinaison verticale                                 | -040 à +040 : angle                                                                                                                                                 |
|          | 2 : déplacer la tête à un angle        | 0 : rotation horizontale                                  | 000 à 180 : angle                                                                                                                                                   |
|          |                                        | 1 : inclinaison verticale                                 | 000 à 040 : angle                                                                                                                                                   |
|          | 3 : action                             | 0 : commencer à déplacer un axe dans un sens              | 0 : vers le haut                                                                                                                                                    |
|          |                                        |                                                           | 1 : vers le bas                                                                                                                                                     |
|          |                                        |                                                           | 2 : vers la gauche                                                                                                                                                  |
|          |                                        |                                                           | 3 : vers la droite                                                                                                                                                  |
|          |                                        | 1 : arrêter le déplacement d'un axe                       | 0 : axe horizontal                                                                                                                                                  |
|          |                                        |                                                           | 1 : axe vertical                                                                                                                                                    |
|          |                                        | 2 : calibrer le lance-missile                             |                                                                                                                                                                     |
|          | 4 : lancer des missiles                | 1 à 3 : nombre de missiles à envoyer                      |                                                                                                                                                                     |
|          | 5 : récupérer des informations         | 0 : récupérer la position de la tête                      | Renvoie la position des deux axes (par exemple : « 090/180,015/040 »)                                                                                               |
|          |                                        | 1 : récupérer l’état des trois missiles                   | Renvoie l’état de chaque missile (par exemple « 001 »)                                                                                                              |
|          |                                        | 2 : permet de savoir si le lance-missile est opérationnel | Renvoie true (1) si le lance-missile est initialisé et calibré, false (0) si ce n’est pas le cas et ne renvoie rien si le lance-missile n’est pas connecté          |
|          |                                        | 3 : récupérer les actions actuelles du lance-missile      | Renvoie une chaîne de caractères : Position 1 = déplacement vertical, position 2 = déplacement horizontal, position 3 = le nombre de missiles en cours de lancement |

Il faut ajouter à la fin de tout message envoyé le caractère "e" afin d'indiquer au programme que c'est la fin de la commande. De son côté, le lance-missile renverra lui-aussi un "e" à la fin de chacun de ses messages.

Pendant le démarrage (calibration ou initialisation), seule la commande `52` (connaître l'état du lance-missile) est oppérationnelle.

Il est important de bien ajouter des zéros, par exemple, il ne faut pas envoyer `0210` : il faut envoyer `0200010`.

Exemples de message :

- Incliner la tête à 20° : `21020`
- Lancer deux missiles : `42`
- Récupérer l'état des trois missiles : `51`
