#ifndef CMASK_H
#define CMASK_H

// An enum that describes a comparison operation
enum comparison
{
	COMP_LESS_THAN = 0,
	COMP_LESS_THAN_EQ,
	COMP_GREATER_THAN,
	COMP_GREATER_THAN_EQ,
	COMP_EQUAL,
	COMP_NOT_EQUAL
};


void cmask_comp_int32(const char *, int, enum comparison, int *, int *);

#endif