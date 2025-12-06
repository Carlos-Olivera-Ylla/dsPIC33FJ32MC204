/*
 * adc.h
 *
 * Driver simple de ADC para dsPIC33F con XC16
 *
 * API:
 *   void ADC_Init(void);
 *   uint16_t ADC_ReadSingleBlocking(uint8_t channel); // bloqueante, devuelve resultado (0..4095 si 12-bit)
 *   void ADC_StartSingle(uint8_t channel);           // inicia conversión (no bloqueante)
 *   bool ADC_IsConversionDone(void);                 // comprueba si terminó
 *   uint16_t ADC_GetResult(void);                    // devuelve resultado del último muestreo
 *
 * Nota:
 * - Configura los pines como analógicos (ANx) en tu main antes de usar el ADC.
 * - Ajusta AD1CON3.ADCS y AD1CON3.SAMC en adc.c para tiempos de adquisición/Tad
 */

#ifndef ADC_H
#define ADC_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void ADC_Init(void);

/* Lee un canal (ANx) de forma bloqueante. 'channel' es el número AN (e.g., 0 para AN0, 1 para AN1, ...). */
uint16_t ADC_ReadSingleBlocking(uint8_t channel);

/* API no bloqueante */
void ADC_StartSingle(uint8_t channel); /* inicia muestreo/conversión en channel */
bool ADC_IsConversionDone(void);       /* true si DONE */
uint16_t ADC_GetResult(void);          /* resultado del último muestreo (raw) */

/* Resolución en bits (definir según AD1CON1.FORM y AD1CON3 configuración) */
#define ADC_RESOLUTION_BITS 12u
#define ADC_MAX_VALUE       ((1u << ADC_RESOLUTION_BITS) - 1u)

#ifdef __cplusplus
}
#endif

#endif /* ADC_H */