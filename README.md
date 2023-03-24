# Projet de Système d'Exploitation

Projet réalisé à l'ENSIMAG consistant en la réalisation d'un système d'exploitation gérant le strict minimum pour pouvoir opérer dessus :
- Gestion de l'écran
- Gestion du temps
- Gestion des processus ( interruptions, ordonnanceur, changement de contexte, terminaison et création de processus )

# Pour le compiler

Notez qu'il faudra peut-être ajouter dans votre PATH les répertoires dans lesquels les logiciels seront installés, et modifier le Makefile fourni pour ajouter les bons préfixes (par exemple i386-pc-elf-) aux binaires appelés.

- Si vous êtes sous Linux :

Notez qu'il faut une version 32 bits de GCC pour compiler le noyau : si votre système ne dispose que d'outils 64 bits, vous devrez installer les bibliothèques 32 bits. Les noms des paquets dépendent de la version de Linux mais il vous faut : l'outil make, l'émulateur qemu (si plusieurs versions sont proposées, choisissez qemu-system-i386), l'outil de mise au point gdb et les bibliothèques GCC 32 bits (libc6-dev-i386 sous Debian ou compatibles et libgcc.i686, glibc.i686, glibc-static.i686 et glibc-devel.i686 sous CentOS et compatibles).

Remarque : sur les versions récentes de GCC fournies avec Linux, il peut être nécessaire d'ajouter l'option -fno-pic dans les options de compilation du Makefile si vous obtenez une erreur d'accès mémoire lors de l'utilisation de la fonction malloc.

Autre remarque : si vous utilisez une version très récente de Linux, vous avez peut-être la version 10 de GCC (tapez gcc -v pour savoir) qui peut ne pas fonctionner (cela dépend de la version du système) avec les sources distribuées. Plusieurs solutions dans ce cas :

essayez d'ajouter l'option -march=pentium dans les options de compilation (CFLAGS) dans le Makefile ;
sinon vous pouvez installer la version 9 de GCC avec votre gestionnaire de paquets préféré : attention, il est possible qu'il y ait des conflits avec la version 10

- Si vous êtes sous macOS

Il vous faut un environnement de développement gérant le format ELF 32 bits, ce qui n'est pas le cas du compilateur CLANG fourni par Apple.

Si vous utilisez le gestionnaire de paquets Homebrew, il suffit de lancer la commande suivante pour installer tout ce qu'il faut : brew install i686-elf-binutils i686-elf-gcc i386-elf-gdb qemu.

Sinon, vous pouvez télécharger cette archive testée sous Yosemite et la décompresser dans le répertoire /usr/local.
