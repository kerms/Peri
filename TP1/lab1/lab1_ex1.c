/*
 * This file is a template for the first Lab in the SESI-PERI module.
 *
 * It draws inspiration from Dom & Gert code on ELinux.org.
 *
 * Authors:
 *   Julien Peeters <julien.peeters@lip6.fr>
 */

/*
 * Headers that are required for printf and mmap.
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <pthread.h>

/*
 * Base of peripherals and base of GPIO controller.
 */

#define BCM2835_PERIPH_BASE     0x20000000
#define BCM2835_GPIO_BASE       ( BCM2835_PERIPH_BASE + 0x200000 )

/*
 * Paging definitions.
 */

#define RPI_PAGE_SIZE           4096
#define RPI_BLOCK_SIZE          4096

/*
 * Helper macros for accessing GPIO registers.
 */

#define GPIO_CONF_FUNC_REG( ptr, gpio ) \
    *( (ptr) + ( (gpio) / 10 ) ) 

#define GPIO_CONF_FUNC_INPUT_MASK( gpio ) \
    ( ~( 0x7 << ( ( (gpio) % 10 ) * 3 ) ) ) 

#define GPIO_CONF_FUNC_OUTPUT_MASK( gpio ) \
    ( 0x1 << ( ( (gpio) % 10 ) * 3 ) ) 

#define GPIO_CONF_AS_INPUT( ptr, gpio ) \
    GPIO_CONF_FUNC_REG( ptr, gpio ) &= GPIO_CONF_FUNC_INPUT_MASK( gpio ) 

#define GPIO_CONF_AS_OUTPUT( ptr, gpio ) \
    do { \
        GPIO_CONF_AS_INPUT( ptr, gpio ); \
        GPIO_CONF_FUNC_REG( ptr, gpio ) |= \
            GPIO_CONF_FUNC_OUTPUT_MASK( gpio ); \
    } while ( 0 ) 

#define GPIO_CONF_REG( ptr, addr, gpio ) \
    *( (ptr) + ( ( (addr) / sizeof( uint32_t ) ) + ( (gpio) / 32 ) ) ) 

#define GPIO_SET_REG( ptr, gpio ) \
    GPIO_CONF_REG( ptr, 0x1c, gpio )

#define GPIO_CLR_REG( ptr, gpio ) \
    GPIO_CONF_REG( ptr, 0x28, gpio )

#define GPIO_SET( ptr, gpio ) \
    GPIO_SET_REG( ptr, gpio ) = 1 << ( (gpio) % 32 ) 

#define GPIO_CLR( ptr, gpio ) \
    GPIO_CLR_REG( ptr, gpio ) = 1 << ( (gpio) % 32 )

#define GPIO_VALUE( ptr, gpio ) \
    ( ( GPIO_CONF_REG( ptr, 0x34, gpio ) >> ( (gpio) % 32 ) ) & 0x1 ) 

/*
 * Setup the access to memory-mapped I/O.
 */

static int mmap_fd;

int
setup_gpio_mmap ( uint32_t volatile ** ptr )
{
    void * mmap_result;

    mmap_fd = open ( "/dev/mem", O_RDWR | O_SYNC );

    if ( mmap_fd < 0 ) {
        return -1;
    }

    mmap_result = mmap (
        NULL
      , RPI_BLOCK_SIZE
      , PROT_READ | PROT_WRITE
      , MAP_SHARED
      , mmap_fd
      , BCM2835_GPIO_BASE );

    if ( mmap_result == MAP_FAILED ) {
        close ( mmap_fd );
        return -1;
    }

    *ptr = ( uint32_t volatile * ) mmap_result;

    return 0;
}

void
teardown_gpio_mmap ( void * ptr )
{
    munmap ( ptr, RPI_BLOCK_SIZE );
}

void
delay ( unsigned int milisec )
{
    struct timespec ts, dummy;
    ts.tv_sec  = ( time_t ) milisec / 1000;
    ts.tv_nsec = ( long ) ( milisec % 1000 ) * 1000000;
    nanosleep ( &ts, &dummy );
}

#define min( a, b ) ( a < b ? a : b )
#define max( a, b ) ( a < b ? b : a )

