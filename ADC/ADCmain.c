/*
 * main.c - Programa de verificación ADC usando config.h
 *
 * Autor: Carlos Olivera Ylla
 * Fecha: 2025-12-05
 *
 * Descripción:
 *  - Usa config.h / config.c y el driver ADC (adc.h / adc.c).
 *  - Potenciómetro conectado a AN0 (RA0). LEDS en RB0..RB7 muestran los
 *    8 bits más significativos del resultado ADC (12-bit -> usamos bits [11:4]).
 *
 * Requisitos:
 *  - Asegúrate de haber añadido config.h, config.c, adc.h y adc.c al proyecto.
 *  - Ajusta los mapeos de pines si tu encapsulado no dispone de RB0..RB7.
 *
 * Nota:
 *  - config.h define FCY y la macro DELAY_MS(ms) usando libpic30.
 *  - Este main intenta ser robusto frente a distintas definiciones de registros
 *    (AD1PCFGL / AD1PCFG, ANSELAbits, etc.) usando #ifdef.
 */

#include "config.h"
#include "adc.h"
#include <xc.h>
#include <stdint.h>

/* Tiempo entre lecturas (ms) */
#define SAMPLE_PERIOD_MS 50u

static void board_pins_init(void)
{
    /* --- Configurar AN0 (RA0) como analógico --- */
    /* Si existe AD1PCFGL / AD1PCFG -> clear PCFG0 bit para AN0 analog */
    #if defined(AD1PCFGL)
        AD1PCFGL &= ~(1u << 0);    /* PCFG0 = 0 -> AN0 analógico */
    #elif defined(AD1PCFG)
        AD1PCFG &= ~(1u << 0);
    #endif

    /* Si existe ANSELA, marcar ANSA0 como analógico */
    #ifdef ANSELAbits
        ANSELAbits.ANSA0 = 1;
    #endif

    /* Asegurar RA0 como entrada */
    #ifdef TRISAbits
        TRISAbits.TRISA0 = 1;
    #elif defined(TRISA)
        TRISA |= (1u << 0);
    #endif

    /* --- Configurar RB0..RB7 como salidas para LEDs --- */
    #ifdef LATB
        LATB &= 0xFF00; /* limpiar LSB antes de configurar TRISB para evitar glitches */
    #endif

    #ifdef TRISBbits
        TRISBbits.TRISB0 = 0;
        TRISBbits.TRISB1 = 0;
        TRISBbits.TRISB2 = 0;
        TRISBbits.TRISB3 = 0;
        TRISBbits.TRISB4 = 0;
        TRISBbits.TRISB5 = 0;
        TRISBbits.TRISB6 = 0;
        TRISBbits.TRISB7 = 0;
    #elif defined(TRISB)
        /* Poner 8 LSB como salida (0) */
        TRISB &= (uint16_t)~0x00FF;
    #endif
}

/* Función principal */
int main(void)
{
    uint16_t adc_value;
    uint8_t leds;

    /* Inicialización del sistema (puertos, gestión básica) */
    SYSTEM_Initialize();

    /* Inicialización de pines específica para esta aplicación */
    board_pins_init();

    /* Imprimir configuración (si stdout está redirigido a UART) */
    SYSTEM_PrintConfiguration();

    /* Inicializar ADC */
    ADC_Init();

    /* Bucle principal: leer AN0 y mostrar 8 MSB en RB0..RB7 */
    while (1)
    {
        /* Lectura bloqueante en canal AN0 (canal = 0) */
        adc_value = ADC_ReadSingleBlocking(0);

        /* Tomamos bits [11:4] de la lectura 12-bit para obtener 8 niveles */
        leds = (uint8_t)((adc_value >> 4) & 0xFFu);

        /* Escribir en LATB (preservar bits altos si existen) */
        #ifdef LATB
            LATB = (LATB & 0xFF00) | leds;
        #elif defined(PORTB)
            PORTB = (PORTB & 0xFF00) | leds;
        #endif

        /* Esperar antes de la siguiente lectura para que los LEDs sean visibles */
        DELAY_MS(SAMPLE_PERIOD_MS);
    }

    /* no debería llegar aquí */
    return 0;
}