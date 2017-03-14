//
//  ws2812.cpp
//  DMX Controller
//
//  Created by Chris Staite on 09/02/2017.
//  Copyright © 2017 Chris Staite. All rights reserved.
//

#include "ws2812.h"
#include "pin.h"

#include <avr/io.h>
#include <avr/interrupt.h>

// Timing in ns
#define w_zeropulse   350
#define w_onepulse    900
#define w_totalperiod 1250

// Fixed cycles used by the inner loop
#define w_fixedlow    2
#define w_fixedhigh   4
#define w_fixedtotal  8   

// Insert NOPs to match the timing, if possible
#define w_zerocycles    (((F_CPU/1000)*w_zeropulse          )/1000000)  // 2.8
#define w_onecycles     (((F_CPU/1000)*w_onepulse    +500000)/1000000)  // 7.7
#define w_totalcycles   (((F_CPU/1000)*w_totalperiod +500000)/1000000)  // 10.5

// w1 - nops between rising edge and falling edge - low
#define w1 (w_zerocycles-w_fixedlow)  // 0.8
// w2   nops between fe low and fe high
#define w2 (w_onecycles-w_fixedhigh-w1)  // 2.9
// w3   nops to complete loop
#define w3 (w_totalcycles-w_fixedtotal-w1-w2)  // -1.2

#if w1>0
  #define w1_nops w1
#else
  #define w1_nops  0
#endif

// The only critical timing parameter is the minimum pulse length of the "0"
// Warn or throw error if this timing can not be met with current F_CPU settings.
#define w_lowtime ((w1_nops+w_fixedlow)*1000000)/(F_CPU/1000)
#if w_lowtime>550
   #error "Light_ws2812: Sorry, the clock speed is too low. Did you set F_CPU correctly?"
#elif w_lowtime>450
   #warning "Light_ws2812: The timing is critical and may only work on WS2812B, not on WS2812(S)."
   #warning "Please consider a higher clockspeed, if possible"
#endif   

#if w2>0
#define w2_nops w2
#else
#define w2_nops  0
#endif

#if w3>0
#define w3_nops w3
#else
#define w3_nops  0
#endif

#define w_nop1  "nop      \n\t"
#define w_nop2  "rjmp .+0 \n\t"
#define w_nop4  w_nop2 w_nop2
#define w_nop8  w_nop4 w_nop4
#define w_nop16 w_nop8 w_nop8

void inline ws2812_sendarray_mask(uint8_t *data,uint16_t datlen,uint8_t maskhi)
{
  uint8_t curbyte,ctr,masklo;
  uint8_t sreg_prev;
  
  DDRA |= maskhi; // Enable output
  
  masklo	=~maskhi&PORTA;
  maskhi |=        PORTA;
  
  sreg_prev=SREG;
  cli();  

  while (datlen--) {
    curbyte=*data++;
    
    asm volatile(
    "       ldi   %0,8  \n\t"
    "loop%=:            \n\t"
    "       out   %2,%3 \n\t"    //  '1' [01] '0' [01] - re
#if (w1_nops&1)
w_nop1
#endif
#if (w1_nops&2)
w_nop2
#endif
#if (w1_nops&4)
w_nop4
#endif
#if (w1_nops&8)
w_nop8
#endif
#if (w1_nops&16)
w_nop16
#endif
    "       sbrs  %1,7  \n\t"    //  '1' [03] '0' [02]
    "       out   %2,%4 \n\t"    //  '1' [--] '0' [03] - fe-low
    "       lsl   %1    \n\t"    //  '1' [04] '0' [04]
#if (w2_nops&1)
  w_nop1
#endif
#if (w2_nops&2)
  w_nop2
#endif
#if (w2_nops&4)
  w_nop4
#endif
#if (w2_nops&8)
  w_nop8
#endif
#if (w2_nops&16)
  w_nop16 
#endif
    "       out   %2,%4 \n\t"    //  '1' [+1] '0' [+1] - fe-high
#if (w3_nops&1)
w_nop1
#endif
#if (w3_nops&2)
w_nop2
#endif
#if (w3_nops&4)
w_nop4
#endif
#if (w3_nops&8)
w_nop8
#endif
#if (w3_nops&16)
w_nop16
#endif

    "       dec   %0    \n\t"    //  '1' [+2] '0' [+2]
    "       brne  loop%=\n\t"    //  '1' [+3] '0' [+4]
    :	"=&d" (ctr)
    :	"r" (curbyte), "I" (_SFR_IO_ADDR(PORTA)), "r" (maskhi), "r" (masklo)
    );
  }
  
  SREG=sreg_prev;
}

namespace led {

void WS2812::sendRgb(RGB* values, uint16_t count)
{
    static_assert(sizeof(RGB) == 3, "RGB structure has been padded");
    ws2812_sendarray_mask(reinterpret_cast<uint8_t*>(values), count * sizeof(RGB), m_mask);
}

}  // namespace led

