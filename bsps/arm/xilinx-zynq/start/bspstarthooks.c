/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMZynq
 *
 * @brief This source file contains the implementation of bsp_start_hook_0()
 *   and bsp_start_hook_1().
 */

/*
 * Copyright (C) 2013, 2014 embedded brains GmbH & Co. KG
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

#define ARM_CP15_TEXT_SECTION BSP_START_TEXT_SECTION

#include <bsp.h>
#include <bsp/start.h>
#include <bsp/arm-a9mpcore-start.h>

BSP_START_TEXT_SECTION void bsp_start_hook_0(void)
{
  /* CLOCK_DRIVER_USE_ONLY_BOOT_PROCESSOR is an indicator for QEMU BSPs */
#if defined(CLOCK_DRIVER_USE_ONLY_BOOT_PROCESSOR) && defined(RTEMS_SMP)
  uint32_t cpu_id = arm_cortex_a9_get_multiprocessor_cpu_id();

  /* QEMU starts both cores, so wait until this core is intended to start. */
  if (cpu_id != 0) {
    volatile uint32_t *kick_address = (uint32_t *)0xfffffff0;
    while (*kick_address == 0) {
      _ARM_Wait_for_event();
    }
  }
#endif
  arm_a9mpcore_start_hook_0();
}

BSP_START_TEXT_SECTION void bsp_start_hook_1(void)
{
  arm_a9mpcore_start_hook_1();
  bsp_start_copy_sections();
  zynq_setup_mmu_and_cache();

#if !defined(RTEMS_SMP) \
  && (defined(BSP_DATA_CACHE_ENABLED) \
    || defined(BSP_INSTRUCTION_CACHE_ENABLED))
  /* Enable unified L2 cache */
  rtems_cache_enable_data();
#endif

  bsp_start_clear_bss();
}
