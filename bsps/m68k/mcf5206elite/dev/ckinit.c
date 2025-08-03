/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  Clock Driver for MCF5206eLITE board
 *
 *  This driver initailizes timer1 on the MCF5206E as the
 *  main system clock
 */

/*
 *  Author: Victor V. Vengerov <vvv@oktet.ru>
 *
 *  Based on work:
 *    David Fiddes, D.J@fiddes.surfaid.org
 *    http://www.calm.hw.ac.uk/davidf/coldfire/
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdlib.h>
#include <bsp.h>
#include <rtems/libio.h>
#include <rtems/clockdrv.h>
#include "mcf5206/mcf5206e.h"

/*
 * Clock_driver_ticks is a monotonically increasing counter of the
 * number of clock ticks since the driver was initialized.
 */
volatile uint32_t   Clock_driver_ticks;

rtems_isr (*rtems_clock_hook)(rtems_vector_number) = NULL;

static rtems_isr
Clock_isr (rtems_vector_number vector)
{
  /* Clear pending interrupt... */
  *MCF5206E_TER(MBAR,1) = MCF5206E_TER_REF | MCF5206E_TER_CAP;

  /* Announce the clock tick */
  Clock_driver_ticks++;
  rtems_clock_tick();
  if (rtems_clock_hook != NULL)
      rtems_clock_hook(vector);
}

static void
Clock_exit(void)
{
  /* disable all timer1 interrupts */
  *MCF5206E_IMR(MBAR) |= MCF5206E_INTR_BIT(MCF5206E_INTR_TIMER_1);

  /* reset timer1 */
  *MCF5206E_TMR(MBAR,1) = MCF5206E_TMR_ICLK_STOP;

  /* clear pending */
  *MCF5206E_TER(MBAR,1) = MCF5206E_TER_REF | MCF5206E_TER_CAP;
}


static void
Install_clock(rtems_isr_entry clock_isr)
{
  Clock_driver_ticks = 0;

  /* Configure timer1 interrupts */
  *MCF5206E_ICR(MBAR,MCF5206E_INTR_TIMER_1) =
      MCF5206E_ICR_AVEC |
      ((BSP_INTLVL_TIMER1 << MCF5206E_ICR_IL_S) & MCF5206E_ICR_IL) |
      ((BSP_INTPRIO_TIMER1 << MCF5206E_ICR_IP_S) & MCF5206E_ICR_IP);

  /* Register the interrupt handler */
  set_vector(clock_isr, BSP_INTVEC_TIMER1, 1);

  /* Reset timer 1 */
  *MCF5206E_TMR(MBAR, 1) = MCF5206E_TMR_RST;
  *MCF5206E_TMR(MBAR, 1) = MCF5206E_TMR_ICLK_STOP;
  *MCF5206E_TMR(MBAR, 1) = MCF5206E_TMR_RST;
  *MCF5206E_TCN(MBAR, 1) = 0; /* Reset counter */
  *MCF5206E_TER(MBAR, 1) = MCF5206E_TER_REF | MCF5206E_TER_CAP;

  /* Set Timer 1 prescaler so that it counts in microseconds */
  *MCF5206E_TMR(MBAR, 1) =
      (((BSP_SYSTEM_FREQUENCY/1000000 - 1) << MCF5206E_TMR_PS_S) &
       MCF5206E_TMR_PS) |
      MCF5206E_TMR_CE_NONE | MCF5206E_TMR_ORI | MCF5206E_TMR_FRR |
      MCF5206E_TMR_RST;

  /* Set the timer timeout value from the BSP config */
  *MCF5206E_TRR(MBAR, 1) = rtems_configuration_get_microseconds_per_tick() - 1;

  /* Feed system frequency to the timer */
  *MCF5206E_TMR(MBAR, 1) |= MCF5206E_TMR_ICLK_MSCLK;

  /* Enable timer 1 interrupts */
  *MCF5206E_IMR(MBAR) &= ~MCF5206E_INTR_BIT(MCF5206E_INTR_TIMER_1);

  /* Register the driver exit procedure so we can shutdown */
  atexit(Clock_exit);
}

void _Clock_Initialize( void )
{
  Install_clock (Clock_isr);
}
