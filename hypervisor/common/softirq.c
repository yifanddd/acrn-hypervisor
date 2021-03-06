/*
 * Copyright (C) 2018 Intel Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <hypervisor.h>
#include <softirq.h>


static softirq_handler softirq_handlers[NR_SOFTIRQS];

void init_softirq(void)
{
}

/*
 * @pre: nr will not equal or large than NR_SOFTIRQS
 */
void register_softirq(uint16_t nr, softirq_handler handler)
{
	softirq_handlers[nr] = handler;
}

/*
 * @pre: nr will not equal or large than NR_SOFTIRQS
 */
void fire_softirq(uint16_t nr)
{
	bitmap_set_lock(nr, &per_cpu(softirq_pending, get_cpu_id()));
}

void do_softirq(void)
{
	uint16_t nr;
	uint16_t cpu_id = get_cpu_id();
	volatile uint64_t *softirq_pending_bitmap =
			&per_cpu(softirq_pending, cpu_id);

	while (true) {
		nr = ffs64(*softirq_pending_bitmap);
		if (nr >= NR_SOFTIRQS)
			break;

		bitmap_clear_lock(nr, softirq_pending_bitmap);
		(*softirq_handlers[nr])(cpu_id);
	}
}
