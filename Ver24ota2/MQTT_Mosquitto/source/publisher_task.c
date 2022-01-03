/******************************************************************************
* File Name:   publisher_task.c
*
* Description: This file contains the task that sets up the user button GPIO 
*              for the publisher and publishes MQTT messages on the topic
*              'MQTT_PUB_TOPIC' to control a device that is actuated by the
*              subscriber task. The file also contains the ISR that notifies
*              the publisher task about the new device state to be published.
*
* Related Document: See README.md
*
*
*******************************************************************************
* Copyright 2020-2021, Cypress Semiconductor Corporation (an Infineon company) or
* an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
*
* This software, including source code, documentation and related
* materials ("Software") is owned by Cypress Semiconductor Corporation
* or one of its affiliates ("Cypress") and is protected by and subject to
* worldwide patent protection (United States and foreign),
* United States copyright laws and international treaty provisions.
* Therefore, you may use this Software only as provided in the license
* agreement accompanying the software package from which you
* obtained this Software ("EULA").
* If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
* non-transferable license to copy, modify, and compile the Software
* source code solely for use in connection with Cypress's
* integrated circuit products.  Any reproduction, modification, translation,
* compilation, or representation of this Software except as specified
* above is prohibited without the express written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
* reserves the right to make changes to the Software without notice. Cypress
* does not assume any liability arising out of the application or use of the
* Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ("High Risk Product"). By
* including Cypress's product in a High Risk Product, the manufacturer
* of such system or application assumes all risk of such use and in doing
* so agrees to indemnify Cypress against all liability.
*******************************************************************************/

#include "cyhal.h"
#include "cybsp.h"
#include "FreeRTOS.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Task header files */
#include "publisher_task.h"
#include "mqtt_task.h"
#include "subscriber_task.h"

/* Configuration file for MQTT client */
#include "mqtt_client_config.h"

/* Middleware libraries */
#include "cy_mqtt_api.h"
#include "cy_retarget_io.h"

#include <inttypes.h>
#include <math.h>

/******************************************************************************
* Macros
******************************************************************************/
/* Interrupt priority for User Button Input. */
#define USER_BTN_INTR_PRIORITY          (5)

/* The maximum number of times each PUBLISH in this example will be retried. */
#define PUBLISH_RETRY_LIMIT             (10)

/* A PUBLISH message is retried if no response is received within this 
 * time (in milliseconds).
 */
#define PUBLISH_RETRY_MS                (1000)

/* Queue length of a message queue that is used to communicate with the 
 * publisher task.
 */
#define PUBLISHER_TASK_QUEUE_LENGTH     (3u)

/******************************************************************************
* Global Variables
*******************************************************************************/
/* FreeRTOS task handle for this task. */
TaskHandle_t publisher_task_handle;

/* Handle of the queue holding the commands for the publisher task */
QueueHandle_t publisher_task_q;

/* Structure to store publish message information. */
cy_mqtt_publish_info_t publish_info =
{
    .qos = (cy_mqtt_qos_t) MQTT_MESSAGES_QOS,
    .topic = MQTT_PUB_TOPIC,
    .topic_len = (sizeof(MQTT_PUB_TOPIC) - 1),
    .retain = false,
    .dup = false
};

/******************************************************************************
* Function Prototypes
*******************************************************************************/
static void publisher_init(void);
static void publisher_deinit(void);
static void isr_button_press(void *callback_arg, cyhal_gpio_event_t event);
void print_heap_usage(char *msg);

/*
 * Onderstaande code hoort bij het uitlezen van analoge sensoren
 */
#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"

/*******************************************************************************
* Macros
*******************************************************************************/
/* Macro for ADC Channel configuration*/
#define SINGLE_CHANNEL 1
#define MULTI_CHANNEL  2

/*
 * Macro to choose between single channel and multiple channel configuration of
 * ADC. Single channel configuration uses channel 0 in single ended mode.
 * Multiple channel configuration uses two channels, channel 0 in single ended
 * mode and channel 1 in differential mode.
 *
 * The default configuration is set to use single channel.
 * To use multiple channel configuration set ADC_EXAMPLE_MODE macro to MULTI_CHANNEL.
 *
 */
#define ADC_EXAMPLE_MODE SINGLE_CHANNEL

/* Channel 0 input pin */
#define VPLUS_CHANNEL_0             (P10_0)

