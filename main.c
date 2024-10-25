#include "MKL46Z4.h"
#include "lcd.h"

// LED (RG)
// LED_GREEN = PTD5 (pin 98)
// LED_RED = PTE29 (pin 26)

// SWICH
// RIGHT (SW1) = PTC3 (pin 73)
// LEFT (SW2) = PTC12 (pin 88)

// Enable IRCLK (Internal Reference Clock)
// see Chapter 24 in MCU doc


unsigned int aciertos = 0;
unsigned int fallos = 0;
unsigned int redOn = 0;
unsigned int greenOn = 0;
unsigned int buttonPulsed = 0;
  
void irclk_ini()
{
  MCG->C1 = MCG_C1_IRCLKEN(1) | MCG_C1_IREFSTEN(1);
  MCG->C2 = MCG_C2_IRCS(0); //0 32KHZ internal reference clock; 1= 4MHz irc
}

void delay(void)
{
  volatile int i;

  for (i = 0; i < 1000000; i++);
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
    
    NVIC_EnableIRQ(PORTC_PORTD_IRQn); //interrupciones del puerto C
    
    PORTC->PCR[3] |= PORT_PCR_IRQC(0xA);
    PORTC->PCR[12] |= PORT_PCR_IRQC(0xA);
}

void led_init()
{
    SIM->SCGC5 |= (1 << 12);      // Habilitar reloxo para o porto D
    SIM->SCGC5 |= (1 << 13);      // Habilitar reloxo para o porto E
    
    PORTD->PCR[5] = 1 << 8;       // Configurar PTD5 como GPIO
    PORTE->PCR[29] = 1 << 8;      // Configurar PTE29 como GPIO
    
    GPIOD->PDDR |= (1 << 5);      // Configurar PTD5 como saída
    GPIOE->PDDR |= (1 << 29);     // Configurar PTE29 como saída
    
    GPIOD->PSOR |= (1 << 5);      // Apagar o LED (pón o PTD5 en alto)
    GPIOE->PSOR |= (1 << 29);     // Apagar o LED (pón o PTE29 en alto)
}

//para las interrupciones
void PORTDIntHandler(void){
    if(PORTC->ISFR & (1<<12)){ //si botón derecho
      if(redOn){
        aciertos++;
      }else{
        fallos++;
      }
      buttonPulsed = 1;
    }else if(PORTC->ISFR & (1<<3)){ //si botón izquierdo
      if(greenOn){
        aciertos++;
      }else{
        fallos++;
      }
      buttonPulsed = 1;
    }
    
    //actualizamos el contador
    lcd_display_time(aciertos, fallos);
    
    //limpiamos los flag para que el interrupt deje de producirse
    PORTC->ISFR |= (1 << 12);
    PORTC->ISFR |= (1 << 3);
}

int main(void)
{
  irclk_ini(); // Enable internal ref clk to use by LCD
  init_buttons();
  led_init();

  lcd_ini();
  lcd_display_time(aciertos, fallos);
  
  SIM->COPC = 0;               // Desactivar Watchdog Timer

  // 'Random' sequence :-)
  volatile unsigned int sequence = 0x32B14D98,
    index = 0;

  while (index < 32) {
    if (sequence & (1 << index)) { //odd
      GPIOD->PCOR |= (1 << 5);      // Encender LED verde
      GPIOE->PSOR |= (1 << 29);     // Apagar LED rojo
      redOn = 0;
      greenOn = 1;
    } else { //even
      GPIOD->PSOR |= (1 << 5);      // Apagar LED verde
      GPIOE->PCOR |= (1 << 29);     // Encender LED rojo
      redOn = 1;
      greenOn = 0;
    }
    
    delay();
    index++;
    
    if(!buttonPulsed){
      fallos++;
      //actualizamos el contador
      lcd_display_time(aciertos, fallos);
    }
    buttonPulsed = 0;
  }
  
  //apagamos ambos LEDs
  GPIOD->PSOR |= (1 << 5);
  GPIOE->PSOR |= (1 << 29);

  while (1) {
    LCD->WF8B[LCD_FRONTPLANE0] = LCD_CLEAR;
    LCD->WF8B[LCD_FRONTPLANE1] = LCD_CLEAR;
    LCD->WF8B[LCD_FRONTPLANE2] = LCD_CLEAR;
    LCD->WF8B[LCD_FRONTPLANE3] = LCD_CLEAR;
    LCD->WF8B[LCD_FRONTPLANE4] = LCD_CLEAR;
    LCD->WF8B[LCD_FRONTPLANE5] = LCD_CLEAR;
    LCD->WF8B[LCD_FRONTPLANE6] = LCD_CLEAR;
    LCD->WF8B[LCD_FRONTPLANE7] = LCD_CLEAR;
    delay();
    lcd_display_time(aciertos, fallos);
    delay();
  }

  return 0;
}
