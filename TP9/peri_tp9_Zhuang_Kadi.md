# TP9: Modèle client-serveur et MQTT

```
Yasma KADI
Kailun ZHUANG
```

### Objectif

Le but de ce TP est de créer un réseau de capteurs avec MQTT. 

### Client-serveur d'une application de vote

Dans un premier temps, on a commenté le code fourni. 
__Le serveur__ :

- Créer un point de connection (Socket)

```c
sockfd = socket(AF_INET, SOCK_STREAM, 0);
```
AF_INET : IPv4
SOCK_STREAM : TCP

- Attacher la socket à un port après l'avoir converti en [big-endian](https://en.wikipedia.org/wiki/Endianness)

```c
bzero((char *) &serv_addr, sizeof(serv_addr));
int portno = atoi(argv[1]);// convert the portno as string to an int
struct sockaddr_in serv_addr.sin_port = htons(portno);// convert the port number to bigendian
```

- Ajouter les informations de connection

```c
serv_addr.sin_family = AF_INET;//IPv4
serv_addr.sin_addr.s_addr = INADDR_ANY;//Takes one of the available interfaces
```

- Lier le socket avec une structure sockaddr : "nommer" la socket

```c
 if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) // if not bound correctly
        error("ERROR on binding"); // display error
```

- Écouter sur la socket avec un nombre maximum de connections pendantes = 5

```c
 listen(sockfd, 5); 
```

- Attendre qu'un client se connecte

```c
int newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
```

##### Threads

Ensuite, on a "threadé" le serveur. Dans le processus `main`, on attend constamment une nouvelle connection, à chaque fois qu'une connection est établie, on passe la main à un thread qui s'occupe du client.


Le code du serveur qui permet de recueillir les votes : 

```c
     // dans le main
      while (1) 
        {
                //accept a connection to server from client pending list
                newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
                if (newsockfd < 0) 
                {
                    error("ERROR on accept");
                }
                //display the client IP address and its port number 
                printf("Received packet from %s:%d\n\n",
                inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));

                struct info_connection *to_thread = malloc(sizeof(struct info_connection));
                char * addr = inet_ntoa(cli_addr.sin_addr);

                to_thread->IP_Address = addr;
                to_thread->sockfd = newsockfd;
                if (pthread_create(&vote, NULL, connection_handler, to_thread))
                {
                        perror("pthread_create failed");
                }


                pthread_detach(vote);
            }
```

```c


//Test si le user a déjà voté
int already_exist(const char * username){
    int i;
    for (i = 0; i < pos_tab; ++i)
    {
        if (strcmp(name_tab[i],username)==0)
        {
            return 1;
        }
    }
    return 0;
}

//Ajouter un nouveau utilisateur
void add_user(const char * username)
{
    int len_user = strlen(username);
    char * new_user = malloc (sizeof(char)*(len_user+1));
    strcpy(new_user, username);
    name_tab[pos_tab++] = new_user;
}

//Test si le vote est valide
int choice_valid(char choice)
{
    if ((choice & ~1) == 0 ) 
    { 
        return 1;
    }
    return 0; 
}

//Thread qui gère les connections i.e vote
void * connection_handler(void* arg){
        struct info_connection * info = (struct info_connection *) arg;
        int newsockfd = info->sockfd;
        char buffer[256];
        bzero(buffer, 256);
        int n, ack = ACK_OK;
        n = read(newsockfd, buffer, 255);
        if (n < 0){
                perror("ERROR reading from socket");
        }

        printf(" Data: [%s]\n", buffer);
        printf(" SUMMER : %d vs WINTER : %d\n", vote[SUMMER], vote[WINTER]);

        if (already_exist(&buffer[1])== 1)
        {
            printf("Already registered\n");
            ack = ACK_ALREADY_REGISTERED;
            goto _ack;
        }

        if ( choice_valid(buffer[0])==0)
        {
            printf("Invalid choice\n");
            ack = ACK_INVALID_CHOICE;
            goto _ack;
        }

        switch (buffer[0]) 
        {
            case SUMMER :
            case WINTER :
                ++vote[(int)buffer[0]];
                break;
            default:
                break;
        }

_ack:
        sprintf(buffer, "%d", ack);
        n = write(newsockfd,buffer,strlen(buffer));
        if (n != strlen(buffer))
            printf("ERROR message not fully trasmetted");

        free(info);
        close(newsockfd);
        return 0;
}
```

