/*! @file : leds.h
 * @author  Daniel David Uribe Lambis
 * @version 1.0.0
 * @date    11/11/2022
 * @brief   Driver para
 * @details
 *
*/

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "K32L2B31A.h"
#include "fsl_debug_console.h"
#include "fsl_adc16.h"
#include "fsl_device_registers.h"
#include "fsl_common.h"
#include "fsl_gpio.h"

#include "lpuart0.h"
#include "leds.h"
#include "sens_luz.h"


/*******************************************************************************
 * Definitions
 ******************************************************************************/
enum{
	CMD_AT_ATI_Display_Product_Identification_Information=0,
	CMD_AT_AT_GMI_Request_Manufacturer_Identification,
	CMD_AT_AT_GMM_Request_TA_Model_Identification,
	CMD_AT_AT_GMR_Request_TA_Revision_Identification_of_Software_Release,
	CMD_AT_AT_CGMI_Request_Manufacturer_Identification,
};

typedef struct _iot_nodo_data{
	uint16_t data_sensor_luz_adc;
	uint16_t data_sensor_luz_lux;
	//------------------------------------
	uint8_t data_sensor_luz_voltaje;
	uint8_t data_sensor_temperatura;
	uint16_t data_sensor_humedad;
	//------------------------------------
	uint16_t data_sensor_presion_atmosferica;
	uint16_t reservado;
	//------------------------------------
} iot_nodo_data_t;

typedef struct _sigfox_frame{
	uint32_t preamble;               //4 bytes
	uint16_t frame_synchronization;  //2 bytes
	uint32_t end_point_id;           //4 bytes
	uint8_t payload[12];             // 0 a 12 bytes
	uint16_t aunthenticacion;        //var , length
	uint16_t fcs;                    // 2 bytes
} sigfox_frame_t;

/*******************************************************************************
 * Private Prototypes
 ******************************************************************************/


/*******************************************************************************
 * External vars
 ******************************************************************************/
extern uint8_t dato_lpuart0;
extern uint8_t flag_nuevo_dato_lpuart0;

/*******************************************************************************
 * Local vars
 ******************************************************************************/


volatile static uint8_t i = 0 ;

const char msg1[100]={'h','o','l','a','1',0x00};
const char msg2[]="hola2";
#define MSG3 "hola3"

const char* cmd_at[5]={
		"ATI\r\n",
		"AT+GMI\r\n",
		"AT+GMM\r\n",
		"AT+GMR\r\n",
		"AT+CGMI\r\n",
};

uint32_t msg_size;
iot_nodo_data_t datos_locales;

sigfox_frame_t sigfox_frame;


/*******************************************************************************
 * Private Source Code
 ******************************************************************************/
void ec25_print_data_raw(uint8_t *data_ptr, uint32_t data_size) {
	for (uint32_t i = 0; i < data_size; i++) {
		PRINTF("%c", *(data_ptr + i));
	}
}

void ec25_print_data_ascii_hex(uint8_t *data_ptr, uint32_t data_size) {
	for (uint32_t i = 0; i < data_size; i++) {
		PRINTF("0x%02x ", *(data_ptr + i));
	}
}



int main(void) {

/* Hardware de la placa de inicio. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    /* Init FSL debug console. */
    BOARD_InitDebugConsole();
#endif

    led_off_green();
    led_off_red();

    datos_locales.data_sensor_luz_adc=Sens_dato();
    	datos_locales.data_sensor_luz_voltaje=2;
    	datos_locales.data_sensor_luz_lux=3;
    	datos_locales.data_sensor_temperatura=4;
    	datos_locales.data_sensor_humedad=5;
    	datos_locales.data_sensor_presion_atmosferica=6;



    while(1) {


        i++ ;
        /*if(leer_bandera_nuevo_dato()!=0){
        		escribir_bandera_nuevo_dato(0);
        		ADC16_SetChannelConfig(ADC0_PERIPHERAL, ADC0_CH0_CONTROL_GROUP,	&ADC0_channelsConfig[0]);

        		while (0U == (kADC16_ChannelConversionDoneFlag	& ADC16_GetChannelStatusFlags(ADC0_PERIPHERAL,	ADC0_CH0_CONTROL_GROUP)))
        		{
        			}
        		datos_locales.data_sensor_luz_adc=Sens_dato();

        		sigfox_frame.preamble=0x1234;
        		sigfox_frame.frame_synchronization=0x1234;
        		sigfox_frame.end_point_id=0x1234;
        		sigfox_frame.aunthenticacion=0x1234;
        		sigfox_frame.fcs=0x1234;



        		memcpy(&sigfox_frame.payload[0],(uint8_t *)(&datos_locales),sizeof(datos_locales));

        		ec25_print_data_raw((uint8_t *)(&sigfox_frame),sizeof(sigfox_frame));*/
       		//}



        if(leer_bandera_nuevo_dato()!=0){
        	PRINTF("Valor en ASCII : %d\r\n", leer_dato());
        	escribir_bandera_nuevo_dato(0);
        	if(leer_dato()==82){
        		/* R=82*/
        		PRINTF("LED ROJO SE ENCIENDE");
        		led_on_red();
        	  	}
        	if(leer_dato()==114){
        		/* r=114*/
        		PRINTF("LED ROJO SE APAGA");
   	      		led_off_red();
           	  	}

        	if(leer_dato()==86){
        		/* V=86*/
        		PRINTF("LED VERDE SE ENCIENDE");
  	       		led_on_green();
          	  	}
        	if(leer_dato()==118){
        		/* v=118*/
        		PRINTF("LED VERDE SE APAGA");
   	       		led_off_green();
           	  	}
        	if(leer_dato()==76){
        		/* L=76*/
        	PRINTF("lux: %f\r\n", Sens_dato());
        	   	}

        }

        __asm volatile ("nop");
    }
    return 0 ;
}
