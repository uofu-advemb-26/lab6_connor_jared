  #ifndef FREERTOS_CONFIG_H
  #define FREERTOS_CONFIG_H

  #define configNUMBER_OF_CORES 1

  // This example uses a common include to avoid repetition
  #include "FreeRTOSConfig_examples_common.h"

  // Run-time stats so tests can measure how long each task ran
  #undef configGENERATE_RUN_TIME_STATS
  #define configGENERATE_RUN_TIME_STATS           1

  #define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS()
  #define portGET_RUN_TIME_COUNTER_VALUE()        ( *( volatile uint32_t * ) 0x40054028 )

  #endif