/*******************************************************************************
*       Enumerated Types
*******************************************************************************/
/* ADC Channel constants*/
enum ADC_CHANNELS
{
  CHANNEL_0 = 0,
  CHANNEL_1,
  NUM_CHANNELS
} adc_channel;

/*******************************************************************************
* Function Prototypes
*******************************************************************************/

/* Single channel initialization function*/
void adc_single_channel_init(void);

/* Function to read input voltage from channel 0 */
void adc_single_channel_process(void);

/*******************************************************************************
* Global Variables
*******************************************************************************/
cyhal_lptimer_t lptimer_obj;
cyhal_lptimer_info_t lptimer_obj_info;

/* ADC Object */
cyhal_adc_t adc_obj;

/* ADC Channel 0 Object */
cyhal_adc_channel_t adc_chan_0_obj;

/* Default ADC configuration */
const cyhal_adc_config_t adc_config = {
        .continuous_scanning=false, // Continuous Scanning is disabled
        .average_count=1,           // Average count disabled
        .vref=CYHAL_ADC_REF_VDDA,   // VREF for Single ended channel set to VDDA
        .vneg=CYHAL_ADC_VNEG_VSSA,  // VNEG for Single ended channel set to VSSA
        .resolution = 12u,          // 12-bit resolution
        .ext_vref = NC,             // No connection
        .bypass_pin = NC };       // No connection
/*
 * Bovenstaande code hoort bij het uitlezen van analoge sensoren
 */

#define MCWDT_0_HW MCWDT_STRUCT0

/* HAL Objects */
cyhal_pwm_t pwm;
cyhal_clock_t system_clock;

/******************************************************************************
 * Function Name: publisher_task
 ******************************************************************************
 * Summary:
 *  Task that sets up the user button GPIO for the publisher and publishes 
 *  MQTT messages to the broker. The user button init and deinit operations,
 *  and the MQTT publish operation is performed based on commands sent by other
 *  tasks and callbacks over a message queue.
 *
 * Parameters:
 *  void *pvParameters : Task parameter defined during task creation (unused)
 *
 * Return:
 *  void
 *
 ******************************************************************************/
