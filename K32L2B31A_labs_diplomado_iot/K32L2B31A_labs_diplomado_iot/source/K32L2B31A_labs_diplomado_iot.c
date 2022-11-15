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

enum{
	FSM_ESTADO_INICIO=0,
	FSM_ESTADO_ESPERA_NUEVO_DATO_LPUART0,
	FSM_ESTADO_ANALIZA_NUEVO_DATO_LPUART0,
	FSM_ESTADO_ENVIAR_COMANDO_ATI,
	FSM_ESTADO_START_ADC,
	FSM_ESTADO_ESPERA_TIEMPO_RESULTADO,
	FSM_ESTADO_CAPTURA_RESULTADO_ADC,
	FSM_ESTADO_CAPTURA_IMPRIME_RESULTADO_ADC,
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
uint8_t fst_estado_actual=FSM_ESTADO_INICIO;
const char* cmd_at[5]={
		"ATI\r\n",
		"AT+GMI\r\n",
		"AT+GMM\r\n",
		"AT+GMR\r\n",
		"AT+CGMI\r\n",
};

uint32_t msg_size;
iot_nodo_data_t datos_locales;




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
    	switch (fst_estado_actual) {
    			case FSM_ESTADO_INICIO:
    				/*Escribir condiciones iniciales para le ejecución de toda la FSM*/
    				datos_locales.data_sensor_luz_adc = 1;
    				datos_locales.data_sensor_luz_voltaje = 2;
    				datos_locales.data_sensor_luz_lux = 3;
    				datos_locales.data_sensor_temperatura = 4;
    				datos_locales.data_sensor_humedad = 5;
    				datos_locales.data_sensor_presion_atmosferica = 6;

    				fst_estado_actual=FSM_ESTADO_ESPERA_NUEVO_DATO_LPUART0;
    				break;

    			case FSM_ESTADO_ESPERA_NUEVO_DATO_LPUART0:
    				if(leer_bandera_nuevo_dato()!=0){
    					escribir_bandera_nuevo_dato(0);
    					fst_estado_actual=FSM_ESTADO_ANALIZA_NUEVO_DATO_LPUART0;
    				}
    				break;

    			case FSM_ESTADO_ANALIZA_NUEVO_DATO_LPUART0:
    				switch(leer_dato()){
    				case 'l':
    					fst_estado_actual=FSM_ESTADO_START_ADC;
    					break;
    				case 'm':
    					fst_estado_actual=FSM_ESTADO_ENVIAR_COMANDO_ATI;
    					break;
    				case 'b':
    					borrar_buffer();
    					break;
    				default://dato ilegal
    					fst_estado_actual=FSM_ESTADO_INICIO;
    					break;
    				}
    				break;

    			case FSM_ESTADO_ENVIAR_COMANDO_ATI:
    				PRINTF("%s",cmd_at[CMD_AT_ATI_Display_Product_Identification_Information]);
    				fst_estado_actual=FSM_ESTADO_INICIO;
    				break;

    			case FSM_ESTADO_START_ADC:
    				/*Genera señal de START para tomar dato ADC*/
    				ADC16_SetChannelConfig(ADC0_PERIPHERAL, ADC0_CH0_CONTROL_GROUP, &ADC0_channelsConfig[0]);
    				fst_estado_actual=FSM_ESTADO_ESPERA_TIEMPO_RESULTADO;
    				break;

    			case FSM_ESTADO_ESPERA_TIEMPO_RESULTADO:
    				if (0U== (kADC16_ChannelConversionDoneFlag& ADC16_GetChannelStatusFlags(ADC0_PERIPHERAL,ADC0_CH0_CONTROL_GROUP))) {
    					//No ha terminado de realizar la operacion del ADC
    				} else {
    					fst_estado_actual=FSM_ESTADO_CAPTURA_RESULTADO_ADC;
    				}
    				break;

    			case FSM_ESTADO_CAPTURA_RESULTADO_ADC:
    				datos_locales.data_sensor_luz_adc = ADC16_GetChannelConversionValue(ADC0_PERIPHERAL, ADC0_CH0_CONTROL_GROUP);
    				fst_estado_actual=FSM_ESTADO_CAPTURA_IMPRIME_RESULTADO_ADC;
    				break;

    			case FSM_ESTADO_CAPTURA_IMPRIME_RESULTADO_ADC:
    				PRINTF("data_sensor_luz_adc: %d\r\n",datos_locales.data_sensor_luz_adc);
    				fst_estado_actual=FSM_ESTADO_INICIO;
    				break;

    			default:	//estado ilegal
    				fst_estado_actual=FSM_ESTADO_INICIO;
    				break;

        }

        __asm volatile ("nop");
    }
    return 0 ;
}
