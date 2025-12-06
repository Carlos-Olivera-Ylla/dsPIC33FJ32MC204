/*
 * adc.c
 *
 * Implementación del driver ADC para dsPIC33F (XC16).
 *
 * Uso:
 *  - Configura el pin ANx como analógico en main (ejemplo en main.c).
 *  - Llama a ADC_Init() y luego ADC_ReadSingleBlocking(channel) o la API no bloqueante.
 */

#include "adc.h"
#include <xc.h>    /* registros específicos del dispositivo (XC16) */

/* Ajustes por defecto: puedes cambiarlos según necesidades */
#define ADC_SAMPLE_TIME    4   /* SAMC (Tad cycles) */
#define ADC_ADCS           4   /* ADCS => Tad = (ADCS+1) * Tcy */

/* Internals */
static volatile uint16_t adc_last_result = 0;

/* Inicializa ADC (modo manual: SAMP controla muestreo, luego SAMP=0 lanza conversión) */
void ADC_Init(void)
{
    /* Apagar ADC mientras configuramos */
    #ifdef AD1CON1
    AD1CON1bits.ADON = 0;
    #endif

    /* AD1CON1: FORM=00 Integer, SSRC=000 manual, ASAM=0 */
    #ifdef AD1CON1
    AD1CON1 = 0;
    AD1CON1bits.FORM = 0;   /* Integer */
    AD1CON1bits.SSRC = 0;   /* Conversion triggered by SAMP->0 */
    AD1CON1bits.ASAM = 0;   /* Auto sampling disabled */
    #endif

    /* AD1CON2: SMPI = 0 -> interrupt at every sample (no scanning) */
    #ifdef AD1CON2
    AD1CON2 = 0;
    AD1CON2bits.SMPI = 0;
    #endif

    /* AD1CON3: SAMC, ADCS */
    #ifdef AD1CON3
    AD1CON3 = 0;
    AD1CON3bits.SAMC = ADC_SAMPLE_TIME; /* tiempo de sample en Tad */
    AD1CON3bits.ADCS = ADC_ADCS;        /* Tad = (ADCS+1)*Tcy */
    #endif

    /* AD1CHS0: seleccionar canal por defecto AN0 (si existe) */
    #ifdef AD1CHS0
    AD1CHS0 = 0;
    #endif

    /* Vaciar buffer si existe */
    #ifdef ADC1BUF0
    volatile uint16_t tmp = ADC1BUF0;
    (void)tmp;
    #endif

    /* Encender ADC */
    #ifdef AD1CON1
    AD1CON1bits.ADON = 1;
    #endif
}

/* Inicia muestreo y conversión en el canal 'channel' (ANx). No bloqueante. */
void ADC_StartSingle(uint8_t channel)
{
    /* Selecciona canal positivo (CH0SA) si existe */
    #ifdef AD1CHS0
    AD1CHS0bits.CH0SA = channel; /* asigna AN channel al MUX + */
    #endif

    /* Empezar muestreo */
    #ifdef AD1CON1
    AD1CON1bits.SAMP = 1;
    #endif

    /* pequeña espera para adquisición */
    for (volatile int i = 0; i < 60; ++i) { __builtin_nop(); }

    /* Parar muestreo -> comienza conversión */
    #ifdef AD1CON1
    AD1CON1bits.SAMP = 0;
    #endif
}

/* Espera a que la conversión termine y devuelve el resultado (bloqueante) */
uint16_t ADC_ReadSingleBlocking(uint8_t channel)
{
    ADC_StartSingle(channel);

    /* Esperar DONE */
    #ifdef AD1CON1
    while (!AD1CON1bits.DONE) { /* espera activa */ }
    #endif

    /* Leer buffer */
    uint16_t r = 0;
    #ifdef ADC1BUF0
    r = ADC1BUF0;
    #endif

    adc_last_result = r & ADC_MAX_VALUE;
    return adc_last_result;
}

/* Indica si la conversión actual terminó */
bool ADC_IsConversionDone(void)
{
    #ifdef AD1CON1
    return (bool)AD1CON1bits.DONE;
    #else
    return true;
    #endif
}

/* Devuelve el resultado del último muestreo (raw value) */
uint16_t ADC_GetResult(void)
{
    uint16_t r = 0;
    #ifdef ADC1BUF0
    r = ADC1BUF0;
    #endif
    adc_last_result = r & ADC_MAX_VALUE;
    return adc_last_result;
}