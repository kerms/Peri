
# TP7: Serveur WEB minimaliste

```
Yasma KADI
Kailun ZHUANG
```

### Communication par FIFO

[//]: # (Dans quel répertoire est créée la fifo ?)

	* La fifo est créée dans le répertoire /tmp.

```c
char    *myfifo = "/tmp/myfifo";
```

[//]: # (Quelle est la différence entre mkfifo et open ?)

	* `mkfifo` sert à créer une fifo. Quant au `open`, il sert à l'ouvrir.

[//]: # (Pourquoi tester que la fifo existe ?)

	* Si la fifo n'existe pas, le programme devrait s'arrêter. Donc si on ne teste pas, le programme peut faire n'importe quoi (dans notre cas : boucle infinie). En outre, dans le cas où on veut la créer, il faut s'assurer qu'on n'écrase pas celle qui existe.
	
	Ainsi, on a rajouté quelques lignes pour vérifier si le fichier existe, sinon on quitte le programme.

```c
/* create the FIFO (named pipe) */
err = mkfifo(myfifo, 0666);
if (err == -1) {
	perror(myfifo);
	return err;
}

/* write "Hi" to the FIFO */
fd = open(myfifo, O_WRONLY);
if (fd == -1) {
	perror(myfifo);
	return -1;
}
```

[//]: # (À quoi sert flush ?)

	* Lorsqu'on utilise des appels systèmes tels que `write`, `printf`, le système stocke les données dans un buffer, pour les envoyer par gros packets. Tout cela pour ne pas faire trop d'appels systèmes pour peu de données. Par ailleurs, on ne sait pas si les données ont été vraiment écrites/envoyées. Flush permet de bien envoyer/écrire en vidant le buffer.

[//]: # (Pourquoi ne ferme-t-on pas la fifo ? )

	* À la fin de l'exécution du programme, la fifo est fermée automatiquement par le `garbage collector` qui "recycle" la mémoire. 

[//]: # (Que fait readline ?)

	* Le `readline` permet de lire des caractères d'une ligne jusqu'à ce qu'il atteind un `\n`.

	* Lorsqu'on lance un écrivain (en C ou en Pyhton) rien ne se passe tant qu'on n'a pas lancé un lecteur. En effet, `pipe_out.flush()` ne peut pas vider le buffer tant que le lecteur ne lit pas la fifo. Le pipe est "bouché" du côté lecteur.


### Création d'un serveur fake

La fonction `select` prend en arguments un descripteur de fichier, un flag `fd_set` et un timeout `struct timeval`. 
Cette fonction permet l'attente sur plusieurs descripteurs de fichiers.

Pour chaque set de descripteurs

```c
FD_ZERO(&fdset);
```

Pour chaque descripteur de fichier, on associe le set correspondant.

```c
FD_SET(fd1, &fdset);
FD_SET(fd2, &fdset);
```

Puis, on attend qu'un événement se produise :

```c
if (select(s2f+1, &rfds, NULL, NULL, &tv) != 0) {   // wait until timeout
    if (FD_ISSET(f1, &rfds)) {
		/* code */
    }

    if (FD_ISSET(f2, &rfds)) {
    	/* code */
    }
}
```

Pour lire l'entrée clavier en Python2, on utilise la fonction `raw_input()` comme suit :

```python
string = raw_input("Enter your message : ")
s2f.write(string + "\n") # ne pas oublier le '\n'
```


### Création d'un serveur web

On a repris le code du tp1 puis mis le code de fake.c dans un thread  : 

```c

void* listen_fifo(void *arg) {

    int     f2s, s2f;                                  // fifo file descriptors
    char    *f2sName = "/tmp/f2s_fw";                  // filo names
    char    *s2fName = "/tmp/s2f_fw";                  //
    char    serverRequest[MAXServerResquest];          // buffer for the request
    fd_set  rfds;                                           // flag for select
    struct  timeval tv;                                     // timeout
    tv.tv_sec = 1;                                          // 1 second
    tv.tv_usec = 0;
    struct gpio_info * info = (struct gpio_info *) arg;
    printf("threads fifo created\n");

    /* test mkfifo */
    result = setup_fifo(s2fName, f2sName, s2f, f2s);
    if ( result != 0 ) {
        exit ( 1 );
    }

    do {
        FD_ZERO(&rfds);                                     // erase all flags
        FD_SET(s2f, &rfds);                                 // wait for s2f
        FD_SET(STDIN_FILENO, &rfds);

        if (select(s2f+1, &rfds, NULL, NULL, &tv) != 0) {   // wait until timeout
            if (FD_ISSET(s2f, &rfds)) {                     // something to read
                int nbchar;
                if ((nbchar = read(s2f, serverRequest, MAXServerResquest)) == 0) break;
                serverRequest[nbchar]=0;
                fprintf(stderr,"[server]: %s", serverRequest);
                write(f2s, serverRequest, nbchar);

                /***********************
                 * Gestion de requete  *
                 ***********************/
                if (strcmp(serverRequest, "on\n") == 0) {
                    info->on = 1; /* turn on the LED */
                } else if (strcmp(serverRequest, "off\n") == 0) {
                    info->on = 0;
                }
            }

            if (FD_ISSET(STDIN_FILENO, &rfds)) {
                int nbchar;
                if ((nbchar = read(STDIN_FILENO, serverRequest, MAXServerResquest)) == 0) break;
                serverRequest[nbchar]=0;
                fprintf(stderr,"[stdin ]: %s", serverRequest);
            }
        }
    }
    while (1);

    close(f2s);
    close(s2f);
}
```

configurer le navigateur :

	- ne pas rediriger peri

Puis 

	- lancer le server et le programme C
	- ouvrir la page sur l'adresse peri:8025 
	- entrer on/off

On voit bien la LED s'allumer ou s'éteindre selon on/off

##### Remarque 

Il faut laisser le port du server à 8000 ou 8100.

Lorsque le client entre une donnée, le '\n' est envoyé, donc il ne faut pas l'oublier dans le `strcmp`.
