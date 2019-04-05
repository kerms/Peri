
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

[//]: # (Quelle différence mkfifo et open ?)

	* `mkfifo` sert à créer une fifo. Quant au `open`, il sert à l'ouvrir.

[//]: # (Pourquoi tester que la fifo existe ?)

	* Si la fifo n'existe pas, le programme devrait s'arrêter. Donc si on ne teste pas, le programme peut faire n'importe quoi (dans notre cas : infinite loop).
	
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

	* Lorsqu'on utilise des appels systèmes tels que `write`, `printf`, le système stoque les données dans un buffer, pour envoyer par gros packet. Tout cela pour ne pas faire trop d'appels systèmes pour peu de données. Par ailleurs, on ne sait pas si les données ont été vraiment écrites/envoyées.

	Flush permet de bien envoyer/écrire en vidant le buffer.

[//]: # (Pourquoi ne ferme-t-on pas la fifo ? )

[//]: # (//TODO)

[//]: # (Que fait readline ?)

	* Le `readline` permet de lire des caractères jusqu'à ce qu'il atteind un `\n`.

	* Lorsqu'on lance un écrivain (en C ou en Pyhton) rien ne se passe tant qu'on n'a pas lancé un lecteur. En effet, `pipe_out.flush()` ne peut pas vider le buffer tant que le lecteur ne lit pas sur la fifo. Le pipe est fermé du côté de lecteur.




### Création d'un serveur fake

La fonction `select` prend en arguments un descripteur de fichier, son flag `fd_set` et un timeout `struct timeval`. 
Cette fonction nous permet d'attendre sur plusieurs descripteurs de fichiers.

Pour chaques sets de descripteurs

```c
FD_ZERO(&fdset);  
```

Pour chaques descripteurs de fichiers, on associe à cet set.

```c
FD_SET(fd1, &fdset);
FD_SET(fd2, &fdset);
```

Puis on attend qu'un événement arrive :

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

