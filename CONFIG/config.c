/*
 * config.c - Implementación de funciones de sistema para dsPIC33FJ32MC204
 *
 * Autor: Carlos Olivera Ylla
 * Fecha: 2025-12-05
 *
 * Descripción:
 *  Implementa las funciones declaradas en config.h:
 *    SYSTEM_Initialize, SYSTEM_Deinitialize, SYSTEM_EnterSleep,
 *    SYSTEM_Wakeup, SYSTEM_Reset, SYSTEM_EnableInterrupts,
 *    SYSTEM_DisableInterrupts, SYSTEM_GetClockFrequency,
 *    SYSTEM_GetState, SYSTEM_PrintConfiguration
 *
 * Nota importante:
 *  - Este archivo proporciona implementaciones seguras y portables (stubs)
 *    y acciones de inicialización de puertos básicas. En las funciones
 *    SYSTEM_EnterSleep() y SYSTEM_Reset() se muestran las opciones para
 *    ejecutar la instrucción de bajo consumo / reset real del dispositivo,
 *    pero están comentadas para evitar comportamientos imprevisibles si el
 *    proyecto no espera un reset o sleep inmediato.
 *
 *  - Ajusta/activar las líneas comentadas si conoces bien tu configuración
 *    de hardware y quieres que estas funciones hagan la operación hardware
 *    real (por ejemplo usar __builtin_write_OSCCONL para reset).
 *
 */

#include "config.h"
#include <xc.h>
#include <stdint.h>
#include <stdio.h>  /* usado opcionalmente por SYSTEM_PrintConfiguration */

/* Estado interno del sistema */
static volatile System_State_t system_state = SYS_STATE_INIT;

/* ------------------------------------------------------------------------- */
/* Helper: inicializa puertos según macros de config.h                         */
/* ------------------------------------------------------------------------- */
static void ports_init(void)
{
    /* Inicializa LATx antes de configurar TRISx para evitar glitches */
    #ifdef LATB
    LATB = 0x0000;
    #endif

    /* PORTB: si está habilitado, ponemos RB0..RB7 como salidas para leds, si
       no, dejamos los pines como entradas para ahorro (valor por defecto). */
    #ifdef CONFIG_PORT_B_ENABLED
        #ifdef TRISBbits
            /* Si existen bits individuales, los definimos explícitamente */
            /* Cuidado: modifica según qué pines estén disponibles en tu encapsulado */
            TRISBbits.TRISB0 = 0;
            TRISBbits.TRISB1 = 0;
            TRISBbits.TRISB2 = 0;
            TRISBbits.TRISB3 = 0;
            TRISBbits.TRISB4 = 0;
            TRISBbits.TRISB5 = 0;
            TRISBbits.TRISB6 = 0;
            TRISBbits.TRISB7 = 0;
        #else
            /* Si no hay bits individuales, configurar máscara: 0 -> output */
            TRISB &= (uint16_t)~0x00FF;
        #endif
    #else
        /* Puerto B no habilitado: poner pines como entradas (alta impedancia) */
        #ifdef TRISB
            TRISB |= 0x00FF; /* LSB como entradas */
        #endif
    #endif
}

/* ------------------------------------------------------------------------- */
/* Implementaciones públicas                                                  */
/* ------------------------------------------------------------------------- */

void SYSTEM_Initialize(void)
{
    /* Bloquear interrupciones mientras configuramos */
    SYSTEM_DisableInterrupts();

    /* Inicializaciones de puertos y periféricos dependientes de config.h */
    ports_init();

    /* Inicialización adicional (timers, ADC, UART...) puede hacerse desde
       otros módulos que llamen a sus init específicos. */

    /* Estado listo */
    system_state = SYS_STATE_READY;

    /* Restaurar interrupciones si las teníamos activas */
    SYSTEM_EnableInterrupts();
}

void SYSTEM_Deinitialize(void)
{
    /* Poner el sistema en estado inicial / apagar periféricos si es necesario */
    /* Ejemplo: poner puertos en entradas para ahorro */
    #ifdef TRISB
    TRISB |= 0x00FF;
    #endif

    /* Opcional: apagar módulos (ADC, PWM, Timers) según sea necesario aquí. */
    system_state = SYS_STATE_INIT;
}

