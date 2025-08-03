/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreCPUAArch64
 *
 * @brief This header file provides interfaces of the AArch64 CPU port.
 */

/*
 * Copyright (C) 2020 On-Line Applications Research Corporation (OAR)
 * Written by Kinsey Moore <kinsey.moore@oarcorp.com>
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

#ifndef _RTEMS_SCORE_CPU_H
#define _RTEMS_SCORE_CPU_H

#include <rtems/score/basedefs.h>
#if defined(RTEMS_PARAVIRT)
#include <rtems/score/paravirt.h>
#endif
#include <rtems/score/aarch64.h>
#include <libcpu/vectors.h>
#include <limits.h>

/**
 * @defgroup RTEMSScoreCPUAArch64 AArch64
 *
 * @ingroup RTEMSScoreCPU
 *
 * @brief This group contains the AArch64 CPU port.
 *
 * @{
 */

/**
 * @name Program State Registers
 */
/**@{**/

#define AARCH64_PSTATE_N (1LL << 31)
#define AARCH64_PSTATE_Z (1LL << 30)
#define AARCH64_PSTATE_C (1LL << 29)
#define AARCH64_PSTATE_V (1LL << 28)
#define AARCH64_PSTATE_D (1LL << 9)
#define AARCH64_PSTATE_A (1LL << 8)
#define AARCH64_PSTATE_I (1LL << 7)
#define AARCH64_PSTATE_F (1LL << 6)

/** @} */

/*
 *  AArch64 uses the PIC interrupt model.
 */
#define CPU_SIMPLE_VECTORED_INTERRUPTS FALSE

#define CPU_ISR_PASSES_FRAME_POINTER FALSE

#define CPU_HARDWARE_FP FALSE

#define CPU_SOFTWARE_FP FALSE

#define CPU_ALL_TASKS_ARE_FP FALSE

#define CPU_IDLE_TASK_IS_FP FALSE

#define CPU_USE_DEFERRED_FP_SWITCH FALSE

#define CPU_ENABLE_ROBUST_THREAD_DISPATCH TRUE

#define CPU_STACK_GROWS_UP FALSE

#if defined(AARCH64_MULTILIB_CACHE_LINE_MAX_64)
  #define CPU_CACHE_LINE_BYTES 64
#else
  #define CPU_CACHE_LINE_BYTES 32
#endif

#define CPU_STRUCTURE_ALIGNMENT RTEMS_ALIGNED( CPU_CACHE_LINE_BYTES )

#define CPU_MODES_INTERRUPT_MASK 0x1

#define CPU_MPCI_RECEIVE_SERVER_EXTRA_STACK 0

#define CPU_PROVIDES_ISR_IS_IN_PROGRESS FALSE

#define CPU_STACK_MINIMUM_SIZE (1024 * 8)

/* This could be either 4 or 8, depending on the ABI in use.
 * Could also use __LP64__ or __ILP32__ */
/* AAPCS64, section 5.1, Fundamental Data Types */
#define CPU_SIZEOF_POINTER __SIZEOF_POINTER__

/* AAPCS64, section 5.1, Fundamental Data Types */
#define CPU_ALIGNMENT 16

#define CPU_HEAP_ALIGNMENT CPU_ALIGNMENT

/* AAPCS64, section 6.2.2, Stack constraints at a public interface */
#define CPU_STACK_ALIGNMENT 16

#define CPU_INTERRUPT_STACK_ALIGNMENT CPU_CACHE_LINE_BYTES

#define CPU_USE_GENERIC_BITFIELD_CODE TRUE

#define CPU_USE_LIBC_INIT_FINI_ARRAY TRUE

#define CPU_MAXIMUM_PROCESSORS 32

#define AARCH64_CONTEXT_CONTROL_THREAD_ID_OFFSET 0x70

#ifdef AARCH64_MULTILIB_VFP
  #define AARCH64_CONTEXT_CONTROL_D8_OFFSET 0x78
#endif

#define AARCH64_CONTEXT_CONTROL_ISR_DISPATCH_DISABLE 0x68

#ifdef RTEMS_SMP
  #if defined(AARCH64_MULTILIB_VFP)
    #define AARCH64_CONTEXT_CONTROL_IS_EXECUTING_OFFSET 0xb8
  #else
    #define AARCH64_CONTEXT_CONTROL_IS_EXECUTING_OFFSET 0x78
  #endif
#endif

#define AARCH64_EXCEPTION_FRAME_SIZE 0x350

