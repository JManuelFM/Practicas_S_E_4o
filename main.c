#include "MKL46Z4.h"
#include "FreeRTOS.h"
#include "task.h"
#include "lcd.h"

  

volatile uint8_t creadores = 0;
volatile uint8_t consumidores = 0;
volatile uint8_t datos_pendentes = 0;

void delay(void)
{
  volatile int i;

  for (i = 0; i < 1000000; i++);
}

void irclk_ini()
{
  MCG->C1 = MCG_C1_IRCLKEN(1) | MCG_C1_IREFSTEN(1);
  MCG->C2 = MCG_C2_IRCS(0); //0 32KHZ internal reference clock; 1= 4MHz irc
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

void led_green_init()
{
	SIM_COPC = 0;
	SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK;
	PORTD_PCR5 = PORT_PCR_MUX(1);
	GPIOD_PDDR |= (1 << 5);
	GPIOD_PSOR = (1 << 5);
}

void led_green_toggle()
{
	GPIOD_PTOR = (1 << 5);
}

void led_red_init()
{
	SIM_COPC = 0;
	SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK;
	PORTE_PCR29 = PORT_PCR_MUX(1);
	GPIOE_PDDR |= (1 << 29);
	GPIOE_PSOR = (1 << 29);
}

void led_red_toggle(void)
{
	GPIOE_PTOR = (1 << 29);
}

void taskLedGreen(void *pvParameters)
{
    for (;;) {
        led_green_toggle();
        vTaskDelay(200/portTICK_RATE_MS);
    }
}

void taskLedRed(void *pvParameters)
{
    for (;;) {
        led_red_toggle();
        vTaskDelay(500/portTICK_RATE_MS);
    }
}

void taskDisplayData(void *pvParameters){
	for(;;){
	  lcd_display_dec(datos_pendentes * 100 + creadores * 10 + consumidores);
	  vTaskDelay(200/portTICK_RATE_MS);
	}
}

void PORTC_PORTD_IRQHandler(void){
    if(PORTC->ISFR & (1<<12)){ //si botón derecho
      consumidores = (consumidores+1)%4;
      PORTC->ISFR |= (1 << 12);
    }else if(PORTC->ISFR & (1<<3)){ //si botón izquierdo
      creadores = (creadores+1)%4;
      PORTC->ISFR |= (1 << 3);
    }
}

int main(void)
{
	led_green_init();
	led_red_init();
	init_buttons();
	
	irclk_ini();
	
	lcd_ini();

	/* create green led task */
	xTaskCreate(taskLedGreen, (signed char *)"TaskLedGreen", 
		configMINIMAL_STACK_SIZE, (void *)NULL, 1, NULL);

	/* create red led task */
	xTaskCreate(taskLedRed, (signed char *)"TaskLedRed", 
		configMINIMAL_STACK_SIZE, (void *)NULL, 1, NULL);
		
	xTaskCreate(taskDisplayData, (signed char *)"TaskDisplayData", 
		configMINIMAL_STACK_SIZE, (void *)NULL, 1, NULL);
	
	/* start the scheduler */
	vTaskStartScheduler();

	/* should never reach here! */
	for (;;);

	return 0;
}
