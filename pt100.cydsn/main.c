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
#include <stdio.h>

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

 enum {
 PT0,
 REF
};

int main()
{
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    
    sys_init();
    
    /* Enable global interrupts */
    CyGlobalIntEnable;

    /* Start ISRs */
    ADC_IRQ_StartEx(ADC_ISR_Handler);
    
    /* Start ADC conversion */
    ADC_StartConvert();

    
    for(;;)
    {
        float mVpt0 = get_oversample();
        
        float mRpt0 = (mVpt0*(1000000)/(5000-mVpt0)) - 500 ; // res in mohms, tension in mV 
        
        int32 temp_pt0 = RTD_1_GetTemperature((int)mRpt0);

        char toto[1024];        
        sprintf(toto, "mVpt0 %d, resistance %d temperature:%d \r\n",(int)mVpt0, (int)mRpt0, temp_pt0);
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
}



/* [] END OF FILE */