#define AARCH64_EXCEPTION_FRAME_REGISTER_SP_OFFSET 0xF8
#define AARCH64_EXCEPTION_FRAME_REGISTER_LR_OFFSET 0xF0
#define AARCH64_EXCEPTION_FRAME_REGISTER_DAIF_OFFSET 0x108
#define AARCH64_EXCEPTION_FRAME_REGISTER_SYNDROME_OFFSET 0x118
#define AARCH64_EXCEPTION_FRAME_REGISTER_VECTOR_OFFSET 0x128
#define AARCH64_EXCEPTION_FRAME_REGISTER_FPSR_OFFSET 0x138
#define AARCH64_EXCEPTION_FRAME_REGISTER_Q0_OFFSET 0x150

#ifndef ASM

#ifdef __cplusplus
extern "C" {
#endif

/*
   This is to fix the following warning
   ISO C does not support 'uint128_t' types
*/
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
typedef unsigned __int128 uint128_t;
#pragma GCC diagnostic pop

typedef struct {
  uint64_t register_x19;
  uint64_t register_x20;
  uint64_t register_x21;
  uint64_t register_x22;
  uint64_t register_x23;
  uint64_t register_x24;
  uint64_t register_x25;
  uint64_t register_x26;
  uint64_t register_x27;
  uint64_t register_x28;
  uint64_t register_fp;
  uint64_t register_lr;
  uint64_t register_sp;
  uint64_t isr_dispatch_disable;
  uint64_t thread_id;
#ifdef AARCH64_MULTILIB_VFP
  uint64_t register_d8;
  uint64_t register_d9;
  uint64_t register_d10;
  uint64_t register_d11;
  uint64_t register_d12;
  uint64_t register_d13;
  uint64_t register_d14;
  uint64_t register_d15;
#endif
#ifdef RTEMS_SMP
  volatile bool is_executing;
#endif
} Context_Control;

static inline void _AARCH64_Data_memory_barrier( void )
{
  __asm__ volatile ( "dmb SY" : : : "memory" );
}

static inline void _AARCH64_Data_synchronization_barrier( void )
{
  __asm__ volatile ( "dsb SY" : : : "memory" );
}

static inline void _AARCH64_Instruction_synchronization_barrier( void )
{
  __asm__ volatile ( "isb" : : : "memory" );
}

void _CPU_ISR_Set_level( uint32_t level );

uint32_t _CPU_ISR_Get_level( void );

#if defined(AARCH64_DISABLE_INLINE_ISR_DISABLE_ENABLE)
uint64_t AArch64_interrupt_disable( void );
void AArch64_interrupt_enable( uint64_t isr_cookie );
void AArch64_interrupt_flash( uint64_t isr_cookie );
#else
static inline uint64_t AArch64_interrupt_disable( void )
{
  uint64_t isr_cookie;

  __asm__ volatile (
    "mrs %[isr_cookie], DAIF\n"
    "msr DAIFSet, #0x2\n"
    : [isr_cookie] "=&r" (isr_cookie)
  );

  return isr_cookie;
}

static inline void AArch64_interrupt_enable( uint64_t isr_cookie )
{
  __asm__ volatile (
    "msr DAIF, %[isr_cookie]\n"
    : : [isr_cookie] "r" (isr_cookie)
  );
}

static inline void AArch64_interrupt_flash( uint64_t isr_cookie )
{
  AArch64_interrupt_enable(isr_cookie);
  AArch64_interrupt_disable();
}
#endif  /* !AARCH64_DISABLE_INLINE_ISR_DISABLE_ENABLE */

#define _CPU_ISR_Disable( _isr_cookie ) \
  do { \
    _isr_cookie = AArch64_interrupt_disable(); \
  } while (0)

#define _CPU_ISR_Enable( _isr_cookie )  \
  AArch64_interrupt_enable( _isr_cookie )

#define _CPU_ISR_Flash( _isr_cookie ) \
  AArch64_interrupt_flash( _isr_cookie )

static inline bool _CPU_ISR_Is_enabled( uint64_t isr_cookie )
{
  return ( isr_cookie & AARCH64_PSTATE_I ) == 0;
}

void _CPU_Context_Initialize(
  Context_Control *the_context,
  void *stack_area_begin,
  size_t stack_area_size,
  uint64_t new_level,
  void (*entry_point)( void ),
  bool is_fp,
  void *tls_area
);

#define _CPU_Context_Get_SP( _context ) \
  (_context)->register_sp

#ifdef RTEMS_SMP
  static inline bool _CPU_Context_Get_is_executing(
    const Context_Control *context
  )
  {
    return context->is_executing;
  }

  static inline void _CPU_Context_Set_is_executing(
    Context_Control *context,
    bool is_executing
  )
  {
    context->is_executing = is_executing;
  }

  RTEMS_NO_RETURN void _AArch64_Start_multitasking( Context_Control *heir );

  #define _CPU_Start_multitasking( _heir ) _AArch64_Start_multitasking( _heir )
#endif

#define _CPU_Context_Restart_self( _the_context ) \
   _CPU_Context_restore( (_the_context) );

#define _CPU_Context_Initialize_fp( _destination ) \
  do { \
    *(*(_destination)) = _CPU_Null_fp_context; \
  } while (0)

/**
 * @brief CPU initialization.
 */
void _CPU_Initialize( void );

typedef void ( *CPU_ISR_handler )( void );

/**
 * @brief CPU switch context.
 */
void _CPU_Context_switch( Context_Control *run, Context_Control *heir );

RTEMS_NO_RETURN void _CPU_Context_switch_no_return(
  Context_Control *executing,
  Context_Control *heir
);

RTEMS_NO_RETURN void _CPU_Context_restore( Context_Control *new_context );

#ifdef RTEMS_SMP
  uint32_t _CPU_SMP_Initialize( void );

  bool _CPU_SMP_Start_processor( uint32_t cpu_index );

  void _CPU_SMP_Finalize_initialization( uint32_t cpu_count );

  void _CPU_SMP_Prepare_start_multitasking( void );

  /**
   * @brief Returns the index of the current processor.
   *
   * The function depends on _CPU_Get_current_per_CPU_control().
   * It shall not be used before the TPIDR_EL1 is initialized.
   */
  uint32_t _CPU_SMP_Get_current_processor( void );

  void _CPU_SMP_Send_interrupt( uint32_t target_processor_index );

  static inline void _AARCH64_Send_event( void )
  {
    __asm__ volatile ( "sev" : : : "memory" );
  }

  static inline void _AARCH64_Wait_for_event( void )
  {
    __asm__ volatile ( "wfe" : : : "memory" );
  }
#endif


static inline uint32_t CPU_swap_u32( uint32_t value )
{
  uint32_t tmp = value; /* make compiler warnings go away */
  __asm__ volatile ("EOR %1, %0, %0, ROR #16\n"
                "BIC %1, %1, #0xff0000\n"
                "MOV %0, %0, ROR #8\n"
                "EOR %0, %0, %1, LSR #8\n"
                : "=r" (value), "=r" (tmp)
                : "0" (value), "1" (tmp));
  return value;
}

static inline uint16_t CPU_swap_u16( uint16_t value )
{
  return (uint16_t) (((value & 0xffU) << 8) | ((value >> 8) & 0xffU));
}

typedef uint32_t CPU_Counter_ticks;

uint32_t _CPU_Counter_frequency( void );

CPU_Counter_ticks _CPU_Counter_read( void );

RTEMS_NO_RETURN void *_CPU_Thread_Idle_body( uintptr_t ignored );

typedef enum {
  AARCH64_EXCEPTION_SP0_SYNCHRONOUS = 0,
  AARCH64_EXCEPTION_SP0_IRQ = 1,
  AARCH64_EXCEPTION_SP0_FIQ = 2,
  AARCH64_EXCEPTION_SP0_SERROR = 3,
  AARCH64_EXCEPTION_SPx_SYNCHRONOUS = 4,
  AARCH64_EXCEPTION_SPx_IRQ = 5,
  AARCH64_EXCEPTION_SPx_FIQ = 6,
  AARCH64_EXCEPTION_SPx_SERROR = 7,
  AARCH64_EXCEPTION_LEL64_SYNCHRONOUS = 8,
  AARCH64_EXCEPTION_LEL64_IRQ = 9,
  AARCH64_EXCEPTION_LEL64_FIQ = 10,
  AARCH64_EXCEPTION_LEL64_SERROR = 11,
  AARCH64_EXCEPTION_LEL32_SYNCHRONOUS = 12,
  AARCH64_EXCEPTION_LEL32_IRQ = 13,
  AARCH64_EXCEPTION_LEL32_FIQ = 14,
  AARCH64_EXCEPTION_LEL32_SERROR = 15,
  MAX_EXCEPTIONS = 16,
  AARCH64_EXCEPTION_MAKE_ENUM_64_BIT = INT_MAX
} AArch64_symbolic_exception_name;

#define VECTOR_POINTER_OFFSET 0x78
#define VECTOR_ENTRY_SIZE 0x80
void _AArch64_Exception_interrupt_no_nest( void );
void _AArch64_Exception_interrupt_nest( void );
static inline void* AArch64_set_exception_handler(
  AArch64_symbolic_exception_name exception,
  void (*handler)(void)
)
{
  /* get current table address */
  char *vbar = (char*)AArch64_get_vector_base_address();

  /* calculate address of vector to be replaced */
  char *cvector_address = vbar + VECTOR_ENTRY_SIZE * exception
    + VECTOR_POINTER_OFFSET;

  /* get current vector pointer */
  void (**vector_address)(void) = (void(**)(void))cvector_address;
  void (*current_vector_pointer)(void);
  current_vector_pointer = *vector_address;

  /* replace vector pointer */
  *vector_address = handler;

  /* return now-previous vector pointer */

/*
 * This was put in to fix the following warning:
 * warning: ISO C forbids conversion of function pointer to object pointer type.
 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
 return (void*)current_vector_pointer;
#pragma GCC diagnostic pop
}

typedef struct {
  uint64_t register_x0;
  uint64_t register_x1;
  uint64_t register_x2;
  uint64_t register_x3;
  uint64_t register_x4;
  uint64_t register_x5;
  uint64_t register_x6;
  uint64_t register_x7;
  uint64_t register_x8;
  uint64_t register_x9;
  uint64_t register_x10;
  uint64_t register_x11;
  uint64_t register_x12;
  uint64_t register_x13;
  uint64_t register_x14;
  uint64_t register_x15;
  uint64_t register_x16;
  uint64_t register_x17;
  uint64_t register_x18;
  uint64_t register_x19;
  uint64_t register_x20;
  uint64_t register_x21;
  uint64_t register_x22;
  uint64_t register_x23;
  uint64_t register_x24;
  uint64_t register_x25;
  uint64_t register_x26;
  uint64_t register_x27;
  uint64_t register_x28;
  uint64_t register_fp;
  void *register_lr;
#ifdef AARCH64_MULTILIB_ARCH_V8_ILP32
  uint32_t _register_lr_top;
#endif
  uintptr_t register_sp;
#ifdef AARCH64_MULTILIB_ARCH_V8_ILP32
  uint32_t _register_sp_top;
#endif
  void *register_pc;
#ifdef AARCH64_MULTILIB_ARCH_V8_ILP32
  uint32_t _register_pc_top;
#endif
  uint64_t register_daif;
  uint64_t register_cpsr;
  uint64_t register_syndrome;
  uint64_t register_fault_address;
  AArch64_symbolic_exception_name vector;
  uint64_t reserved_for_stack_alignment;
  uint64_t register_fpsr;
  uint64_t register_fpcr;
  uint128_t register_q0;
  uint128_t register_q1;
  uint128_t register_q2;
  uint128_t register_q3;
  uint128_t register_q4;
  uint128_t register_q5;
  uint128_t register_q6;
  uint128_t register_q7;
  uint128_t register_q8;
  uint128_t register_q9;
  uint128_t register_q10;
  uint128_t register_q11;
  uint128_t register_q12;
  uint128_t register_q13;
  uint128_t register_q14;
  uint128_t register_q15;
  uint128_t register_q16;
  uint128_t register_q17;
  uint128_t register_q18;
  uint128_t register_q19;
  uint128_t register_q20;
  uint128_t register_q21;
  uint128_t register_q22;
  uint128_t register_q23;
  uint128_t register_q24;
  uint128_t register_q25;
  uint128_t register_q26;
  uint128_t register_q27;
  uint128_t register_q28;
  uint128_t register_q29;
  uint128_t register_q30;
  uint128_t register_q31;
} CPU_Exception_frame;

void _CPU_Exception_frame_print( const CPU_Exception_frame *frame );

RTEMS_NO_RETURN void _CPU_Exception_resume( CPU_Exception_frame *frame );

RTEMS_NO_RETURN void
_CPU_Exception_dispatch_and_resume( CPU_Exception_frame *frame );

void _CPU_Exception_disable_thread_dispatch( void );

int _CPU_Exception_frame_get_signal( CPU_Exception_frame *frame );

void _CPU_Exception_frame_set_resume( CPU_Exception_frame *frame,
                                      void *address );

void _CPU_Exception_frame_make_resume_next_instruction(
  CPU_Exception_frame *frame
);

void _AArch64_Exception_frame_copy(
  CPU_Exception_frame *new_ef,
  CPU_Exception_frame *old_ef
);

void _AArch64_Exception_default( CPU_Exception_frame *frame );

/** Type that can store a 32-bit integer or a pointer. */
typedef uintptr_t CPU_Uint32ptr;

#ifdef __cplusplus
}
#endif

#endif /* ASM */

/** @} */

#endif /* _RTEMS_SCORE_CPU_H */
