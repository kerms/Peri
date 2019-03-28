# TP4: Pilotage d'un écran LCD en mode utilisateur et par un driver

```
Yasma KADI
Kailun ZHUANG
```

### Configuration des GPIO pour le pilotage de l'écran

- On peut configurer les GPIOs en utilisant la fonction `gpfsel()` qui permet d'écrire dans les registres GPFSELx (1: output, 0: input). Dans notre cas, on considère les signaux RS, E et Dx comme des sorties (pour la raspberry pi).

- On peut utiliser la fonction `gpio_write` pour écrire des valeurs dans les registres de l'écran LCD.

- Pour réaliser l'initialisation, en envoie les valeurs suivantes (en mode 4 bits) :
 
```cpp
lcd_command(0b00110011);
lcd_command(0b00110010);
```

- Pour afficher un caractère, on écrit `1` dans `RS`. Ensuite, on met le curseur : `LCD_SETDDRAMADDR + offset`, et on envoie l'octet avec `lcd_write4bits`.

- Pour lancer une commande telle que l'effacement de l'écran, le déplacement du curseur ..., on écrit `0` dans `RS`, puis on envoie l'octet correspandant à la commande.

Exemples de commandes et leurs octets:
```
LCD_CLEARDISPLAY        0b00000001
LCD_RETURNHOME          0b00000010
LCD_ENTRYMODESET        0b00000100
LCD_DISPLAYCONTROL      0b00001000
LCD_CURSORSHIFT         0b00010000
LCD_FUNCTIONSET         0b00100000
LCD_SETCGRAMADDR        0b01000000
LCD_SETDDRAMADDR        0b10000000
```
### Fonctionnement de l'écran et fonctions de base


##### Q1

Le mot clé `volatile` sert à indiquer au compilateur de ne pas optimiser les accès à la variable avec laquelle il est utilisé.

##### Q2

Les flags dans `open` et `mmap` définissent les droits d'accès.

`O_RDWR` : ouvrire en lecture et écriture \
`O_SYNC` : bloquant jusqu'à la terminaison des accès mémoire\
`PROT_READ | PROT_WRITE` : donner la permission lecture et écriture\
`MAP_SHARED` : partager la mémoire avec les autres processus.

##### Q3

La fonction `munmap` permet de libérer les ressources allouées avec `mmap`.

##### Q4

Les masques servent à configurer l'affichage:

- Pour ajuster l'affichage 
```
LCD_EM_RIGHT
LCD_EM_LEFT 
LCD_EM_DISPLAYSHIFT
LCD_EM_DISPLAYNOSHIFT
```
- Pour contrôler l'activation/désactivation de l'affichage (écran + curseur)
```
LCD_DC_DISPLAYON
LCD_DC_DISPLAYOFF
LCD_DC_CURSORON 
LCD_DC_CURSOROFF
LCD_DC_BLINKON 
LCD_DC_BLINKOFF
```
- Pour afficher le déplacement sur l'écran
```
LCD_CS_DISPLAYMOVE
LCD_CS_CURSORMOVE
LCD_CS_MOVERIGHT
LCD_CS_MOVELEFT
```
- Pour définir la fonction
```
LCD_FS_8BITMODE 
LCD_FS_4BITMODE
LCD_FS_2LINE
LCD_FS_1LINE
LCD_FS_5x10DOTS
LCD_FS_5x8DOTS
```

##### Q5

La fonction `lcd_message` remplit un tableau de taille 4 x 20.\
Elle écrit sur la ligne suivante si elle arrive au bout de la ligne, dans la limite de 80 caractères.

2. Positionnement du curseur

```cpp
void lcd_set_cursor(int x, int y)
{
	// condition invalide
    if (x > 19 || y > 3 || x < 0 || y < 0) {
        printf("bad cursor\n");
        return;
    }
    int a[] = { 0, 0x40, 0x14, 0x54 };
    lcd_command(LCD_SETDDRAMADDR + a[y] + x);
}
```

