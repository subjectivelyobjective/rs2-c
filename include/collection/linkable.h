#ifndef LINKABLE_H
#define LINKABLE_H

#include <stdbool.h>
#include <stdint.h>

typedef struct linkable {
	int64_t id;
	struct linkable *next;
	struct linkable *prev;
	void *ptrvalue;
} linkable;

linkable *linkable_new(void *ptr);

void linkable_unlink(linkable *node);
void linkable_free(linkable *node);

#endif
