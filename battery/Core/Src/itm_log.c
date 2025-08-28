#include "itm_log.h"
#include <stdarg.h>
/* Pull in device header to ensure IRQn_Type and CoreDebug definitions present */
#if !defined(USE_HAL_STUBS)
#include "stm32g0xx.h"
#endif

#ifndef ITM_STIMULUS_PORT
#define ITM_STIMULUS_PORT 0
#endif

static inline int ITM_PortReady(void){
#ifdef ITM
    return (ITM->TCR & ITM_TCR_ITMENA_Msk) && (ITM->TER & (1UL << ITM_STIMULUS_PORT));
#else
    return 0;
#endif
}

void ITM_Log_Init(void){
#ifdef CoreDebug
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; /* enable tracing */
#endif
}

void ITM_Log_PutChar(char c){
#ifdef ITM
    if (!ITM_PortReady()) return;
    ITM_SendChar((uint32_t)c);
#else
    (void)c;
#endif
}

void ITM_Log_Write(const char *buf){
    if (!buf) return;
    while(*buf){ ITM_Log_PutChar(*buf++); }
}

void ITM_Log_Printf(const char *fmt, ...){
#ifdef USE_ITM_LOG
    char tmp[160];
    va_list ap; va_start(ap, fmt);
    int n = vsnprintf(tmp, sizeof(tmp)-1, fmt, ap);
    va_end(ap);
    if (n < 0) return;
    if (n >= (int)sizeof(tmp)) n = (int)sizeof(tmp)-2;
    tmp[n++]='\n'; tmp[n]='\0';
    ITM_Log_Write(tmp);
#else
    (void)fmt; /* no-op when ITM logging disabled */
#endif
}
