/*
 * config.h - Archivo de configuración para dsPIC33FJ32MC204
 *
 * Autor: Carlos Olivera Ylla
 * Fecha: 2025-12-04
 *
 * Descripción: Configuración modular del microcontrolador.
 *
 * USO: Descomenta las opciones que necesites y comenta las que no.
 *      Solo una configuración de cada tipo debe estar activa.
 *
 ******************************************************************************/

#ifndef CONFIG_H
#define CONFIG_H

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>

/* --------------------------------------------------------------------------
 * CONFIGURACIÓN DEL SISTEMA - DESCOMENTAR UNA OPCIÓN POR CATEGORÍA
 * ------------------------------------------------------------------------ */

/* 1. CONFIGURACIÓN DEL OSCILADOR
 *
 * Nota:
 *  - FOSC_PRIM  : frecuencia del oscilador primario / fuente (antes de PLL)
 *  - FOSC       : frecuencia del sistema (después de PLL si aplica)
 *  - FCY        : frecuencia de instrucción (FOSC / 2 para dsPIC33)
 *
 * Asegúrate de seleccionar exactamente UNA de las opciones de abajo.
 */

/* OPCIÓN A: Oscilador interno máximo con PLL (RECOMENDADA) */
#define CONFIG_OSC_INTERNO_PLL

/* OPCIÓN B: Oscilador interno sin PLL */
// #define CONFIG_OSC_INTERNO_SIMPLE

/* OPCIÓN C: Oscilador externo con PLL */
// #define CONFIG_OSC_EXTERNO_PLL

/* OPCIÓN D: Oscilador externo sin PLL */
// #define CONFIG_OSC_EXTERNO_SIMPLE

/* 2. WATCHDOG TIMER */
#define CONFIG_WDT_OFF
// #define CONFIG_WDT_ON_NORMAL
// #define CONFIG_WDT_ON_LONG

/* 3. RESET por MCLR */
#define CONFIG_MCLR_ENABLED
// #define CONFIG_MCLR_DISABLED

/* 4. BROWN-OUT RESET (BOR) */
#define CONFIG_BOR_OFF
// #define CONFIG_BOR_27V
// #define CONFIG_BOR_20V
// #define CONFIG_BOR_42V

/* 5. PROTECCIÓN DE CÓDIGO */
 // #define CONFIG_CODE_PROTECT_ON
#define CONFIG_CODE_PROTECT_OFF

/* 6. PINES DE DEBUG */
#define CONFIG_DEBUG_OFF
// #define CONFIG_DEBUG_ON

/* 7. CLOCK SWITCHING */
#define CONFIG_CLOCK_SWITCH_OFF
// #define CONFIG_CLOCK_SWITCH_ON

/* 8. PUERTOS (ahorro de energía / inicialización condicional) */
 // #define CONFIG_PORT_A_ENABLED
#define CONFIG_PORT_B_ENABLED
 // #define CONFIG_PORT_C_ENABLED
 // #define CONFIG_PORT_D_ENABLED
 // #define CONFIG_PORT_E_ENABLED
 // #define CONFIG_PORT_F_ENABLED
 // #define CONFIG_PORT_G_ENABLED

/* --------------------------------------------------------------------------
 * CONSTANTES DEL SISTEMA (valores coherentes y calculados)
 * ------------------------------------------------------------------------ */

/* Frecuencias por configuración (ajusta los valores primarios según tu hardware) */
#if defined(CONFIG_OSC_INTERNO_PLL)

    /* Ejemplo: oscilador interno 8 MHz -> PLL -> FOSC = 80 MHz -> FCY = 40 MHz */
    #ifndef FOSC_PRIM
    #define FOSC_PRIM   8000000UL   /* frecuencia de la fuente interna (antes del PLL) */
    #endif

    #ifndef FOSC
    #define FOSC        80000000UL  /* frecuencia del sistema después del PLL */
    #endif

    #ifndef FCY
    #define FCY         (FOSC / 2UL) /* instrucción por ciclo: FOSC/2 */
    #endif