void publisher_task(void *pvParameters)
{
    /* Status variable */
    cy_rslt_t result;

    publisher_data_t publisher_q_data;

    /* Command to the MQTT client task */
    mqtt_task_cmd_t mqtt_task_cmd;

    /* To avoid compiler warnings */
    (void) pvParameters;

    /* Initialize and set-up the user button GPIO. */
    publisher_init();

    /* Create a message queue to communicate with other tasks and callbacks. */
    publisher_task_q = xQueueCreate(PUBLISHER_TASK_QUEUE_LENGTH, sizeof(publisher_data_t));

    /*
     * Onderstaande code hoort bij het uitlezen van de analoge sensoren
     */
	/* Initialize Channel 0 */
	adc_single_channel_init();

	/* Update ADC configuration */
	result = cyhal_adc_configure(&adc_obj, &adc_config);

	if(result != CY_RSLT_SUCCESS)
	{
		printf("ADC configuration update failed. Error: %ld\n", (long unsigned int)result);
		CY_ASSERT(0);
	}

	/*
	 * Bovenstaande code hoort bij het uitlezen van de analoge sensoren
	*/

	cy_rslt_t result2;
	cy_en_mcwdt_status_t mcwdt_init_status = CY_MCWDT_SUCCESS;
	uint32_t event1_cnt, event2_cnt;
	uint32_t counter1_value, counter0_value;

	/* The time between two presses of switch */
	uint32_t timegap;

	/* Init the system clock (based on FLL) to enable frequency changes later */
	cyhal_clock_get(&system_clock, &CYHAL_CLOCK_FLL);
	cyhal_clock_init(&system_clock);

	/* Initialize event count value */
	event1_cnt = 0;
	event2_cnt = 0;

    while (true)
    {
        /* Wait for commands from other tasks and callbacks. */
        if (pdTRUE == xQueueReceive(publisher_task_q, &publisher_q_data, portMAX_DELAY))
        {
            switch(publisher_q_data.cmd)
            {
                case PUBLISHER_INIT:
                {
                    /* Initialize and set-up the user button GPIO. */
                    publisher_init();
                    break;
                }

                case PUBLISHER_DEINIT:
                {
                    /* Deinit the user button GPIO and corresponding interrupt. */
                    publisher_deinit();
                    break;
                }

                case PUBLISH_MQTT_MSG:
                {
                	int enterReadOut = 0;
                	int flag = 1;
                	while(true) {
                		if(flag == 1){
							event1_cnt = event2_cnt;
							flag = 0;
						}
                		counter0_value = Cy_MCWDT_GetCount(MCWDT_0_HW, CY_MCWDT_COUNTER0);
						counter1_value = Cy_MCWDT_GetCount(MCWDT_0_HW, CY_MCWDT_COUNTER1);
						event2_cnt = ((counter1_value<<16) | (counter0_value<<0));
						/* Calculate the time between two presses of switch and print on the
						 * terminal. MCWDT Counter0 and Counter1 are clocked by LFClk sourced
						 * from WCO of frequency 32768 Hz
						 */
						if(event2_cnt > event1_cnt)
						{
							timegap = (event2_cnt - event1_cnt)/CY_SYSCLK_WCO_FREQ;
						}
						else /* counter overflow */
						{
							timegap = 0;
						}

						if (timegap < 10){
							enterReadOut = 0;
						}
						else if(timegap >= 10){
							enterReadOut = 1;
						}
						if(enterReadOut == 1){
							flag = 1;
							//Temperatuursensor readout
							float adc_result_0 = cyhal_adc_read_uv(&adc_chan_0_obj)/10000;
							printf("Result from sensor %d\n", adc_result_0);

							//Recalculate value to °C
							float value = (adc_result_0/4095.0)*5000;
							float celcius = value/10;
							float farhenheit = (celcius*9)/5 + 32;

							printf("Temperature = %10.10f", celcius);

							// convert 123 to string [buf]
							char snum[5];
							itoa(adc_result_0, snum, 10);


							/* Publish the data received over the message queue. */
							//publish_info.payload = publisher_q_data.data; //normale code van het voorbeeld

							publish_info.payload = snum; //onze sensorwaarde zou toegewezen moeten worden aan de payload

							publish_info.payload_len = strlen(publish_info.payload);

							printf("  Publisher: Publishing '%s' on the topic '%s'\n\n",
								   (char *) publish_info.payload, publish_info.topic);

							result = cy_mqtt_publish(mqtt_connection, &publish_info);

							if (result != CY_RSLT_SUCCESS)
							{
								printf("  Publisher: MQTT Publish failed with error 0x%0X.\n\n", (int)result);

								/* Communicate the publish failure with the the MQTT
								 * client task.
								 */
								mqtt_task_cmd = HANDLE_MQTT_PUBLISH_FAILURE;
								xQueueSend(mqtt_task_q, &mqtt_task_cmd, portMAX_DELAY);
							}

							print_heap_usage("publisher_task: After publishing an MQTT message");
						}

                	}

                }
            }
        }
    }
}

/******************************************************************************
 * Function Name: publisher_init
 ******************************************************************************
 * Summary:
 *  Function that initializes and sets-up the user button GPIO pin along with  
 *  its interrupt.
 * 
 * Parameters:
 *  void
 *
 * Return:
 *  void
 *
 ******************************************************************************/
static void publisher_init(void)
{
    /* Initialize the user button GPIO and register interrupt on falling edge. */
    cyhal_gpio_init(CYBSP_USER_BTN, CYHAL_GPIO_DIR_INPUT,
                    CYHAL_GPIO_DRIVE_PULLUP, CYBSP_BTN_OFF);
    cyhal_gpio_register_callback(CYBSP_USER_BTN, isr_button_press, NULL);
    cyhal_gpio_enable_event(CYBSP_USER_BTN, CYHAL_GPIO_IRQ_FALL,
                            USER_BTN_INTR_PRIORITY, true);
    
    printf("Press the user button (SW2) to publish \"%s\"/\"%s\" on the topic '%s'...\n\n", 
           MQTT_DEVICE_ON_MESSAGE, MQTT_DEVICE_OFF_MESSAGE, publish_info.topic);
}

/******************************************************************************
 * Function Name: publisher_deinit
 ******************************************************************************
 * Summary:
 *  Cleanup function for the publisher task that disables the user button  
 *  interrupt and deinits the user button GPIO pin.
 *
 * Parameters:
 *  void
 *
 * Return:
 *  void
 *
 ******************************************************************************/
