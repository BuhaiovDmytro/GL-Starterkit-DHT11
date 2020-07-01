#ifndef HW_DELAY_H
#define HW_DELAY_H

#define CORE_FREQ 168000000UL

#ifndef DELAY_LOOP_CYCLES
#define DELAY_LOOP_CYCLES 3.0
#endif

#define delay_us( US ) _delay_loops( (uint32_t)((double)US * CORE_FREQ / DELAY_LOOP_CYCLES / 1000000.0) )
#define delay_ms( MS ) _delay_loops( (uint32_t)((double)MS * CORE_FREQ / DELAY_LOOP_CYCLES / 1000.0) )
#define delay_s( S )   _delay_loops( (uint32_t)((double)S  * CORE_FREQ / DELAY_LOOP_CYCLES) )

static inline void _delay_loops(uint32_t loops)
{
   asm volatile (
      "1: SUBS %[loops], %[loops], #1 \n"
      "   BNE 1b \n"
      : [loops] "+r"(loops)
   );
}

#endif /* HW_DELAY_H */