#elif defined(CONFIG_OSC_INTERNO_SIMPLE)

    /* Ejemplo: usar oscilador interno simple (sin PLL) */
    #ifndef FOSC_PRIM
    #define FOSC_PRIM   7370000UL   /* ejemplo de oscilador interno exacto (7.37 MHz) */
    #endif

    #ifndef FOSC
    #define FOSC        FOSC_PRIM
    #endif

    #ifndef FCY
    #define FCY         (FOSC / 2UL)
    #endif

#elif defined(CONFIG_OSC_EXTERNO_PLL)

    /* Ajusta según tu cristal/external oscillator */
    #ifndef FOSC_PRIM
    #define FOSC_PRIM   8000000UL
    #endif

    #ifndef FOSC
    #define FOSC        80000000UL
    #endif

    #ifndef FCY
    #define FCY         (FOSC / 2UL)
    #endif

#elif defined(CONFIG_OSC_EXTERNO_SIMPLE)

    #ifndef FOSC_PRIM
    #define FOSC_PRIM   7370000UL
    #endif

    #ifndef FOSC
    #define FOSC        FOSC_PRIM
    #endif

    #ifndef FCY
    #define FCY         (FOSC / 2UL)
    #endif

#else

    /* Si no seleccionaste ninguna opción, definimos valores por defecto seguros */
    #warning "Ninguna opción de oscilador seleccionada en config.h: usando valores por defecto"
    #ifndef FOSC_PRIM
    #define FOSC_PRIM   7370000UL
    #endif
    #ifndef FOSC
    #define FOSC        FOSC_PRIM
    #endif
    #ifndef FCY
    #define FCY         (FOSC / 2UL)
    #endif

#endif /* CONFIG_OSC_* */

/* Asegurarse de que FCY esté definido como entero literal para libpic30 */
#ifndef FCY
#error "FCY no definido. Revisa la sección de configuración de oscilador en config.h"
#endif

/* Incluir libpic30.h para usar __delay_ms y __delay_us.
 * Nota: libpic30.h requiere que FCY esté definido antes de incluirlo.
 */
#include <libpic30.h>

/* Macros de delay */
#define DELAY_MS(ms)    __delay_ms(ms)
#define DELAY_US(us)    __delay_us(us)

/* --------------------------------------------------------------------------
 * TIP: Directivas de configuración (pragma config)
 *
 * No las activo aquí por defecto, pero puedes añadir tu bloque de pragmas
 * específico del dispositivo en un archivo separado (p. ej. system_config.c)
 * o descomentar/ajustar aquí.
 *
 * Ejemplo (comentado):
 *
 * // #pragma config FNOSC = PRIPLL    // Oscillator Selection
 * // #pragma config POSCMD = HS       // Primary Oscillator Mode
 * // #pragma config FWDTEN = OFF      // Watchdog Timer
 *
 * ------------------------------------------------------------------------ */

/* --------------------------------------------------------------------------
 * ESTADOS DEL SISTEMA
 * ------------------------------------------------------------------------ */
typedef enum {
    SYS_STATE_INIT = 0,
    SYS_STATE_READY,
    SYS_STATE_BUSY,
    SYS_STATE_ERROR,
    SYS_STATE_SLEEP
} System_State_t;

/* --------------------------------------------------------------------------
 * PROTOTIPOS DE FUNCIONES (implementa en system.c o donde prefieras)
 * ------------------------------------------------------------------------ */
void SYSTEM_Initialize(void);
void SYSTEM_Deinitialize(void);
void SYSTEM_EnterSleep(void);
void SYSTEM_Wakeup(void);
void SYSTEM_Reset(void);
void SYSTEM_EnableInterrupts(void);
void SYSTEM_DisableInterrupts(void);
uint32_t SYSTEM_GetClockFrequency(void);
System_State_t SYSTEM_GetState(void);
void SYSTEM_PrintConfiguration(void);

/* Implementación inline útil para obtener FCY sin llamar a función separada.
 * Si prefieres, implementa SYSTEM_GetClockFrequency en system.c consultando registros.
 */
static inline uint32_t SYSTEM_GetClockFrequency_inline(void)
{
    return (uint32_t)FCY;
}

#endif /* CONFIG_H */