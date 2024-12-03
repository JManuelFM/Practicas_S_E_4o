#include "MKL46Z4.h"
#include "FreeRTOS.h"
#include "task.h"
#include "lcd.h"
#include "queue.h"
  

volatile uint8_t creadores = 0;
volatile uint8_t consumidores = 0;
volatile uint8_t datos_pendentes = 0;

QueueHandle_t cola;

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

void taskDisplayData(void *pvParameters){
	int dataleft, creators, consumers;
	for(;;){
	  taskENTER_CRITICAL();
	  dataleft = datos_pendentes;
	  creators = creadores;
	  consumers = consumidores;
	  
	  lcd_display_time(dataleft, creators * 10 + consumers);
	  taskEXIT_CRITICAL();
	  vTaskDelay(100/portTICK_RATE_MS);
	}
}

void taskCreate1(void *pvParameters){
	for(;;){
	  //crea datos
	  if (creadores > 0){
	    uint32_t dato = 1; // Dato a enviar
	    if (uxQueueSpacesAvailable(cola) > 0) { // Verifica si hay espacio en la cola
	      if (xQueueSend(cola, &dato, 0) == pdTRUE) { // Envía el dato sin bloquear
        	datos_pendentes++;
    	      }
	    }
	  }
	  vTaskDelay(100/portTICK_RATE_MS);
	}
}

void taskCreate2(void *pvParameters){
	for(;;){
	  //crea datos
	  if (creadores > 1){
	    uint32_t dato = 2; // Dato a enviar
	    if (uxQueueSpacesAvailable(cola) > 0) { // Verifica si hay espacio en la cola
	      if (xQueueSend(cola, &dato, 0) == pdTRUE) { // Envía el dato sin bloquear
        	datos_pendentes++;
    	      }
	    }
	  }
	  vTaskDelay(100/portTICK_RATE_MS);
	}
}

void taskCreate3(void *pvParameters){
	for(;;){
	  //crea datos
	  if (creadores > 2){
	    uint32_t dato = 3; // Dato a enviar
	    if (uxQueueSpacesAvailable(cola) > 0) { // Verifica si hay espacio en la cola
	      if (xQueueSend(cola, &dato, 0) == pdTRUE) { // Envía el dato sin bloquear
        	datos_pendentes++;
    	      }
	    }
	  }
	  vTaskDelay(100/portTICK_RATE_MS);
	}
}

void taskConsume1(void *pvParameters){
	for(;;){
	  //consume datos
	  if (consumidores > 0){
	    uint32_t datoRecibido;
	    if (uxQueueMessagesWaiting(cola) > 0) { // Verifica si hay datos en la cola
    	      if (xQueueReceive(cola, &datoRecibido, 0) == pdTRUE) { // Recibe el dato sin bloquear
        	datos_pendentes--;
    	      }
	    }
	  }
	  vTaskDelay(100/portTICK_RATE_MS);
	}
}

void taskConsume2(void *pvParameters){
	for(;;){
	  //consume datos
	  if (consumidores > 1){
	    uint32_t datoRecibido;
	    if (uxQueueMessagesWaiting(cola) > 0) { // Verifica si hay datos en la cola
    	      if (xQueueReceive(cola, &datoRecibido, 0) == pdTRUE) { // Recibe el dato sin bloquear
        	datos_pendentes--;
    	      }
	    }
	  }
	  vTaskDelay(100/portTICK_RATE_MS);
	}
}

void taskConsume3(void *pvParameters){
	for(;;){
	  //consume datos
	  if (consumidores > 2){
	    uint32_t datoRecibido;
	    if (uxQueueMessagesWaiting(cola) > 0) { // Verifica si hay datos en la cola
    	      if (xQueueReceive(cola, &datoRecibido, 0) == pdTRUE) { // Recibe el dato sin bloquear
        	datos_pendentes--;
    	      }
	    }
	  }
	  vTaskDelay(100/portTICK_RATE_MS);
	}
}

void PORTDIntHandler(void){
    if(PORTC->ISFR & (1<<12)){ //si botón derecho
      consumidores++;
      if(consumidores == 4) consumidores = 0;
      PORTC->ISFR |= (1 << 12);
    }else if(PORTC->ISFR & (1<<3)){ //si botón izquierdo
      creadores++;
      if(creadores == 4) creadores = 0;
      PORTC->ISFR |= (1 << 3);
    }
}

int main(void)
{
	init_buttons();
	
	irclk_ini();
	
	lcd_ini();
	
	SIM->COPC = 0;
	
	cola = xQueueCreate(99, sizeof(uint32_t));
	
        if (cola == NULL) {
          // Error al crear la cola
          while (1);
        }
		
	xTaskCreate(taskDisplayData, (signed char *)"TaskDisplayData", 
		configMINIMAL_STACK_SIZE, (void *)NULL, 1, NULL);
		
	xTaskCreate(taskCreate1, (signed char *)"TaskCreate1", 
		configMINIMAL_STACK_SIZE, (void *)NULL, 1, NULL);
		
	xTaskCreate(taskCreate2, (signed char *)"TaskCreate2", 
		configMINIMAL_STACK_SIZE, (void *)NULL, 1, NULL);
		
	xTaskCreate(taskCreate3, (signed char *)"TaskCreate3", 
		configMINIMAL_STACK_SIZE, (void *)NULL, 1, NULL);
		
	xTaskCreate(taskConsume1, (signed char *)"TaskConsume1", 
		configMINIMAL_STACK_SIZE, (void *)NULL, 1, NULL);
		
	xTaskCreate(taskConsume2, (signed char *)"TaskConsume2", 
		configMINIMAL_STACK_SIZE, (void *)NULL, 1, NULL);
		
	xTaskCreate(taskConsume3, (signed char *)"TaskConsume3", 
		configMINIMAL_STACK_SIZE, (void *)NULL, 1, NULL);
	
	/* start the scheduler */
	vTaskStartScheduler();

	/* should never reach here! */
	for (;;);

	return 0;
}
