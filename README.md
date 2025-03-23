# TP_FTP
Mini projet SR d'un protocole de transfère de fichier, application client-serveur


Concernant les 2 premières questions on a tout simplement retranscrit en C ce qui était demandé à savoir définir un type de structure de données et un type énuméré.

Pour la question suivante nous avons recycler le code du tp précédent dans lequel on créer un pool de processus à l'aide d'une boucle for, de la fonction fork() et d'un entier NPROC (nombre processus) qui représente le nombre max de processus fils qui peut être utilisé simultanément. 

Question 4: Pour cette question nous avons rencontré pas mal de problème notamment avec l'envoi des SIGINT vers les fils. On a créer un "handlerPapa" qui gère les SIGINT envoyé sur le serveur principal et qui à l'aide de la fonction Kill fait terminer tous les autres processus qui lui sont associés. Kill a besoin qu'on lui donne en paramètre le pid du processus qu'on veut stopper, on a donc initialisé un tableau de pid_t contenant tous les pid des processus créer dans la boucle for. Cependant on avait gardé le handler de SiGCHLD des tp précédents pour le traitement des zombies mais cela ne fonctionnait pas correctement car lorsqu'on fait un Ctrl C un SIGINT pouvait être envoyé à un processus fils en priorité et non pas au père et donc cela n'arrêtait pas le programme. On a donc fait un autre handler "handlerFils" qui lui gère les SIGINT directement envoyé au processus fils. Et donc maintenant le serveur s'arrête proprement lorsqu'il reçoit un signal d'arrêt.