static void publisher_deinit(void)
{
    /* Deregister the ISR and disable the interrupt on the user button. */
    cyhal_gpio_register_callback(CYBSP_USER_BTN, NULL, NULL);
    cyhal_gpio_enable_event(CYBSP_USER_BTN, CYHAL_GPIO_IRQ_FALL,
                            USER_BTN_INTR_PRIORITY, false);
    cyhal_gpio_free(CYBSP_USER_BTN);
}

/******************************************************************************
 * Function Name: isr_button_press
 ******************************************************************************
 * Summary:
 *  GPIO interrupt service routine. This function detects button
 *  presses and sends the publish command along with the data to be published 
 *  to the publisher task over a message queue. Based on the current device 
 *  state, the publish data is set so that the device state gets toggled.
 *
 * Parameters:
 *  void *callback_arg : pointer to variable passed to the ISR (unused)
 *  cyhal_gpio_event_t event : GPIO event type (unused)
 *
 * Return:
 *  void
 *
 ******************************************************************************/

static void isr_button_press(void *callback_arg, cyhal_gpio_event_t event)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    publisher_data_t publisher_q_data;

    //To avoid compiler warnings
    (void) callback_arg;
    (void) event;

    //Assign the publish command to be sent to the publisher task.
    publisher_q_data.cmd = PUBLISH_MQTT_MSG;

    //Assign the publish message payload so that the device state toggles.
    if (current_device_state == DEVICE_ON_STATE)
    {
        publisher_q_data.data = (char *)MQTT_DEVICE_OFF_MESSAGE;
    }
    else
    {
        publisher_q_data.data = (char *)MQTT_DEVICE_ON_MESSAGE;
    }

    //Send the command and data to publisher task over the queue
    xQueueSendFromISR(publisher_task_q, &publisher_q_data, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/****************************************************************************************************************************************
 * Hieronder staat de code om een analoge sensor uit te lezen, sommige code staat ook in de publisher task functie.
 ****************************************************************************************************************************************
*/

/*******************************************************************************
 * Function Name: adc_single_channel_init
 *******************************************************************************
 *
 * Summary:
 *  ADC single channel initialization function. This function initializes and
 *  configures channel 0 of ADC.
 *
 * Parameters:
 *  void
 *
 * Return:
 *  void
 *
 *******************************************************************************/
void adc_single_channel_init(void)
{
    /* Variable to capture return value of functions */
    cy_rslt_t result;

    /* Initialize ADC. The ADC block which can connect to pin 10[0] is selected */
    result = cyhal_adc_init(&adc_obj, VPLUS_CHANNEL_0, NULL);
    if(result != CY_RSLT_SUCCESS)
    {
        printf("ADC initialization failed. Error: %ld\n", (long unsigned int)result);
        CY_ASSERT(0);
    }

    /* ADC channel configuration */
    const cyhal_adc_channel_config_t channel_config = {
            .enable_averaging = false,  // Disable averaging for channel
            .min_acquisition_ns = 1000, // Minimum acquisition time set to 1us
            .enabled = true };          // Sample this channel when ADC performs a scan

    /* Initialize a channel 0 and configure it to scan P10_0 in single ended mode. */
    result  = cyhal_adc_channel_init_diff(&adc_chan_0_obj, &adc_obj, VPLUS_CHANNEL_0,
                                          CYHAL_ADC_VNEG, &channel_config);
    if(result != CY_RSLT_SUCCESS)
    {
        printf("ADC single ended channel initialization failed. Error: %ld\n", (long unsigned int)result);
        CY_ASSERT(0);
    }

    printf("ADC is configured in single channel configuration\r\n\n");
    printf("Provide input voltage at pin P10_0. \r\n\n");
}

/*******************************************************************************
 * Function Name: adc_single_channel_process
 *******************************************************************************
 *
 * Summary:
 *  ADC single channel process function. This function reads the input voltage
 *  and prints the input voltage on UART.
 *
 * Parameters:
 *  void
 *
 * Return:
 *  void
 *
 *******************************************************************************/
void adc_single_channel_process(void)
{
    /* Variable to store ADC conversion result from channel 0 */
    int32_t adc_result_0 = 0;

    /* Read input voltage, convert it to millivolts and print input voltage */
    adc_result_0 = cyhal_adc_read_uv(&adc_chan_0_obj)/1000;
    printf("Channel 0 input: %4ldmV\r\n", (long int)adc_result_0);
}


/* [] END OF FILE */
