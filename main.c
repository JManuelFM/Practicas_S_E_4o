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

  
unsigned int aa = 0;
unsigned int ss = 0;

unsigned int aSet = 0;
unsigned int sSet = 0;

unsigned int stopTimer = 0;
  
void irclk_ini()
{
  MCG->C1 = MCG_C1_IRCLKEN(1) | MCG_C1_IREFSTEN(1);
  MCG->C2 = MCG_C2_IRCS(0); //0 32KHZ internal reference clock; 1= 4MHz irc
}

void delay(void)
{
  volatile int i;

  for (i = 0; i < 500000; i++);
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

void LPTMR_Init() {
    SIM->SCGC5 |= SIM_SCGC5_LPTMR_MASK;  // Habilitar reloj para LPTMR

    LPTMR0->CSR = 0;  // Desactivar temporizador antes de configurarlo
    
    LPTMR0->PSR = LPTMR_PSR_PCS(1) | LPTMR_PSR_PBYP_MASK;  // Usar reloj LPO (1 kHz), sin divisor
    LPTMR0->CMR = 1000;  // 1000 ticks = 1 segundo

    NVIC_EnableIRQ(LPTMR0_IRQn);  // Activar interrupción para LPTMR
    NVIC_SetPriority(LPTMR0_IRQn, 2);  // Prioridad baja
    
    LPTMR0->CSR = LPTMR_CSR_TIE_MASK | LPTMR_CSR_TEN_MASK;  // Activar con interrupción
}

void end_count(){
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
    lcd_display_time(aa, ss);
    delay();
  }
}

void alarm(){
  while(ss > 0){
    GPIOD->PCOR |= (1 << 5);
    GPIOE->PCOR |= (1 << 29);
    
    delay();
    
    GPIOD->PSOR |= (1 << 5);
    GPIOE->PSOR |= (1 << 29);
    
    delay();
  }
  
  GPIOD->PSOR |= (1 << 5);
  GPIOE->PSOR |= (1 << 29);
}

void setA(){
  aSet = 0;
  GPIOE->PCOR |= (1 << 29);
  while(!aSet){
    lcd_display_time(aa, ss);
  }
  GPIOE->PSOR |= (1 << 29);
}

void setS(){
  sSet = 0;
  GPIOD->PCOR |= (1 << 5);
  while(!sSet){
    lcd_display_time(aa, ss);
  }
  GPIOD->PSOR |= (1 << 5);
}

//para las interrupciones por botones
void PORTDIntHandler(void){
    if(PORTC->ISFR & (1<<12)){ //botón derecho para cambiar ss/aa y para parar/reanudar timer
      if(!sSet){
        ss++;
        ss = ss%100;
      }
      if(sSet && !aSet){
        aa++;
        aa = aa%100;
      }
      if(sSet && aSet){
        stopTimer = !stopTimer;
      }
    }else if(PORTC->ISFR & (1<<3)){ //botón izquierdo para confirmar
      if(!sSet){
        sSet = 1;
      }
      if(sSet && !aSet){
        aSet = 1;
      }
    }
    
    //limpiamos los flag para que el interrupt deje de producirse
    PORTC->ISFR |= (1 << 12);
    PORTC->ISFR |= (1 << 3);
}

void LPTMRIntHandler(void) {
    if (LPTMR0->CSR & LPTMR_CSR_TCF_MASK) {
        if (!stopTimer && ss > 0) {
            ss--;  // Decrementar el contador de segundos

            // Actualizar el LCD
            lcd_display_time(aa, ss);
        }
        LPTMR0->CSR |= LPTMR_CSR_TCF_MASK;
    }
}

int main(void)
{
  SIM->COPC = 0;      // Desactivar Watchdog Timer

  lcd_ini();
  
  stopTimer = 0;
  
  irclk_ini(); // Enable internal ref clk to use by LCD
  init_buttons();
  led_init();
  
  setS();
  
  setA();
  
  LPTMR_Init();
  
  while (1) {
    if (ss == aa) {
      alarm();
      end_count();
    }
  }

  return 0;
}
