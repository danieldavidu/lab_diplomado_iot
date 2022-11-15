/*! @file : sens_luz.c
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
#include "sens_luz.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/


/*******************************************************************************
 * Private Prototypes
 ******************************************************************************/


/*******************************************************************************
 * External vars
 ******************************************************************************/


/*******************************************************************************
 * Local vars
 ******************************************************************************/


/*******************************************************************************
 * Private Source Code
 ******************************************************************************/
void Sens_Inicia(void);
void Sens_Espera_Result(void);

void Sens_Inicia(void){
	ADC16_SetChannelConfig(ADC0_PERIPHERAL, ADC0_CH0_CONTROL_GROUP, &ADC0_channelsConfig[0]);

}
void Sens_Espera_Result(void){

	 while (0U == (kADC16_ChannelConversionDoneFlag & ADC16_GetChannelStatusFlags(ADC0_PERIPHERAL, ADC0_CH0_CONTROL_GROUP)))
	              {
	    	      }

}


float Sens_dato(void){
	float voltaje;
	float corriente;
	float lux;
	float K1;
	float K2;
	float K3;
	uint16_t dato_adc;

	Sens_Inicia();
	Sens_Espera_Result();

	dato_adc= ADC16_GetChannelConversionValue(ADC0_PERIPHERAL, ADC0_CH0_CONTROL_GROUP);
	    	     dato_adc=4095-dato_adc;
	    	     K1=3.3/4095;
	    	     voltaje=dato_adc*K1;
	    	     K2=0.1e-3;
	    	     corriente=voltaje*K2;
	    	     K3=(10000/0.33e-3);
	    	     lux=corriente*K3;

	    	     /*captura los datos e imprime por consola* /
	    	                    SysTick_DelayTicks(200U);
	    	            	    PRINTF("ADC Value: %d\r\n", dato_adc);
	    	            	    PRINTF("voltaje: %f\r\n",voltaje);
	    	            	    PRINTF("corriente: %f\r\n",corriente);
	    	            	    PRINTF("LUX: %f\r\n",lux);
	    	            	    PRINTF("-------------------------------------\r\n",lux);*/

	return lux;

}

/*******************************************************************************
 * Public Source Code
 ******************************************************************************/
 