/*
namespace led {

namespace {

// This routine writes an array of bytes with RGB values to the Dataout pin
// using the fast 800kHz clockless WS2811/2812 protocol.

// Timing in ns
constexpr int32_t ZERO_PULSE_NS   = 400;   //  400 +- 150
constexpr int32_t ONE_PULSE_NS    = 800;   //  800 +- 150
constexpr int32_t TOTAL_PERIOD_NS = 1250;  // 1250 +- 300

// The number of cycles used by the instructions for
// each bit of the loop
constexpr int32_t LOW_CYCLES_USED   = 3;
constexpr int32_t HIGH_CYCLES_USED  = 6;
constexpr int32_t TOTAL_CYCLES_USED = 10;

// Calculate the number of cycles we should be emitting each pulse for
constexpr int32_t ZERO_CYCLES  = ((F_CPU / 1000) *   ZERO_PULSE_NS         ) / 1000000;
// Round up
constexpr int32_t ONE_CYCLES   = ((F_CPU / 1000) *    ONE_PULSE_NS + 500000) / 1000000;
// Round up
constexpr int32_t TOTAL_CYCLES = ((F_CPU / 1000) * TOTAL_PERIOD_NS + 500000) / 1000000;

// Calculate the difference between the cycles used and the optimum
// number of cycles for each stage, ignoring negative numbers
constexpr int32_t ZERO_CYCLES_NOP =
    (ZERO_CYCLES - LOW_CYCLES_USED < 0) ? 0 : (ZERO_CYCLES - LOW_CYCLES_USED);
constexpr int32_t ONE_CYCLES_NOP =
    (ONE_CYCLES - HIGH_CYCLES_USED - (ZERO_CYCLES - LOW_CYCLES_USED) < 0) ? 0 :
    (ONE_CYCLES - HIGH_CYCLES_USED - (ZERO_CYCLES - LOW_CYCLES_USED));
constexpr int32_t FINAL_CYCLES_NOP =
    (TOTAL_CYCLES - TOTAL_CYCLES_USED - (ONE_CYCLES - HIGH_CYCLES_USED) < 0) ? 0 :
    (TOTAL_CYCLES - TOTAL_CYCLES_USED - (ONE_CYCLES - HIGH_CYCLES_USED));

// The only critical timing parameter is the minimum pulse length of the "0"
// Warn or throw error if this timing can not be met with current F_CPU settings.
static_assert(((ZERO_CYCLES_NOP + LOW_CYCLES_USED) * 1000000) / (F_CPU / 1000) <= 550,
              "The clock speed is too low for the WS2812");
static_assert(((ZERO_CYCLES_NOP + LOW_CYCLES_USED) * 1000000) / (F_CPU / 1000) >= 250,
              "The clock speed is too high for the WS2812");

// Here's a nice little template to produce our NOP's based on
// the values we just calculated...
template<int32_t N>
__attribute__((always_inline)) inline void nop()
{
    nop<N-2>();
    nop<2>();
}

template<>
__attribute__((always_inline)) inline void nop<0>()
{
}

template<>
__attribute__((always_inline)) inline void nop<1>()
{
    asm volatile("nop\n\t");
}

template<>
__attribute__((always_inline)) inline void nop<2>()
{
    asm volatile("rjmp .+0\n\t");
}

}  // anon namespace

void WS2812::sendRgb(RGB* values, uint16_t count)
{
    static_assert(sizeof(RGB) == 3, "RGB structure has been padded");
    sendRgb(reinterpret_cast<uint8_t*>(values), count * sizeof(RGB));
}

void WS2812::sendRgb(uint8_t* values, uint16_t count)
{
    // Counter for the 8 bits in the byte
    uint8_t ctr;
    
    count = 3;

    // Calculate the masks
    volatile uint8_t* port = &m_port;
    uint8_t masklo = *port & ~m_mask;
    uint8_t maskhi = *port |  m_mask;

    // Save the interrupt state and disable whether
    // it's set or not...
    uint8_t sreg_prev = SREG;
    cli();

    while (count--)
    {
        // One line only to evaluate to ldi rn, Z+
        // Does not get optimized to that on two lines...
        uint8_t curbyte = *values++;

        asm volatile(
            "ldi %0, 8\n\t"
            "WS2812loop:\n\t"
            "st X, %3\n\t"
            : "=&d" (ctr)
            : "r" (curbyte), "x" (port), "r" (maskhi), "r" (masklo)
        );
        nop<ZERO_CYCLES_NOP>();
        asm volatile(
            "sbrs %1, 7\n\t"
            "st X, %4\n\t"
            "lsl %1\n\t"
            : "=&d" (ctr)
            : "r" (curbyte), "x" (port), "r" (maskhi), "r" (masklo)
        );
        nop<ONE_CYCLES_NOP>();
        asm volatile(
            "brcc skipone%=\n\t"
            "st X, %4\n\t"
            "skipone%=:"
            :	"=&d" (ctr)
            :	"r" (curbyte), "x" (port), "r" (maskhi), "r" (masklo)
        );
        nop<FINAL_CYCLES_NOP>();
        asm volatile(
            "dec   %0\n\t"
            "brne  WS2812loop\n\t"
            :	"=&d" (ctr)
            :	"r" (curbyte), "x" (port), "r" (maskhi), "r" (masklo)
        );
    }

    // Re-enable interrupts if they were already enabled
    SREG = sreg_prev;
}

}  // namespace led
*/
