/* sched: a simple scheduler written in C.
 * Copyright (C) 2015 Cyphar
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * 1. The above copyright notice and this permission notice shall be included in
 *    all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/* arch/x86_64.h -- definitions for the x86_64 architecture */

/* only allow one arch/ header to be included */
#if !defined(__ARCH_H__)
#define __ARCH_H__

#if !defined(FRAME_STACK_SIZE)
#	error "FRAME_STACK_SIZE must be defined when including arch/ header files."
#endif

#include <inttypes.h>

#define __asm_state_copy(frame) do { \
		asm volatile( \
		             "movq %%rax, %[rax];" \
		             "movq %%rbx, %[rbx];" \
		             "movq %%rcx, %[rcx];" \
		             "movq %%rdx, %[rdx];" \
		             "movq %%rsi, %[rsi];" \
		             "movq %%rdi, %[rdi];" \
		             "movq %%rbp, %[rbp];" \
		             "movq %%rsp, %[rsp];" \
		             "movq %%r8,  %[r8];" \
		             "movq %%r9,  %[r9];" \
		             "movq %%r10, %[r10];" \
		             "movq %%r11, %[r11];" \
		             "movq %%r12, %[r12];" \
		             "movq %%r13, %[r13];" \
		             "movq %%r14, %[r14];" \
		             "movq %%r15, %[r15];" \
		             "movw %%cs,  %[cs];" \
		             "movw %%ss,  %[ss];" \
		             "movw %%ds,  %[ds];" \
		             "movw %%es,  %[es];" \
		             "movw %%fs,  %[fs];" \
		             "movw %%gs,  %[gs];" \
		             /* copy the stack */ \
		             "movq $0, %%rdx;" \
		             "movq %%rsp, %%;" \
	                 "__stack_copy_loop%=:" \
	                 "	sub %%rdx, %%;" \
	                 "	test %%, %%;" \
	                 "	jne __stack_copy_loop%=" \
		: \
		  [rax] "=m" ((frame)->rax) \
		  [rbx] "=m" ((frame)->rbx) \
		  [rcx] "=m" ((frame)->rcx) \
		  [rdx] "=m" ((frame)->rdx) \
		  [rsi] "=m" ((frame)->rsi) \
		  [rdi] "=m" ((frame)->rdi) \
		  [rbp] "=m" ((frame)->rbp) \
		  [rsp] "=m" ((frame)->rsp) \
		  [r8]  "=m" ((frame)->r8) \
		  [r9]  "=m" ((frame)->r9) \
		  [r10] "=m" ((frame)->r10) \
		  [r11] "=m" ((frame)->r11) \
		  [r12] "=m" ((frame)->r12) \
		  [r13] "=m" ((frame)->r13) \
		  [r14] "=m" ((frame)->r14) \
		  [r15] "=m" ((frame)->r15) \
		  [rip] "=m" ((frame)->rip) \
		  [cs]  "=m" ((frame)->cs) \
		  [ss]  "=m" ((frame)->ss) \
		  [ds]  "=m" ((frame)->ds) \
		  [es]  "=m" ((frame)->es) \
		  [fs]  "=m" ((frame)->fs) \
		  [gs]  "=m" ((frame)->gs) \
		); \
	} while(0)

struct frame_t {
	/* "magic" registers */
	/* we cannot write to %rip (at all) or %cs (safely), we can only ljmp to the code segment and address */
	/* NOTE: These values are messed around with in order to force context switches on tasks, please don't prod this particular sleeping dragon. */
	uint64_t rip;
	uint16_t cs;

	/* purposeful registers */
	uint64_t rax;
	uint64_t rbx;
	uint64_t rcx;
	uint64_t rdx;
	uint64_t rsi;
	uint64_t rdi;
	uint64_t rbp;
	uint64_t rsp;

	/* general purpose registers */
	uint64_t r8;
	uint64_t r9;
	uint64_t r10;
	uint64_t r11;
	uint64_t r12;
	uint64_t r13;
	uint64_t r14;
	uint64_t r15;

	/* cpu flags */
	/* XXX: You can't access this directly, you can only access it by messing with the stack and other registers.
	 *      So do that magic at some point. Goddamn this is cancer. */
	uint64_t rflags;

	/* segment registers */
	uint16_t ss;
	uint16_t ds;
	uint16_t es;
	uint16_t fs;
	uint16_t gs;

	/* stack, populated in reverse LIFO order to the real stack */
	uint64_t stack[FRAME_STACK_SIZE];
}

#define task_yield(task) do { \
		((task))->_state = DEAD; \
		return; \
	} while(0)

#define task_delay(task)

#endif
