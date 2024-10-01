#include "MKL46Z4.h"

// LED (RG)
// LED_GREEN = PTD5
// LED_RED = PTE29
// BUTTON_LEFT = PTC3
// BUTTON_RIGTH = PTC12

void delay(void)
{
    volatile int i;

    for (i = 0; i < 250000; i++);
}

void init_buttons(){
    SIM->SCGC5 |= (1 << 11);
    
    PORTC->PCR[3] = (1 << 8) //configura el pin como GPIO
                  | (1 << 1) //habilita pull-up
                  | (1 << 0); 
    
    PORTC->PCR[12] = (1 << 8) //configura el pin comoGPIO
                   | (1 << 1) //habilita pull-up
                   | (1 << 0);
                   
    GPIOC->PDDR &= ~(1 << 3);
    GPIOC->PDDR &= ~(1 << 12); //configura ambos como puertos de entrada
}

void state_machine(int state){
    switch(state){
      case 0:
        GPIOE->PSOR |= (1 << 29);
        GPIOD->PSOR |= (1 << 5);
        break;
      case 1:
        GPIOE->PCOR |= (1 << 29);
        GPIOD->PSOR |= (1 << 5);
        break;
      case 2:
        GPIOE->PSOR |= (1 << 29);
        GPIOD->PCOR |= (1 << 5);
        break;
      case 3:
        GPIOE->PCOR |= (1 << 29);
        GPIOD->PCOR |= (1 << 5);
        break;
    }
}

int right_button(){
    if(!(GPIOC->PDIR & (1 << 3))){
    	delay();
        return !(GPIOC->PDIR & (1 << 3));  // Lee el estado del pin 3 de GPIOC
    }
    return 0;
}

int left_button(){
    if(!(GPIOC->PDIR & (1 << 12))){
    	delay();
        return !(GPIOC->PDIR & (1 << 12));  // Lee el estado del pin 12 de GPIOC
    }
    return 0;
}

// LED_GREEN = PTD5
void led_green_init()
{
    SIM->SCGC5 |= (1 << 12);      // Habilitar reloxo para o porto D
    PORTD->PCR[5] = 1 << 8;       // Configurar PTD5 como GPIO
    GPIOD->PDDR |= (1 << 5);      // Configurar PTD5 como saída
    GPIOD->PSOR |= (1 << 5);      // Apagar o LED (pón o PTD5 en alto)
}

// LED_RED = PTE29
void led_red_init()
{
    SIM->SCGC5 |= (1 << 13);      // Habilitar reloxo para o porto E
    PORTE->PCR[29] = 1 << 8;      // Configurar PTE29 como GPIO
    GPIOE->PDDR |= (1 << 29);     // Configurar PTE29 como saída
    GPIOE->PSOR |= (1 << 29);     // Apagar o LED (pón o PTE29 en alto)
}

int main(void)
{
    SIM->COPC = 0;               // Desactivar Watchdog Timer
    int state = 0;
    led_green_init();
    led_red_init();
    init_buttons();

    while (1) {
    	int right = right_button();
    	int left = left_button();
        if(left){
           state = state+1;
           state = state%4;
        }else if(right){
           state = state-1;
           if(state<0)
              state = 3;
        }
        state_machine(state);
    }

    return 0;
}