3. Affichage des paramètres sur le LCD

```cpp
int i, k;
for (i = 1; i < argc; ++i)
{
    lcd_set_cursor(5+i, i%4);
    for (k = 0; k < strlen(argv[i][k]); ++k)
    {
        lcd_data(argv[i][k]);
    }
}
```

### Driver pour le LCD

On crée un nouveau fichier `lcd_driver.c` qui reprend les bases du TP3 et du fichier `lcd_user.c`.

En mode noyau on n'a pas besoin de faire un `mmap` contrairement au mode user.

Il faut faire attention de mettre l'adresse dans le driver différemment qu'en mode USER :

```cpp
/* mode noyau */
volatile *gpio_regs = (struct gpio_s *)__io_address(GPIO_BASE);
```
sinon cela produirait des erreurs de type 

```bash
Message from syslogd@raspberrypi at Jun  2 06:25:36 ...
 kernel:[   61.960617] 3f60: 00000dcc 00000000 00000000 00000000 00000020 00000021 00000018 00000015
```

Le module serait alors bloqué, et impossible de faire rmmod.

Puis on initialise la lcd dans le init du module.

###### Remarque
`.ioctl` ne marche pas, il faut le remplacer par `.unlocked_ioctl`. Il faut alors modifier le prototypage de la nouvelle fonction (les deux `ioctl` n'ont pas le même prototype).

Ce qui nous mène à implémenter la fonction d'`ioctl`.

Mais avant, il faut définir des fonctions spéciales de type `_IO`, le mieux est de mettre ces définitions dans un fichier .h qui sera aussi utilisé par le programme utilisateur.

Dans notre cas :

```c
#define LCDIOCT_CLEAR _IO(MAGIC, 1)
#define LCDIOCT_SETXY _IOW(MAGIC, 2, struct Pos *)
```

Puis on les utilise comme des numéros de commande :

```c
static long my_ioctl(struct file *filep,unsigned int cmd, unsigned long arg) {
	struct Pos p;
    switch(cmd)
    {
  		case LCDIOCT_CLEAR :
	      	lcd_clear();
	      	return 0;
    	case  LCDIOCT_SETXY :
	        copy_from_user(&p, (struct Pos*)arg, sizeof(struct Pos));
	        lcd_set_cursor(p.x, p.y);
      	return 0;
        default :
      return -EINVAL;     /* Invalid argument   */
    }
}
```

Pour permettre l'écriture via la commande `echo`, on doit aussi définir le handler `.write`, qui ne fait simplement qu'écrire dans le lcd.

```c
static ssize_t 
write_lcd_kz(struct file *file, const char *buf, size_t count, loff_t *ppos) {
	unsigned int nblines = (count-1)/20;
	unsigned int nbchar_left = count % 20;
	int i, line;

	// ecrire les lignes n-1 qui sont completes
	for (i = 0, line = 0; line < nblines; ++line)
	{
		lcd_command(LCD_SETDDRAMADDR + lines[line]);
		for (; i < 20; ++i)
		{
			lcd_data(buf[i]);
		}
	}

	// ecrire la ligne restante
	lcd_command(LCD_SETDDRAMADDR + lines[line]);
	for (; i < nbchar_left; ++i)
	{
		lcd_data(buf[i]);
	}

    return count;
}
```
Le code user qu'on a utilisé pour tester notre module est:

```cpp
#define SYMBOL "\xb6\xb2\xd9\xdd & \xd4\xbd\xcf" /* Kailun & Yasma en japonais :) */

int main(int argc, char const *argv[])
{
	struct Pos p;
	p.x = 5;
	p.y = 2;
	int fd = open("/dev/lcd_driverkz",O_RDWR);
	ioctl(fd, LCDIOCT_SETXY, &p);
	write(fd, SYMBOL, strlen(SYMBOL));
	close(fd);
	
	return 0;
}
```