Le code du client qui permet de voter :

```c
#define ARG_IP          1
#define ARG_PORT        2
#define ARG_NAME        3
#define ARG_CHOICE      4

void error(const char *msg)
{
        perror(msg);
        exit(0);
}

//argv[1] <-- server name, argv[2] <-- port number
int main(int argc, char *argv[])
{
        int sockfd, portno, n;
        struct sockaddr_in serv_addr;
        struct hostent *server;

        char * username = argv[ARG_NAME];

        /* octet 0: choix, puis username */
        char buffer[256];

        // Le client doit connaitre l'adresse IP du serveur, et son numero de port
        if (argc < 5) {
                fprintf(stderr,"usage %s hostname port\n", argv[0]);
                exit(0);
        }
        portno = atoi(argv[ARG_PORT]);

        // 1) Création de la socket, INTERNET et TCP

        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0)
                error("ERROR opening socket");

        //Convert name of argv to IP address
        server = gethostbyname(argv[ARG_IP]);
        if (server == NULL) {
                fprintf(stderr,"ERROR, no such host\n");
                exit(0);
        }

        // On donne toutes les infos sur le serveur

        bzero((char *) &serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
        serv_addr.sin_port = htons(portno);

        // On se connecte. L'OS local nous trouve un numéro de port, grâce auquel le serveur
        // peut nous renvoyer des réponses, le \n permet de garantir que le message ne reste
        // pas en instance dans un buffer d'emission chez l'emetteur (ici c'est le clent).

        if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
                error("ERROR connecting");

        /* fill buffer to send */
        if (strcmp(argv[ARG_CHOICE], "summer") == 0) {
                sprintf(buffer,"%d\n", 0);
        } else if (strcmp(argv[ARG_CHOICE], "winter") == 0){
                sprintf(buffer, "%d\n", 1);
        } else {
        	printf("Votre Choix est invalide soit summer, soit winter.\n");
        	return 1;
        }
        strcpy(&buffer[1], username);
        
        /* send buffer to server */
        n = write(sockfd,buffer,strlen(buffer));
        if (n != strlen(buffer))
                error("ERROR message not fully trasmetted");

        n = read(sockfd, buffer, 255);
        if (n < 0){
            perror("ERROR reading from socket");
        }
        printf("ack : %d\n", buffer[0]);
        // On ferme la socket
        close(sockfd);
        return 0;
}
```
##### Vote MQTT avec des pins ESP32 TOUCH

On a réalisé un envoi de vote par l'intermédiaire des pins `touch` de l'ESP32. On a défini les pins suivants :
```c
int touch_summer= 32;
int touch_winter = 12;
```
Selon la valeur lue sur les pins, on exécute un certain comportement. 

```c
	 if (touchRead(touch_summer) < 10) // pin summer touché
	 {
	      snprintf (msg, 75, "touch summer: %d", touchRead(touch_summer));
	      Serial.print("Publish message: ");
	      Serial.println(msg);
	      client.publish(OUT_TOPIC, msg);
	      lastMsg = now;
	      ++value;
	}

    int touch_val = touchRead(touch_winter);
    if(touch_val < 15 && touch_val>0) // pin winter touché
    {
      snprintf (msg, 75, "touch winter : %d", touch_val);
      Serial.print("Publish message: ");
      Serial.println(msg);
      client.publish(OUT_TOPIC, msg);
      lastMsg = now;
      ++value;
    }
```
###### Remarque :
Les `touch` pins ont des valeurs initiales différentes, ce qui explique la différence des seuils dans le code.

##### Protocole Subscribe-Publish

MQTT est un protocole publish-subscribe permettant aux appareils   du network  de publier vers un broker. Les clients se connectent à ce broker qui joue ensuite le rôle de médiateur entre les deux appareils. Chaque appareil peut souscrire à des topics particuliers. Lorsqu’un autre client publie un message sur un topic, le broker transfère le message à tous les autres clients qui y ont souscrit.

__Instalation__

```bash
sudo apt-get install mosquitto

sudo apt-get install mosquitto-clients

ssh -l pi -p 2246 -X -Y peri
```

###### En ligne de commande
__Publish__

```bash
 mosquitto_pub -t 'peri' -m 'hello' 
```
__Subscribe__

```bash
 mosquitto_sub -t 'peri'
```
###### Dans le code C

```c
      client.publish(OUT_TOPIC, "init");
      client.subscribe(IN_TOPIC);
```

