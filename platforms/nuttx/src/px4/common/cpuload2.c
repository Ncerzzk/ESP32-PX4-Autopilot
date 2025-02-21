/****************************************************************************
 *
 *   Copyright (c) 2012-2016 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/**
 * @file cpuload.c
 *
 * Measurement of CPU load of each individual task.
 *
 * @author Lorenz Meier <lorenz@px4.io>
 * @author Petri Tanskanen <petri.tanskanen@inf.ethz.ch>
 */
#include <px4_platform_common/px4_config.h>
#include <px4_platform_common/atomic.h>
#include <px4_platform/cpuload2.h>

#include <drivers/drv_hrt.h>

#include <nuttx/sched_note.h>
#include "px4_arch/hrt.h"

struct systemload_s systemload;


void cpuload_initialize_once(void)
{
	irqstate_t flags = px4_enter_critical_section();


	for (int i=0;i<CONFIG_FS_PROCFS_MAX_TASKS;i++) {
		systemload.tasks[i].valid = false;
	}

	int static_tasks_count = 3;	// there are at least 2 threads that should be initialized statically - "idle" and "init"

#ifdef CONFIG_PAGING
	static_tasks_count++;	// include paging thread in initialization
#endif /* CONFIG_PAGING */
#if CONFIG_SCHED_WORKQUEUE
	static_tasks_count++;	// include high priority work0 thread in initialization
#endif /* CONFIG_SCHED_WORKQUEUE */
#if CONFIG_SCHED_LPWORK
	static_tasks_count++;	// include low priority work1 thread in initialization
#endif /* CONFIG_SCHED_WORKQUEUE */

	//perform static initialization of "system" threads
	for (systemload.total_count = 0; systemload.total_count < static_tasks_count; systemload.total_count++) {
		systemload.tasks[systemload.total_count].total_runtime = 0;
		systemload.tasks[systemload.total_count].curr_start_time = 0;
		systemload.tasks[systemload.total_count].tcb = nxsched_get_tcb(
					systemload.total_count);	// it is assumed that these static threads have consecutive PIDs
		systemload.tasks[systemload.total_count].valid = true;
	}

	systemload.initialized = true;

	px4_leave_critical_section(flags);
}

void px4_sched_note_start(FAR struct tcb_s *tcb)
{
	//irqstate_t flags = px4_enter_critical_section();

	// if (systemload.initialized) {
	// 	for (int i=0;i<CONFIG_FS_PROCFS_MAX_TASKS;i++) {
	// 		if (!systemload.tasks[i].valid) {
	// 			// slot is available
	// 			systemload.tasks[i].total_runtime = 0;
	// 			systemload.tasks[i].curr_start_time = 0;
	// 			systemload.tasks[i].tcb = tcb;
	// 			systemload.tasks[i].valid = true;
	// 			systemload.total_count++;
	// 			break;
	// 		}
	// 	}
	// }

	//px4_leave_critical_section(flags);
}

void px4_sched_note_stop(FAR struct tcb_s *tcb)
{
	//irqstate_t flags = px4_enter_critical_section();

	// if (systemload.initialized) {
	// 	for (int i=0;i<CONFIG_FS_PROCFS_MAX_TASKS;i++) {
	// 		if (systemload.tasks[i].tcb && systemload.tasks[i].tcb->pid == tcb->pid) {
	// 			// mark slot as free
	// 			systemload.tasks[i].valid = false;
	// 			systemload.tasks[i].total_runtime = 0;
	// 			systemload.tasks[i].curr_start_time = 0;
	// 			systemload.tasks[i].tcb = NULL;
	// 			systemload.total_count--;
	// 			break;
	// 		}
	// 	}
	// }

	//px4_leave_critical_section(flags);
}

void px4_sched_note_suspend(FAR struct tcb_s *tcb)
{
	// //irqstate_t flags = px4_enter_critical_section();

	//if (systemload.initialized) {

	//systemload.tasks[0].total_runtime = hrt_absolute_time_();

	// (*(volatile uint32_t *)(0x3FF60024 + 0x000c)) = 1;
	// systemload.tasks[0].total_runtime =
	// (hrt_abstime)(((uint64_t)(*(volatile uint32_t *)(0x3FF60024 + 0x0008)) << 32) | (uint64_t)(*(volatile uint32_t *)(0x3FF60024 + 0x0004)));

	//rUPDATE = 1;
	//systemload.tasks[0].total_runtime = (hrt_abstime)(((uint64_t)rHI << 32) | (uint64_t)rLO);

	// 	if (tcb->pid == 0) {
	// 		systemload.tasks[0].total_runtime += hrt_elapsed_time_(&systemload.tasks[0].curr_start_time);
	// 		return;

	// 	}
	// 	else {
	// 		// if (cpuload_monitor_all_count.load() == 0) {
	// 		// 	return;
	// 		// }
	// 	}

	// 	for (int i=0;i<CONFIG_FS_PROCFS_MAX_TASKS;i++) {
	// 		// Task ending its current scheduling run
	// 		if (systemload.tasks[i].valid && (systemload.tasks[i].curr_start_time > 0)
	// 		    && systemload.tasks[i].tcb && systemload.tasks[i].tcb->pid == tcb->pid) {
	// 			systemload.tasks[i].total_runtime += hrt_elapsed_time_(&systemload.tasks[i].curr_start_time);
	// 			break;
	// 		}
	// 	}
	// }

	//px4_leave_critical_section(flags);
}

void px4_sched_note_resume(FAR struct tcb_s *tcb)
{
	//irqstate_t flags = px4_enter_critical_section();

	// if (systemload.initialized) {
	// 	if (tcb->pid == 0) {
	// 		hrt_store_absolute_time_(&systemload.tasks[0].curr_start_time);
	// 		return;

	// 	}
	// 	else {
	// 		// if (cpuload_monitor_all_count.load() == 0) {
	// 		// 	return;
	// 		// }
	// 	}

	// 	for (int i=0;i<CONFIG_FS_PROCFS_MAX_TASKS;i++) {
	// 		if (systemload.tasks[i].valid && systemload.tasks[i].tcb && systemload.tasks[i].tcb->pid == tcb->pid) {
	// 			// curr_start_time is accessed from an IRQ handler (in logger), so we need
	// 			// to make the update atomic
	// 			hrt_store_absolute_time_(&systemload.tasks[i].curr_start_time);
	// 			break;
	// 		}
	// 	}
	// }

	//px4_leave_critical_section(flags);
}
