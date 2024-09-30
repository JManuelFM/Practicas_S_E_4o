#include "MKL46Z4.h"

// LED (RG)
// LED_GREEN = PTD5
// LED_RED = PTE29

void delay(void)
{
    volatile int i;

    for (i = 0; i < 1000000; i++);
}

// LED_GREEN = PTD5
void led_green_init()
{
    SIM->SCGC5 |= (1 << 12);      // Habilitar reloxo para o porto D
    PORTD->PCR[5] = 1 << 8;       // Configurar PTD5 como GPIO
    GPIOD->PDDR |= (1 << 5);      // Configurar PTD5 como saída
    GPIOD->PSOR |= (1 << 5);      // Apagar o LED (pón o PTD5 en alto)
}

void led_green_toggle()
{
    GPIOD->PTOR = (1 << 5);  // Alternar o estado do LED verde (PTD5)
}

// LED_RED = PTE29
void led_red_init()
{
    SIM->SCGC5 |= (1 << 13);      // Habilitar reloxo para o porto E
    PORTE->PCR[29] = 1 << 8;      // Configurar PTE29 como GPIO
    GPIOE->PDDR |= (1 << 29);     // Configurar PTE29 como saída
    GPIOE->PSOR |= (1 << 29);     // Apagar o LED (pón o PTE29 en alto)
}

void led_red_toggle()
{
    GPIOE->PTOR = (1 << 29);  // Alternar o estado do LED vermello (PTE29)
}

int main(void)
{
    SIM->COPC = 0;               // Desactivar Watchdog Timer
    led_green_init();
    led_green_toggle();
    led_red_init();

    while (1) {
        led_green_toggle();
        led_red_toggle();
        delay();
    }

    return 0;
}

