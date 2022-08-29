#ifndef CMASK_H
#define CMASK_H

#include <stdint.h>

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

// Serial routines for comparisons.
void serial_comp_int32(const int *, int, enum comparison, int *, int *);

// The avx2 routines.
void cmask_comp_lt_int32(const int *, int, int *, int *);
void cmask_comp_gt_int32(const int *, int, int *, int *);
void cmask_comp_le_int32(const int *, int, int *, int *);
void cmask_comp_ge_int32(const int *, int, int *, int *);
void cmask_comp_eq_int32(const int *, int, int *, int *);
void cmask_comp_ne_int32(const int *, int, int *, int *);
void cmask_comp_int32(const int *, int, enum comparison, int *, int *);

// The avx2 routines with packing alongside comparison.
void cmask_pack_comp_lt_int32(const int *, int, int *, int *);
void cmask_pack_comp_gt_int32(const int *, int, int *, int *);
void cmask_pack_comp_le_int32(const int *, int, int *, int *);
void cmask_pack_comp_ge_int32(const int *, int, int *, int *);
void cmask_pack_comp_eq_int32(const int *, int, int *, int *);
void cmask_pack_comp_ne_int32(const int *, int, int *, int *);
void cmask_pack_comp_int32(const int *, int, enum comparison, int *, int *);

// Utilities for generating random data, saving, and loading.
int write_random_data_to_file(int *, int, const char *);
int load_data_from_file(int **, int *, const char *);

#endif