void SYSTEM_EnterSleep(void)
{
    /* Marcar estado */
    system_state = SYS_STATE_SLEEP;

    /* Antes de entrar a sleep normalmente se limpian banderas y se configuran
       wake-up sources (INT, CN, TMR, etc.). Eso debe realizarse por el
       llamador si hace falta. */

    /* Para ejecutar la instrucción SLEEP en XC16 se puede usar:
         Sleep();    // si está disponible en tu versión de xc.h
       o usar una built-in asm. Aquí dejamos la instrucción comentada para que
       la actives voluntariamente cuando estés seguro.
    */

    /* Ejemplo (descomentar para usar el SLEEP hardware):
    #ifdef __XC16__
        Sleep();
    #else
        __asm__ volatile ("pwrsav #0");
    #endif
    */

    /* Como alternativa segura para pruebas: esperar a una interrupción (no real sleep) */
    while (system_state == SYS_STATE_SLEEP)
    {
        /* WFI-like behavior simulated */
        __builtin_nop();
        /* Romperemos este bucle con SYSTEM_Wakeup() que cambia system_state */
    }
}

void SYSTEM_Wakeup(void)
{
    /* Cambia estado a READY si antes estaba en SLEEP */
    if (system_state == SYS_STATE_SLEEP) {
        system_state = SYS_STATE_READY;
    }
    /* Si hubiese que reconfigurar clocks o periféricos tras wakeup, hacerlo aquí */
}

void SYSTEM_Reset(void)
{
    /* Marca estado */
    system_state = SYS_STATE_INIT;

    /* ADVERTENCIA:
       Para realizar un reset por software en PIC24/dsPIC hay mecanismos que
       requieren instrucciones especiales y desbloqueos (es dependiente del
       compilador y del dispositivo). A continuación se da un ejemplo
       comentado para XC16; úsalo solo si sabes lo que haces.
    */

    /* Ejemplo de reset por software (comentado):
    // #include <libpic30.h>
    // __builtin_write_OSCCONL(OSCCON | 0x01); // set SWRST (si aplica)
    // for(;;); // esperar reset
    */

    /* Implementación segura por defecto: entrar en bucle infinito para que el
       programador/depurador pueda detectar el "reset" software en ejecución.
       Si deseas forzar un reset real, sustituye por la secuencia adecuada.
    */
    while (1) { __builtin_nop(); }
}

void SYSTEM_EnableInterrupts(void)
{
    /* Habilitar interrupciones globales */
    /* XC16 built-in */
    #ifdef __XC16__
    __builtin_enable_interrupts();
    #else
    /* Fallback: set bit GIE si el registro existe */
    #ifdef INTCON2
    /* no todos los dispositivos exponen GIE en INTCON2; usar macro si existe */
    #endif
    #endif
}

void SYSTEM_DisableInterrupts(void)
{
    /* Deshabilitar interrupciones globales */
    #ifdef __XC16__
    __builtin_disable_interrupts();
    #endif
}

uint32_t SYSTEM_GetClockFrequency(void)
{
    /* Devuelve FCY definido en config.h (frecuencia de instrucción) */
    return (uint32_t)FCY;
}

System_State_t SYSTEM_GetState(void)
{
    return system_state;
}

void SYSTEM_PrintConfiguration(void)
{
    /* Impresión simple de la configuración por stdout si está disponible.
       Si no has retargeteado printf a UART/ITM/etc., esta función no hará
       nada visible. */
    #ifdef __XC16__
    /* Intentar usar printf si la plataforma lo soporta */
    printf("System configuration:\r\n");
    #ifdef CONFIG_OSC_INTERNO_PLL
    printf("  Oscillator: INTERNAL + PLL\r\n");
    #elif defined(CONFIG_OSC_INTERNO_SIMPLE)
    printf("  Oscillator: INTERNAL (no PLL)\r\n");
    #elif defined(CONFIG_OSC_EXTERNO_PLL)
    printf("  Oscillator: EXTERNAL + PLL\r\n");
    #elif defined(CONFIG_OSC_EXTERNO_SIMPLE)
    printf("  Oscillator: EXTERNAL (no PLL)\r\n");
    #else
    printf("  Oscillator: DEFAULT\r\n");
    #endif

    #ifdef CONFIG_WDT_OFF
    printf("  WDT: OFF\r\n");
    #else
    printf("  WDT: ON\r\n");
    #endif

    #ifdef CONFIG_MCLR_ENABLED
    printf("  MCLR: ENABLED\r\n");
    #else
    printf("  MCLR: DISABLED\r\n");
    #endif

    #ifdef CONFIG_BOR_OFF
    printf("  BOR: OFF\r\n");
    #else
    printf("  BOR: ON\r\n");
    #endif

    #ifdef CONFIG_CODE_PROTECT_OFF
    printf("  Code Protect: OFF\r\n");
    #else
    printf("  Code Protect: ON\r\n");
    #endif

    printf("  FCY: %lu Hz\r\n", (unsigned long)SYSTEM_GetClockFrequency());
    #else
    /* Si no hay soporte printf, una alternativa es parpadear LEDs o cambiar
       un puerto para indicar estado; aquí no hacemos nada por defecto. */
    (void)SYSTEM_GetClockFrequency;
    #endif
}
