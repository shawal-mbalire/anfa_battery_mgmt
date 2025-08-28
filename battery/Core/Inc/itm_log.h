#ifndef ITM_LOG_H
#define ITM_LOG_H

#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Simple ITM / SWV logger abstraction.
 * Enable by defining USE_ITM_LOG; fallback to printf otherwise (BQ_LOG macro wraps this).
 * NOTE: Some low-end Cortex-M0/M0+ devices or board debug headers may NOT route SWO pin; if you
 * see no output in your SWV console, confirm:
 *   1. SWO pin is physically connected on the NUCLEO board (check SB / solder bridges).
 *   2. TRACECLK is enabled (CoreDebug->DEMCR.TRCENA) and viewer sets correct core clock.
 *   3. Stimulus port 0 is enabled in ITM->TER.
 * In absence of SWV, builds with LOG=printf still provide UART/semihost style output.
 */

/* Initialize ITM logging (optionally enable trace in DEMCR). */
void ITM_Log_Init(void);

/* Non-blocking character send; drops if port not ready. */
void ITM_Log_PutChar(char c);

/* Write raw buffer. */
void ITM_Log_Write(const char *buf);

/* printf-style convenience (small static buffer). */
void ITM_Log_Printf(const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif /* ITM_LOG_H */
