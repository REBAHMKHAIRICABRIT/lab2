#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "esp_log.h"

/* FreeRTOS.org includes. */
#include "freertos/FreeRTOSConfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "my_helper_fct.h"

static const uint32_t STACK_SIZE = 4000;


static const uint32_t T1_PRIO = 2;
static const uint32_t T2_PRIO = 2;
static const uint32_t T3_PRIO = 3;


void vTaskFunction_1(void *pvParameters);
void vTaskFunction_2(void *pvParameters);
void vTaskFunction_3(void *pvParameters);

uint32_t msg_to_send = 0;


/**
 * @brief Starting point function
 *
 */

QueueHandle_t xQueue_1 =  NULL;


void app_main(void)
{


      DISPLAY(" Creating the queue");
      xQueue_1 = xQueueCreate( 5, sizeof(uint32_t));


      DISPLAY("Creating the task");
      xTaskCreatePinnedToCore(vTaskFunction_1, "Task 1", STACK_SIZE, (void*)"Task 1", T1_PRIO, NULL,CORE_1);                              
      xTaskCreatePinnedToCore(vTaskFunction_2, "Task 2", STACK_SIZE, (void*)"Task 2", T2_PRIO, NULL,CORE_0);
      xTaskCreatePinnedToCore(vTaskFunction_3, "Task 3", STACK_SIZE, (void*)"Task 3", T3_PRIO, NULL,CORE_0);

      vTaskDelete(NULL);      
}


void vTaskFunction_1(void *pvParameters) {
      TickType_t xLastWakeTime = xTaskGetTickCount();
      DISPLAY("task 1 : Start");
      // Check result

      for(;;){
            if(xQueue_1 != 0){
                  int result =  xQueueSend(xQueue_1,(void *) &msg_to_send, 0);
                  if ( result != pdPASS ){
                        DISPLAY(" task1 : Error while sending value %d to queue ",(int)msg_to_send);
                  }
                  else{
                        DISPLAY("task 1 : Value sended %d to the queue",(int) msg_to_send);
                  }
                  msg_to_send ++;
            }
            else{
                  DISPLAY("Queue 1 not created");
            }
            // Compute time : 40 ms
            COMPUTE_IN_TIME_MS (40) ;
            // block periodically : 500 ms
            vTaskDelayUntil (&xLastWakeTime, pdMS_TO_TICKS (500)) ;
            DISPLAY("task 1 : End");
            }
      vTaskDelete(NULL);      
}


void vTaskFunction_2(void *pvParameters) {
      DISPLAY("task 2 : Start");
      uint32_t *receivedMessage = NULL;
    static const char* TAG = "msgTimeOut"
      // Wait for message
      for(;;){
            if(xQueueReceive(xQueue_1, &receivedMessage, pdTICKS_TO_MS(30)) == pdPASS){
                  DISPLAYI(LOG_COLOR_GREEN,"Task 2 : Received %d from the queue",(int)receivedMessage);
            }
            else{
                  DISPLAYE(LOG_COLOR_RED,"Task 2 : Problem while receiveing character, Timeout expired");
            }
            // Compute time :  30 ms
        COMPUTE_IN_TIME_MS (30) ;
            DISPLAY("task 2 : End");

      }
      vTaskDelete(NULL);      
}

void vTaskFunction_3(void *pvParameters) {

      for(;;){
            // Task blocked during 100 ms
            DISPLAY ("Start of task 3") ;
            vTaskDelay ( pdMS_TO_TICKS (100) ) ;
            DISPLAY ("Task 3 start computing") ;
            // Compute time : 2 ticks or 20 ms
            COMPUTE_IN_TIME_MS (20) ;
      }

      vTaskDelete(NULL);      
}