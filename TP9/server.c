/* A simple server in the internet domain using TCP The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>

#define SUMMER  0
#define WINTER  1

#define ACK_OK                  0
#define ACK_ALREADY_REGISTERED  1
#define ACK_INVALID_CHOICE      2
#define ACK_ERROR               3

struct info_connection
{
        int sockfd;
        char * IP_Address;
};

static char* name_tab[256];
static unsigned int pos_tab = 0;
static int vote[2];

/* Display error message and quit the program */
void error(const char *msg)
{
        perror(msg);
        exit(1);
}

/* Add all chars in the string 
unsigned long hash(unsigned char *str)
{
        unsigned int hash = 0;
        int c;
        
        while (c = *str++)
            hash += c;
        
        return hash;
}
*/


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
        
 /*       if (strcmp(buffer, "ok\n") == 0){
                int hash_value = hash(info->IP_Address)%256;
                } else {
                        ok++;
                        hash_tab[hash_value] = 1;
                        printf("Registered\n");
                }
        }
        else if (strcmp(buffer, "ko\n") == 0){
                int hash_value = hash(info->IP_Address)%256;
                if (hash_tab[hash_value]) {
                        printf("Already registered\n");
                } else {
                        ko++;
                        hash_tab[hash_value] = 1;
                        printf("Registered\n");
                }
        }
*/




_ack:
        sprintf(buffer, "%d", ack);
        n = write(newsockfd,buffer,strlen(buffer));
        if (n != strlen(buffer))
            printf("ERROR message not fully trasmetted");

        free(info);
        close(newsockfd);
        return 0;
}


/* arg[1] <-- PORT NO */
int main(int argc, char *argv[])
{
        int sockfd, newsockfd, portno;
        socklen_t clilen;
        struct sockaddr_in serv_addr, cli_addr;

        pthread_t vote;

        if (argc < 2) {
                fprintf(stderr, "ERROR, no port provided\n");
                exit(1);
        }

        // 1) on crée la socket, SOCK_STREAM signifie TCP

        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0)
                error("ERROR opening socket");

        // 2) on réclame au noyau l'utilisation du port passé en paramètre 
        // INADDR_ANY dit que la socket va être affectée à toutes les interfaces locales

        bzero((char *) &serv_addr, sizeof(serv_addr));
        portno = atoi(argv[1]);// convert the portno as string to an int
        serv_addr.sin_family = AF_INET;//IPv4
        serv_addr.sin_addr.s_addr = INADDR_ANY;//Takes one of the available interfaces
        serv_addr.sin_port = htons(portno);// convert the port number to bigendian
        if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) // if not bound correctly
                error("ERROR on binding"); // display error


        // On commence à écouter sur la socket. Le 5 est le nombre max
        // de connexions pendantes

        listen(sockfd, 5); 
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

        close(sockfd);
        return 0;
}