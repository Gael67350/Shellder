# Shellder
Projet de système S6 - Shell développé avec les primitives systèmes UNIX et POSIX en C

### Objectifs

Le but de ce projet est de programmer un shell en C. Ce projet exploratoire a pour but de se familiariser avec les couches basses du système.

Le shell est une simple boucle d’interaction qui affiche un invite de commande de la forme user@machine:repertoire/courant>, lit une ligne entrée au clavier, l’exécute, et recommence. Le shell quitte quand l’utilisateur tape ctrl-D à la place d’une commande ou bien lorsque la commande exit est tapée.

### Compilation et exécution

Avant de pouvoir l’exécuter, le projet doit être compilé. Pour cela, une makefile est mise à disposition. Il suffit donc de saisir la commande ci-dessous pour compiler le projet (il est nécessaire de se positionner à la racine du projet, dans le même répertoire que la makefile) :

```
make
```

Une fois compilé, Shellder peut être exécuté. Pour cela, veuillez vous positionner dans le répertoire bin du projet (nécessaire pour utiliser le shell avec les commandes fournies) et exécuter le programme Shellder comme ci-dessous :

```
./Shellder
```

À tout moment, il est possible de quitter Shellder en tapant la commande ```exit``` ou ```CTRL+D```.
La commande ```cd``` permet de changer de répertoire courant.
Il est aussi possible d’exécuter des commandes et programmes externes à Shellder en fournissant le chemin absolu ou relatif vers celui-ci.

### Fonctionnalités du noyau

#### Tubes et redirections

Les redirections ont bien été implémentées dans Shellder et fonctionnent de la même manière que pour la console classique de Linux. Il est d’ailleurs possible d’effectuer des redirections à partir/vers les trois descripteurs de fichiers standards (standard d’entrée, standard de sortie et standard d’erreur), ceci quelque soit l’ordre des redirections.

Les tubes ont été implémentés dans une logique récursive en utilisant une structure de commande chaînée. Cela permet donc de saisir des commandes contenant plusieurs pipes. Cependant, lors des tests, nous avons constatés que cette fonctionnalité ne fonctionne pas comme initialement prévu. En effet, lors du parsing de la commande pour récupérer les commandes intermédiaires se trouvant entre les pipes, nous avons constaté que l’appel à la fonction en charge de la construction de la structure de commande interrompait l’exécution de la boucle de parsing, faisant appel à la fonction strtok() pour récupérer les commandes les unes à la suite des autres, à la suite de chaque pipe.

#### Gestion des variables

Le noyau de Shellder dispose de fonctionnalités permettant de manipuler les variables d’environnement telles que le PATH.
La commande getenv permet de récupérer le contenu d’une variable d’environnement :
```
getenv VARIABLE
```

La commande ```setenv``` permet de modifier le contenu d’une variable d’environnement (elle remplace la valeur précédente pour cette variable si celle-ci existait déjà) :
```
setenv VARIABLE VALEUR
```

#### Gestion des tâches

Avec Shellder, il est possible de lancer une commande en tant que tâche de fonds. Pour cela, il est nécessaire de mentionner le caractère « & » comme argument à la fin de la ligne de commande.
Par exemple, pour lancer le programme xclock avec Shellder, il est nécessaire de saisir la commande suivante :
```
/usr/bin/xclock &
```

### Descriptions des commandes

#### Copie de fichiers et de répertoires

```
cp <path of the original file> <path of the output file>
```

La copie de fichiers et de répertoires est assurée par l’intermédiaire de la commande cp. Cette commande sert principalement à copier des fichiers (et éventuellement des répertoires). On peut aussi bien copier un fichier donné vers une destination précise que copier un répertoire de fichiers de manière récursive.
Les autorisations d’accès des fichiers et des répertoires créés seront les mêmes que celles des fichiers d’origine.

#### Concaténation de chaînes de caractères

```
cat [FILE]...

--help Afficher un message d’aide sur la sortie standard.
```

La concaténation de chaînes de caractères est assurée par l’intermédiaire de la commande cat. Cette commande affiche sur la sortie standard le contenu de chacun des fichiers indiqués (ou le contenu de l’entrée standard si aucun nom de fichier n’est fourni, ou si le caractère « - » est indiqué).

#### Affichage du contenu d'un répertoire

```
ls [OPTIONS] [FILE]...

-l En plus du nom, afficher le type du fichier, les permissions d’accès, le nombre de liens physiques, le nom du propriétaire et du groupe, la taille en octets et l’horodatage. Le nombre de blocs total utilisés est affiché par blocs de 512 octets. L’horodatage présenté est celui de la dernière modification du fichier.

-a Afficher tous les fichiers du répertoire, y compris les fichiers commençant par « . ».
```

L’affichage du contenu d’un répertoire est assuré par l’intermédiaire de la commande ls. Cette commande affiche l’ensemble de ses arguments file et s’il s’agit d’un répertoire, affiche l’ensemble des fichiers contenus dans ce répertoire. Si aucun autre argument autre qu’une option n’est fournie, le répertoire en cours est pris par défaut.

Par défaut, un fichier n’est affiché que si son nom ne commence pas par un point (sauf si l’option -a est fournie) et une couleur spécifique est appliquée sur le nom du fichier affiché en fonction de son type (les répertoires en bleu, les fichiers exécutables en vert). Si l’affichage de ls est redirigé vers un autre descripteur de fichier, qui n’est pas un terminal, alors les couleurs ne sont pas affichées.

**Note : Il est possible de cumuler plusieurs options derrière un même caractère « - » (les expressions « -al » et « -la » permettent de fournir simultanément les options « -a » et « -l ».**

####Recherche d'un fichier dans l'arborescence du système

```
find [PATH]... [EXPRESSION]

-name motif Considère uniquement les fichiers dont le nom de base (sans les répertoires du chemin d’accès) correspond au motif donné.

-exec commande Exécute la commande pour chacun des fichiers rencontrés. Tous les arguments suivants sont considérés comme des arguments pour la ligne de commande. L’argument « {} » est remplacé par le nom du fichier en cours de traitement. La commande est exécutée depuis le répertoire de départ.
```

La recherche d’un fichier dans l’arborescence du système est assurée par l’intermédiaire de la commande find. Cette commande parcourt les arborescences de répertoires commençant par chacun des chemins donnés, en évaluant les expressions fournies pour chaque fichier rencontré.

Si aucun chemin n’est mentionné, le répertoire en cours sert de point de départ.
Si aucune expression n’est fournie, find affiche le résultat dans la sortie standard.

La commande se retourne avec le code de retour 0 si tous les fichiers ont pu être examinés correctement, et -1 si une erreur s’est produite.

**Note : Les expressions « -name » et « -exec » peuvent être utilisées simultanément mais l’expression « -name » doit obligatoirement être utilisée avant l’expressions « -exec ».**
