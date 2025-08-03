/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief RTEMS Monitor semaphore support
 */

/*
 * COPYRIGHT (c) 1989-2022. On-Line Applications Research Corporation (OAR).
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <rtems/monitor.h>
#include <rtems/rtems/semimpl.h>
#include <stdio.h>
#include <string.h>    /* memcpy() */

void
rtems_monitor_sema_canonical(
    rtems_monitor_sema_t  *canonical_sema,
    const void            *sema_void
)
{
    const Semaphore_Control *rtems_sema;
    uintptr_t flags;
    Thread_Control *owner;

    canonical_sema->attribute = 0;
    canonical_sema->priority_ceiling = 0;
    canonical_sema->max_count = 0;
    canonical_sema->cur_count = 0;
    canonical_sema->holder_id = 0;

    rtems_sema = (const Semaphore_Control *) sema_void;
    flags = _Semaphore_Get_flags( rtems_sema );

#if defined(RTEMS_MULTIPROCESSING)
    if ( _Semaphore_Is_global( flags ) ) {
      canonical_sema->attribute |= RTEMS_GLOBAL;
    }
#endif

    if ( _Semaphore_Get_discipline( flags ) == SEMAPHORE_DISCIPLINE_PRIORITY ) {
      canonical_sema->attribute |= RTEMS_PRIORITY;
    }

    switch ( _Semaphore_Get_variant( flags ) ) {
      case SEMAPHORE_VARIANT_MUTEX_INHERIT_PRIORITY:
        canonical_sema->attribute |= RTEMS_BINARY_SEMAPHORE
          | RTEMS_INHERIT_PRIORITY;
        break;
      case SEMAPHORE_VARIANT_MUTEX_PRIORITY_CEILING:
        canonical_sema->attribute |= RTEMS_BINARY_SEMAPHORE
          | RTEMS_PRIORITY_CEILING;
        break;
      case SEMAPHORE_VARIANT_MUTEX_NO_PROTOCOL:
        canonical_sema->attribute |= RTEMS_BINARY_SEMAPHORE;
        break;
#if defined(RTEMS_SMP)
      case SEMAPHORE_VARIANT_MRSP:
        canonical_sema->attribute |= RTEMS_BINARY_SEMAPHORE
          | RTEMS_MULTIPROCESSOR_RESOURCE_SHARING;
        break;
#endif
      case SEMAPHORE_VARIANT_SIMPLE_BINARY:
        canonical_sema->attribute |= RTEMS_SIMPLE_BINARY_SEMAPHORE;
        break;
      case SEMAPHORE_VARIANT_COUNTING:
        canonical_sema->attribute |= RTEMS_COUNTING_SEMAPHORE;
        break;
    }

    switch ( _Semaphore_Get_variant( flags ) ) {
      case SEMAPHORE_VARIANT_MUTEX_PRIORITY_CEILING:
        canonical_sema->priority_ceiling = _Scheduler_Unmap_priority(
          _CORE_ceiling_mutex_Get_scheduler( &rtems_sema->Core_control.Mutex ),
          _CORE_ceiling_mutex_Get_priority( &rtems_sema->Core_control.Mutex )
        );
        /* Fall through */
      case SEMAPHORE_VARIANT_MUTEX_INHERIT_PRIORITY:
      case SEMAPHORE_VARIANT_MUTEX_NO_PROTOCOL:
        owner = _CORE_mutex_Get_owner(
          &rtems_sema->Core_control.Mutex.Recursive.Mutex
        );

        if (owner != NULL) {
          canonical_sema->holder_id = owner->Object.id;
          canonical_sema->cur_count = 0;
        } else {
          canonical_sema->cur_count = 1;
        }

        canonical_sema->max_count = 1;
        break;
#if defined(RTEMS_SMP)
      case SEMAPHORE_VARIANT_MRSP:
        canonical_sema->cur_count =
          _MRSP_Get_owner( &rtems_sema->Core_control.MRSP ) == NULL;
        canonical_sema->max_count = 1;
        break;
#endif
      case SEMAPHORE_VARIANT_SIMPLE_BINARY:
        canonical_sema->cur_count = rtems_sema->Core_control.Semaphore.count;
        canonical_sema->max_count = 1;
        break;
      case SEMAPHORE_VARIANT_COUNTING:
        canonical_sema->cur_count = rtems_sema->Core_control.Semaphore.count;
        canonical_sema->max_count = UINT32_MAX;
        break;
    }
}


void
rtems_monitor_sema_dump_header(
    bool verbose RTEMS_UNUSED
)
{
    printf("\
  ID       NAME   ATTR        PRICEIL CURR_CNT HOLDID \n");
/*23456789 123456789 123456789 123456789 123456789 123456789 123456789 1234
          1         2         3         4         5         6         7    */

    rtems_monitor_separator();
}

/*
 */

void
rtems_monitor_sema_dump(
    rtems_monitor_sema_t *monitor_sema,
    bool  verbose RTEMS_UNUSED
)
{
    int length = 0;

    length += rtems_monitor_dump_id(monitor_sema->id);
    length += rtems_monitor_pad(11, length);
    length += rtems_monitor_dump_name(monitor_sema->id);
    length += rtems_monitor_pad(18, length);
    length += rtems_monitor_dump_attributes(monitor_sema->attribute);
    length += rtems_monitor_pad(30, length);
    length += rtems_monitor_dump_priority(monitor_sema->priority_ceiling);
    length += rtems_monitor_pad(38, length);
    length += rtems_monitor_dump_decimal(monitor_sema->cur_count);
    length += rtems_monitor_pad(47, length);
    length += rtems_monitor_dump_id(monitor_sema->holder_id);
    printf("\n");
}
