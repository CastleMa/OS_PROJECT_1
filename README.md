# Objectifs

- [x] Création d'un git ✅ 2024-11-12
- [ ]  [Programme chat en C++](#programme-chat-en-c)
- [ ]  [Programme chat-bot en Bash](#programme-chat-bot-en-bash)
- [ ]  [Tests Auto](#tests-auto)
- [ ]  [Rapport](#rapport)
- [ ] **Deadline** 📅 2024-11-23



# Programme chat en C++

## ToDo
- [x] gestion des erreurs basiques ✅ 2024-11-14
- [ ] l'envoie de messages avec les critères du pdf en utilisant les pipes
- [ ] implémentation des différents signaux SIGINT ET SIGPIPE pour gestion des processus
- [ ] mode manuel avec les 3 cas pour la récéption des messages (SIGINT, mess envoyé par user, +4096bytes en attente)


## Structure
1. [Errors](./include/errors.hpp)
2. [Functions](./src/functions.cpp)
3. [Classes](./src/classes.cpp)
4. [Main](./src/main.cpp)



# Programme chat-bot en Bash

## ToDo
- [ ] voir la section conseils du pdf 

## Structure
1. [Script](./chat-bot)


# Tests Automatiques
*On devrait recevoir des tests automatiques ?*


# Rapport

## Todo 
- [ ] Choix du template latex si latex car peut être markdown si flemme et pas le temps

## Structure 
<small>*une sorte de README mais en rapport*</small>
1. [Introduction et présentation](#introduction-et-présentation)
2. [Installation](#installation)
3. [Liste des fonctionnalités](#liste-des-fonctionnalités)
4. [Usage et illustrations](#usage-et-illustrations)
5. [Difficultés rencontrées](#difficultés-rencontrées)
6. [Solutions apportées](#solutions-apportées)
7. [Choix d'implémentation](#choix-dimplémentation)
8. [Conclusion](#conclusion)


## Notions à mettre dans le rapport
- Justification de l'utilisation du type de mémoire partagée et la quantité (4096bytes à priori)
- Précisions sur les signaux utilisés pour le projet
- Brèves explications sur les retours d'erreurs, SIGINT, mauvais arguments, pseudo trop long, caractères spéciaux
- Justifications du processus utilisé parmi les 2 lors des 3 scénarios du mode --manuel
- Justifications du processus utilisé parmi les 2 lors des 3 scénarios du mode --manuel
- Voir section Conseils du pdf pour préciser l'implémentation de ces conseils
- 2-3 pages



## Brouillon de texte

### Introduction et présentation

Ce projet de INFO-F201 consiste en un petit programme permettant de discuter avec une personne ou un robot localement à l'aide de pipes nommés. Nous allons d'abord expliquer comment installer le programme et le faire tourner. Puis lister les fonctionnalités de ce dernier, montrer des exemples d'utilisation. Ensuite, nous discuterons des difficultés rencontrés et des solutions apportées en justifiant nos choix d'implémentation. Enfin, nous terminerons par faire une conclusion du projet en donnant notre sentiment global de cette aventure.


### Installation

Nous avons pour le projet décidé de faire un dépot github pour faciliter la mise en oeuvre du projet. Comme les fichiers sources et un *Makefile* se trouvent dans le zip, vous pouvez compiler et exécuter le programme depuis le chemin de ce dernier à l'aide de *make*. Alternativement si vous perdez le fichier zip, vous pouvez exécuter ces commandes dans le répertoire de votre choix:

 ```bash
git clone https://github.com/CastleMa/OS_PROJECT_1
cd OS_PROJECT_1
make
```


### Liste des fonctionnalités
Les fonctionnalités proposées sont les suivantes:
```bash
--manuel
```
Permet de recevoir les messages de l'interlocuteur sous certaines conditions.
```bash
--bot
```
Permet de discuter avec un bot et exécuter les commandes suivantes:
- « liste » : lister tous les fichiers du dossier de travail du robot;
- « li FICHIER » : lire le contenu du fichier « FICHIER » (ou tout autre nom choisi) ou
affiche une erreur en cas d’échec (p.ex., si le fichier est inexistant);
- « qui suis-je » : donne le pseudonyme du destinataire;
- « au revoir » : termine le robot avec le code de retour 0


### Usage et illustrations
Dans le répertoire où se trouve le fichier exécutable, faites:
```bash
./chat
```
*Screenshot d'utilisation*



### Difficultés rencontrées, solution apportées, choix d'implémentation

L'utilisation de C n'était pas possible quant à l'utilisation de certaines librairies comme *\<iostream>*, alors nous avons choisi d'utiliser C++. La gestion des erreurs s'est faite via des macros dans un fichier *header* pour permettre une meilleure lisibilité et modularisation.



### Conclusion

blabla