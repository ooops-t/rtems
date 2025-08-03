/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2014 embedded brains GmbH & Co. KG
 *
 * Copyright (C) 2019 DornerWorks
 *
 * Written by Jeff Kubascik <jeff.kubascik@dornerworks.com>
 *        and Josh Whitehead <josh.whitehead@dornerworks.com>
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

#include <rtems/score/cpu.h>

#include <bsp/start.h>
#include <rtems/score/assert.h>

bool _CPU_SMP_Start_processor(uint32_t cpu_index)
{
  volatile uint32_t* const kick_address = (uint32_t*) 0xfffffff0UL;

  _Assert(cpu_index == 1);

  /*
   * Enable the second CPU.
   */
  _ARM_Data_synchronization_barrier();
  _ARM_Instruction_synchronization_barrier();
  *kick_address = (uint32_t) _start;
  _ARM_Data_synchronization_barrier();
  _ARM_Instruction_synchronization_barrier();
  _ARM_Send_event();

  return true;
}