/*
 * Main program.
 */



#define GPIO_LED0   4
#define GPIO_LED1   17
#define GPIO_BTN0   18

struct gpio_info
{
    uint32_t volatile * gpio_base;
    int id_gpio;
    int period;
};

void print_info(struct gpio_info * info)
{
    printf("%u\n", *info->gpio_base);
    printf("%d\n", info->id_gpio);
    printf("%d\n", info->period);
}

int button = 0;
int swt = 0;

void* clignote_led(void *arg) {
    struct gpio_info *info = (struct gpio_info *) arg;
    printf("threads led created\n");
    print_info(info);
    
    while (1) {
        GPIO_SET(info->gpio_base, info->id_gpio);
        delay ( info->period + 2*swt*info->period );
        GPIO_CLR (info->gpio_base, info->id_gpio );
        delay ( info->period + 2*swt*info->period );
    }
}




void* listen_button(void *arg) {
    struct gpio_info * info = (struct gpio_info *) arg;
    printf("threads button created\n");
    print_info(info);
    while(1) {
        if ( (button = GPIO_VALUE(info->gpio_base, info->id_gpio) == 0) ) {
            swt = !swt;
            printf("button pressed now swt = %d\n", swt);
            delay(info->period);
        }
    }
}

int
main ( int argc, char **argv )
{
    int                 result;
    int                 period, half_period;
    uint32_t volatile * gpio_base = 0;
    struct gpio_info gled0, gled1, gbutton;
    pthread_t thread_led0, thread_led1, thread_button;

    /* Retreive the mapped GPIO memory. */
    result = setup_gpio_mmap ( &gpio_base );

    if ( result < 0 ) {
        printf ( "-- error: cannot setup mapped GPIO.\n" );
        exit ( 1 );
    }

    period = 500; /* default = 1Hz */
    if ( argc > 1 ) {
        period = atoi ( argv[1] );
    }
    half_period = period / 2;

    /* Setup GPIO of LED0 to output. */
    GPIO_CONF_AS_OUTPUT ( gpio_base, GPIO_LED0 );
    GPIO_CONF_AS_OUTPUT ( gpio_base, GPIO_LED1 );
    GPIO_CONF_AS_INPUT  ( gpio_base, GPIO_BTN0 );

    /* INIT struct */
    gled0.gpio_base = gpio_base;
    gled0.id_gpio = GPIO_LED0;
    gled0.period = half_period;

    gled1.gpio_base = gpio_base;
    gled1.id_gpio = GPIO_LED1;
    gled1.period = half_period/2;
    

    gbutton.gpio_base = gpio_base;
    gbutton.id_gpio = GPIO_BTN0;
    gbutton.period = half_period;


    printf ( "-- info: start blinking.\n" );

    if(pthread_create(&thread_led0, NULL, clignote_led, (void*)&gled0) == -1) {
        perror("pthread_create");
        return EXIT_FAILURE;
    }

    if(pthread_create(&thread_led1, NULL, clignote_led, (void*)&gled1) == -1) {
        perror("pthread_create");
        return EXIT_FAILURE;
    }

    if(pthread_create(&thread_button, NULL, listen_button, (void*)&gbutton) == -1) {
        perror("pthread_create");
        return EXIT_FAILURE;
    }

    /* Blink led at frequency of 1Hz. */


/*
    while (1) {

        GPIO_SET(gpio_base, GPIO_LED0);

        delay ( half_period );
        GPIO_CLR ( gpio_base, GPIO_LED0 );
        GPIO_SET ( gpio_base, GPIO_LED1 );

        delay ( half_period );
        GPIO_SET(gpio_base, GPIO_LED0);

        delay ( half_period );
        GPIO_CLR ( gpio_base, GPIO_LED0 );
        GPIO_CLR ( gpio_base, GPIO_LED1 );

        delay ( half_period );

    }

    */
    sleep(10000000);

    if (pthread_join(thread_led0, NULL)) {
        perror("pthread_create");
        return EXIT_FAILURE;
    }
    pthread_join(thread_led1, NULL);
    pthread_join(thread_button, NULL);


    return 0;
}
