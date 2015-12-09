#include "lock.h"
#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <linux/futex.h>
#include <sys/syscall.h>
#include <unistd.h>


#define die(M) 	{perror("error: " M); exit(EXIT_FAILURE);}

long syscall(long number, ...);

static void lock_slowpath(unsigned int*);
static void unlock_slowpath(unsigned int*, unsigned int);


void lock(unsigned int *addr)
{
	if ((int) __sync_fetch_and_add(addr, 1))
		lock_slowpath(addr);
}

void unlock(unsigned int *addr)
{
	unsigned int prev;

	prev = (int) __sync_fetch_and_and(addr, 0);

	if (prev != 1)
		unlock_slowpath(addr, prev);
}

static void lock_slowpath(unsigned int *addr)
{
	while ((int)__sync_val_compare_and_swap(addr, 1, 2)) {
		if (-1 == syscall(__NR_futex, addr, FUTEX_WAIT, 2, NULL))
			die("lock_slowpath");
	}
}

static void unlock_slowpath(unsigned int *addr, unsigned int prev)
{
	if (!prev) {
		fprintf(stderr, "error: cannot unlock sth that's not locked\n");
		exit(EXIT_FAILURE);
	}

	if (-1 == syscall(__NR_futex, addr, FUTEX_WAKE, 1, NULL))
		die("unlock_slowpath");
}
