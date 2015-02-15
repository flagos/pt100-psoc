/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include <project.h>
//#include <device.h>
#include <stdio.h>
#include "reglin.h"

/* Macro definitions */
#define LOW                 (0u)
#define HIGH                (1u)
#define CHANNEL_1           (0u)
#define CLEAR_SCREEN        (0x0C)
#define CONVERT_TO_ASCII    (0x30u)
#define SAMPLE_NUMBER       1000

static void sys_init();
static float get_oversample();

/* Interrupt prototypes */
CY_ISR_PROTO(ADC_ISR_Handler);

/* Global variables */
volatile uint8  dataReady     = 0u;
volatile uint32 windowFlag    = 0u;
volatile uint8  channelFlag   = 0u;


int main()
{
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    /* Start the Components */
    int32 mVolts;
    float uA_current_sampling_points[] = {250, 500};
    float uVolts_sampling_values[sizeof(uA_current_sampling_points)/sizeof(uA_current_sampling_points[0])];
    
    sys_init();
    
    /* Start ISRs */
    ADC_IRQ_StartEx(ADC_ISR_Handler);
    
    /* Enable global interrupts */
    CyGlobalIntEnable;

    /* Start ADC conversion */
    ADC_StartConvert();
    /* CyGlobalIntEnable; */ /* Uncomment this line to enable global interrupts. */
    for(;;)
    {
        int i=0;
        int nb_regression_sample = sizeof(uA_current_sampling_points)/sizeof(uA_current_sampling_points[0]);
        for (i=0; i < nb_regression_sample; i++){
            uA_current_sampling_points[i] = (((int)uA_current_sampling_points[i]) / 2.4)*2.4; // resolution idac
            Current_driver_SetValue(((int)uA_current_sampling_points[i]) / 2.4);
            uVolts_sampling_values[i] = get_oversample()*1000;
        }
        
        float result[3];
        // reglin(uA_current_sampling_points, uVolts_sampling_values, nb_regression_sample, result);
        float mResistance = result[0]*1000; // resistance in milli ohms
        mResistance = (uVolts_sampling_values[1]-uVolts_sampling_values[0])/(uA_current_sampling_points[1]-uA_current_sampling_points[0]) * 1000;
                
        int32 temp = RTD_1_GetTemperature((int)mResistance);

        char toto[1024];        
        sprintf(toto, "current[0]: %d uA current[1]:%d uA tension[0]:%d uV tension[1]=%d uV resistance %d mOhms temperature %ld cC\r\n", 
                (int)uA_current_sampling_points[0],
                (int)uA_current_sampling_points[1],
                (int)uVolts_sampling_values[0], 
                (int)uVolts_sampling_values[1], 
                (int)mResistance, 
                temp);

        UART_UartPutString(toto);
        CyDelay(200);
    }
}



/*******************************************************************************
* Function Name: get_oversample
********************************************************************************
*
* Summary:
*  Performs the following tasks:
*   - Converts the channel number to ASCII character
*   - Clears terminal screen
*   - Sends the channel number and voltage to UART
*
* Parameters:
*  None   
*
* Return:
*  uint32: ADC value in mV read with SAMPLE_NUMBER samplings
*
********************************************************************************/
static float get_oversample() {
    uint8 channel = CHANNEL_1;
    int16 adcVal[4u];
    float mVolts = 0;
    int i;
    for (i=0; i<SAMPLE_NUMBER; i++) {
        /* Place your application code here. */
        while(dataReady == 0u)
        {
            ; /* Wait for ADC conversion */
        }
            /* Buffer the results */
        adcVal[CHANNEL_1] = ADC_GetResult16(CHANNEL_1);
        mVolts += ADC_CountsTo_mVolts(CHANNEL_1, adcVal[CHANNEL_1]);
        dataReady = 0u;
    }
    mVolts /= SAMPLE_NUMBER;
    return mVolts;
}





/******************************************************************************
* Function Name: ADC_ISR_Handler
*******************************************************************************
*
* Summary:
*  Interrupt Service Routine. Check the ADC status and sets window and data
*  ready flags.
*
******************************************************************************/
CY_ISR(ADC_ISR_Handler)
{
    uint32 intr_status;

    /* Read interrupt status registers */
    intr_status = ADC_SAR_INTR_MASKED_REG;
    /* Check for End of Scan interrupt */
    if((intr_status & ADC_EOS_MASK) != 0u)
    {
        /* Read range interrupt status and raise the flag */
        windowFlag = ADC_SAR_RANGE_INTR_MASKED_REG;
        /* Clear range detect status */
        ADC_SAR_RANGE_INTR_REG = windowFlag;
        dataReady = 1u;
    }
    /* Clear handled interrupt */
    ADC_SAR_INTR_REG = intr_status;
}


static void sys_init() 
{
    UART_Start();
    ADC_Start();   
    Current_driver_Start();
}



/* [] END OF FILE */
