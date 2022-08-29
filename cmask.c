#include "cmask.h"
#include <stdlib.h>

#define INT32_PER_AVX2_REG 8
#define ALL_BITS 0xFFFFFFFFFFFFFFFF
#define BITS_PER_INT32 32
#define COMPARE(a, operator, b) ((a) (operator) (b))

//----------------------------------------------------------------------------
// The serial execution routines.
//----------------------------------------------------------------------------

void serial_comp_lt_int32(const int *array, int length, int key, int *indices, int *nindices, int pack_inline)
{
    int nind = 0;
    int i;
    
    memset(indices, ALL_BITS, length * sizeof(int));
    
    if (pack_inline) {
        for (i = 0; i < length; i++) {
            if (array[i] < key) {
                indices[nind] = i;
                nind++;
            }
        }
    
        *nindices = nind;
    } else {
        for (i = 0; i < length; i++) {
            if (array[i] < key) {
                indices[i] = i;
            }
        }
        
        serial_pack_indices(indices, length, nindices);
    }
}


void serial_pack_indices(int *indices, int length, int *nindices)
{
    int , nind = 0;

    for (i = 0; i < length; i++) {
        if (indices[i] == ALL_BITS) {
            indices[nind] = indices[i];
            
            nind++;
        }
    }
    
    *nindices = nind;
}

//----------------------------------------------------------------------------
// The AVX2 routines.
//----------------------------------------------------------------------------

// Function that packs entries in a register of 4-byte integers.
void avx2_pack_register(__m256i reg)
{
    int count = 0;
}

void avx2_pack_indices(int *indices, int length, int *nindices)
{
    __m256i sequence = _mm256_setr_epi32(0, 1, 2, 3, 4, 5, 6, 7);
    __m256i indices;
    int cutoff = length % INT32_PER_AVX2_REG;
    
    if (cutoff > 0) {
        
    }
    
    for (int i = cutoff; i < length; i += INT32_PER_AVX2_REG) {
        // Load the index array into memory.
        ireg = _mm256_loadu_si256(&indices[i]);
        
        // Find the entries.
    }
}

void cmask_comp_lt_int32(const int *array, int length, enum comparison, int key, int *indices)
{
    int i;
    int cutoff = length % INT32_PER_AVX2_REG;
    __m256i areg;
    __m256i creg;
    __m128i areglo, areghi;
    __m128i creglo, creghi;
    __m128i kreg = _mm_set1_epi32(key);
    __m256i lsmask;
    
    if (cutoff > 0) {
        switch (cutoff) {
            case 1:
                lsmask = __m256_set1_epi32(ALL_BITS, 0, 0, 0, 0, 0, 0, 0);
                break;
            case 2:
                lsmask = __m256_set1_epi32(ALL_BITS, ALL_BITS, 0, 0, 0, 0, 0, 0);
                break;
            case 3:
                lsmask = __m256_set1_epi32(ALL_BITS, ALL_BITS, ALL_BITS, 0, 0, 0,
                                           0, 0);
                break;
            case 4:
                lsmask = __m256_set1_epi32(ALL_BITS, ALL_BITS, ALL_BITS, ALL_BITS,
                                           0, 0, 0, 0);
                break;
            case 5:
                lsmask = __m256_set1_epi32(ALL_BITS, ALL_BITS, ALL_BITS, ALL_BITS,
                                           ALL_BITS, 0, 0, 0);
                break;
            case 6:
                lsmask = __m256_set1_epi32(ALL_BITS, ALL_BITS, ALL_BITS, ALL_BITS,
                                           ALL_BITS, ALL_BITS, 0, 0);
                break;
            case 7:
                lsmask = __m256_set1_epi32(ALL_BITS, ALL_BITS, ALL_BITS, ALL_BITS,
                                           ALL_BITS, ALL_BITS, ALL_BITS, 0);
                break;
        }
        
        // Load the resulting integer values into a register.
        areg = _mm256_maskload_epi32(array, lsmask);
        
        // Split the register into high and low parts.
        areglo = _mm256_extracti128_si256(areg, 0);
        areghi = _mm256_extracti128_si256(areg, 1);
        
        // Compute the comparisons.
        creglo = _mm_cmplt_epi32(areglo, kreg);
        creghi = _mm_cmplt_epi32(areghi, kreg);
        
        // Combine the low and high comparison registers into one.
        creg = _mm256_set_m128i(creglo, creghi);
        
        // Store the results of the comparison.
        _mm256_maskstore_epi32 (&indices[i], lsmask, creg);
    }
    
    for (i = cutoff; i < length; i += INT32_PER_AVX2_REG) {
        // Load the array entries into a register.
        areg = _mm256_loadu_si256(&array[i]);
        
        // Split the register into high and low parts.
        areglo = _mm256_extracti128_si256(areg, 0);
        areghi = _mm256_extracti128_si256(areg, 1);
        
        // Compute the comparisons.
        creglo = _mm_cmplt_epi32(areglo, kreg);
        creghi = _mm_cmplt_epi32(areghi, kreg);
        
        // Combine the low and high comparison registers into one.
        creg = _mm256_set_m128i(creglo, creghi);
        
        // Store the results of the comparison.
        _mm256_store_si256(&indices[i], creg);
    }
}

void cmask_comp_le_int32(const int *array, int length, enum comparison, int key, int *indices)
{
    int i;
    int cutoff = length % INT32_PER_AVX2_REG;
    __m256i areg;
    __m256i creg;
    __m128i areglo, areghi;
    __m128i creglo, creghi;
    __m128i ereglo, ereghi;
    __m128i kreg = _mm_set1_epi32(key);
    __m128i allreg = _mm_set1_epi32(ALL_BITS);
    __m256i lsmask;
    
    if (cutoff > 0) {
        switch (cutoff) {
            case 1:
                lsmask = __m256_set1_epi32(ALL_BITS, 0, 0, 0, 0, 0, 0, 0);
                break;
            case 2:
                lsmask = __m256_set1_epi32(ALL_BITS, ALL_BITS, 0, 0, 0, 0, 0, 0);
                break;
            case 3:
                lsmask = __m256_set1_epi32(ALL_BITS, ALL_BITS, ALL_BITS, 0, 0, 0,
                                           0, 0);
                break;
            case 4:
                lsmask = __m256_set1_epi32(ALL_BITS, ALL_BITS, ALL_BITS, ALL_BITS,
                                           0, 0, 0, 0);
                break;
            case 5:
                lsmask = __m256_set1_epi32(ALL_BITS, ALL_BITS, ALL_BITS, ALL_BITS,
                                           ALL_BITS, 0, 0, 0);
                break;
            case 6:
                lsmask = __m256_set1_epi32(ALL_BITS, ALL_BITS, ALL_BITS, ALL_BITS,
                                           ALL_BITS, ALL_BITS, 0, 0);
                break;
            case 7:
                lsmask = __m256_set1_epi32(ALL_BITS, ALL_BITS, ALL_BITS, ALL_BITS,
                                           ALL_BITS, ALL_BITS, ALL_BITS, 0);
                break;
        }
        
        // Load the resulting integer values into a register.
        areg = _mm256_maskload_epi32(array, lsmask);
        
        // Split the register into high and low parts.
        areglo = _mm256_extracti128_si256(areg, 0);
        areghi = _mm256_extracti128_si256(areg, 1);
        
        // Compute the comparisons for entries strictly less than the key.
        creglo = _mm_cmplt_epi32(areglo, kreg);
        creghi = _mm_cmplt_epi32(areghi, kreg);
        
        // Compute the comparisons for entries equal to the key.
        ereglo = _mm_cmpeq_epi32(areglo, kreg);
        ereghi = _mm_cmpeq_epi32(areghi, kreg);
        
        // "Or" the equality comparisons with the strict inequality comparisons.
        creglo = _mm_or_si128(creglo, ereglo);
        creghi = _mm_or_si128(creghi, ereghi);
        
        // Combine the low and high comparison registers into one.
        creg = _mm256_set_m128i(creglo, creghi);
        
        // Store the results of the comparison.
        _mm256_maskstore_epi32 (&indices[i], lsmask, creg);
    }
    
    for (i = cutoff; i < length; i += INT32_PER_AVX2_REG) {
        // Load the array entries into a register.
        areg = _mm256_loadu_si256(&array[i]);
        
        // Split the register into high and low parts.
        areglo = _mm256_extracti128_si256(areg, 0);
        areghi = _mm256_extracti128_si256(areg, 1);
        
        // Compute the comparisons.
        creglo = _mm_cmplt_epi32(areglo, kreg);
        creghi = _mm_cmplt_epi32(areghi, kreg);
        
        // Compute the comparisons for entries equal to the key.
        ereglo = _mm_cmpeq_epi32(areglo, kreg);
        ereghi = _mm_cmpeq_epi32(areghi, kreg);
        
        // "Or" the equality comparisons with the strict inequality comparisons.
        creglo = _mm_or_si128(creglo, ereglo);
        creghi = _mm_or_si128(creghi, ereghi);
        
        // Combine the low and high comparison registers into one.
        creg = _mm256_set_m128i(creglo, creghi);
        
        // Store the results of the comparison.
        _mm256_store_si256(&indices[i], creg);
    }
}

void cmask_comp_gt_int32(const int *array, int length, enum comparison, int key, int *indices)
{
    int i;
    int cutoff = length % INT32_PER_AVX2_REG;
    __m256i areg;
    __m256i creg;
    __m128i areglo, areghi;
    __m128i creglo, creghi;
    __m128i kreg = _mm_set1_epi32(key);
    __m256i lsmask;
    
    if (cutoff > 0) {
        switch (cutoff) {
            case 1:
                lsmask = __m256_set1_epi32(ALL_BITS, 0, 0, 0, 0, 0, 0, 0);
                break;
            case 2:
                lsmask = __m256_set1_epi32(ALL_BITS, ALL_BITS, 0, 0, 0, 0, 0, 0);
                break;
            case 3:
                lsmask = __m256_set1_epi32(ALL_BITS, ALL_BITS, ALL_BITS, 0, 0, 0,
                                           0, 0);
                break;
            case 4:
                lsmask = __m256_set1_epi32(ALL_BITS, ALL_BITS, ALL_BITS, ALL_BITS,
                                           0, 0, 0, 0);
                break;
            case 5:
                lsmask = __m256_set1_epi32(ALL_BITS, ALL_BITS, ALL_BITS, ALL_BITS,
                                           ALL_BITS, 0, 0, 0);
                break;
            case 6:
                lsmask = __m256_set1_epi32(ALL_BITS, ALL_BITS, ALL_BITS, ALL_BITS,
                                           ALL_BITS, ALL_BITS, 0, 0);
                break;
            case 7:
                lsmask = __m256_set1_epi32(ALL_BITS, ALL_BITS, ALL_BITS, ALL_BITS,
                                           ALL_BITS, ALL_BITS, ALL_BITS, 0);
                break;
        }
        
        // Load the resulting integer values into a register.
        areg = _mm256_maskload_epi32(array, lsmask);
        
        // Split the register into high and low parts.
        areglo = _mm256_extracti128_si256(areg, 0);
        areghi = _mm256_extracti128_si256(areg, 1);
        
        // Compute the comparisons.
        creglo = _mm_cmpgt_epi32(areglo, kreg);
        creghi = _mm_cmpgt_epi32(areghi, kreg);
        
        // Combine the low and high comparison registers into one.
        creg = _mm256_set_m128i(creglo, creghi);
        
        // Store the results of the comparison.
        _mm256_maskstore_epi32(&indices[i], lsmask, creg);
    }
    
    for (i = cutoff; i < length; i += INT32_PER_AVX2_REG) {
        // Load the array entries into a register.
        areg = _mm256_loadu_si256(&array[i]);
        
        // Split the register into high and low parts.
        areglo = _mm256_extracti128_si256(areg, 0);
        areghi = _mm256_extracti128_si256(areg, 1);
        
        // Compute the comparisons.
        creglo = _mm_cmpgt_epi32(areglo, kreg);
        creghi = _mm_cmpgt_epi32(areghi, kreg);
        
        // Combine the low and high comparison registers into one.
        creg = _mm256_set_m128i(creglo, creghi);
        
        // Store the results of the comparison.
        _mm256_store_si256(&indices[i], creg);
    }
}

void cmask_comp_ge_int32(const int *array, int length, enum comparison, int key, int *indices, int *nindices)
{
    int i;
    int cutoff = length % INT32_PER_AVX2_REG;
    __m256i areg;
    __m256i creg;
    __m128i areglo, areghi;
    __m128i creglo, creghi;
    __m128i ereglo, ereghi;
    __m128i kreg = _mm_set1_epi32(key);
    __m128i allreg = _mm_set1_epi32(ALL_BITS);
    __m256i lsmask;
    
    if (cutoff > 0) {
        switch (cutoff) {
            case 1:
                lsmask = __m256_set1_epi32(ALL_BITS, 0, 0, 0, 0, 0, 0, 0);
                break;
            case 2:
                lsmask = __m256_set1_epi32(ALL_BITS, ALL_BITS, 0, 0, 0, 0, 0, 0);
                break;
            case 3:
                lsmask = __m256_set1_epi32(ALL_BITS, ALL_BITS, ALL_BITS, 0, 0, 0,
                                           0, 0);
                break;
            case 4:
                lsmask = __m256_set1_epi32(ALL_BITS, ALL_BITS, ALL_BITS, ALL_BITS,
                                           0, 0, 0, 0);
                break;
            case 5:
                lsmask = __m256_set1_epi32(ALL_BITS, ALL_BITS, ALL_BITS, ALL_BITS,
                                           ALL_BITS, 0, 0, 0);
                break;
            case 6:
                lsmask = __m256_set1_epi32(ALL_BITS, ALL_BITS, ALL_BITS, ALL_BITS,
                                           ALL_BITS, ALL_BITS, 0, 0);
                break;
            case 7:
                lsmask = __m256_set1_epi32(ALL_BITS, ALL_BITS, ALL_BITS, ALL_BITS,
                                           ALL_BITS, ALL_BITS, ALL_BITS, 0);
                break;
        }
        
        // Load the resulting integer values into a register.
        areg = _mm256_maskload_epi32(array, lsmask);
        
        // Split the register into high and low parts.
        areglo = _mm256_extracti128_si256(areg, 0);
        areghi = _mm256_extracti128_si256(areg, 1);
        
        // Compute the comparisons for entries strictly less than the key.
        creglo = _mm_cmpgt_epi32(areglo, kreg);
        creghi = _mm_cmpgt_epi32(areghi, kreg);
        
        // Compute the comparisons for entries equal to the key.
        ereglo = _mm_cmpeq_epi32(areglo, kreg);
        ereghi = _mm_cmpeq_epi32(areghi, kreg);
        
        // "Or" the equality comparisons with the strict inequality comparisons.
        creglo = _mm_or_si128(creglo, ereglo);
        creghi = _mm_or_si128(creghi, ereghi);
        
        // Combine the low and high comparison registers into one.
        creg = _mm256_set_m128i(creglo, creghi);
        
        // Store the results of the comparison.
        _mm256_maskstore_epi32 (&indices[i], lsmask, creg);
    }
    
    for (i = cutoff; i < length; i += INT32_PER_AVX2_REG) {
        // Load the array entries into a register.
        areg = _mm256_loadu_si256(&array[i]);
        
        // Split the register into high and low parts.
        areglo = _mm256_extracti128_si256(areg, 0);
        areghi = _mm256_extracti128_si256(areg, 1);
        
        // Compute the comparisons.
        creglo = _mm_cmpgt_epi32(areglo, kreg);
        creghi = _mm_cmpgt_epi32(areghi, kreg);
        
        // Compute the comparisons for entries equal to the key.
        ereglo = _mm_cmpeq_epi32(areglo, kreg);
        ereghi = _mm_cmpeq_epi32(areghi, kreg);
        
        // "Or" the equality comparisons with the strict inequality comparisons.
        creglo = _mm_or_si128(creglo, ereglo);
        creghi = _mm_or_si128(creghi, ereghi);
        
        // Combine the low and high comparison registers into one.
        creg = _mm256_set_m128i(creglo, creghi);
        
        // Store the results of the comparison.
        _mm256_store_si256(&indices[i], creg);
    }
}

void cmask_comp_eq_int32(const int *array, int length, enum comparison, int key, int *indices, int *nindices)
{
    int i;
    int cutoff = length % INT32_PER_AVX2_REG;
    __m256i areg;
    __m256i creg;
    __m128i areglo, areghi;
    __m128i creglo, creghi;
    __m128i ereglo, ereghi;
    __m128i kreg = _mm_set1_epi32(key);
    __m128i allreg = _mm_set1_epi32(ALL_BITS);
    __m256i lsmask;
    
    if (cutoff > 0) {
        switch (cutoff) {
            case 1:
                lsmask = __m256_set1_epi32(ALL_BITS, 0, 0, 0, 0, 0, 0, 0);
                break;
            case 2:
                lsmask = __m256_set1_epi32(ALL_BITS, ALL_BITS, 0, 0, 0, 0, 0, 0);
                break;
            case 3:
                lsmask = __m256_set1_epi32(ALL_BITS, ALL_BITS, ALL_BITS, 0, 0, 0,
                                           0, 0);
                break;
            case 4:
                lsmask = __m256_set1_epi32(ALL_BITS, ALL_BITS, ALL_BITS, ALL_BITS,
                                           0, 0, 0, 0);
                break;
            case 5:
                lsmask = __m256_set1_epi32(ALL_BITS, ALL_BITS, ALL_BITS, ALL_BITS,
                                           ALL_BITS, 0, 0, 0);
                break;
            case 6:
                lsmask = __m256_set1_epi32(ALL_BITS, ALL_BITS, ALL_BITS, ALL_BITS,
                                           ALL_BITS, ALL_BITS, 0, 0);
                break;
            case 7:
                lsmask = __m256_set1_epi32(ALL_BITS, ALL_BITS, ALL_BITS, ALL_BITS,
                                           ALL_BITS, ALL_BITS, ALL_BITS, 0);
                break;
        }
        
        // Load the resulting integer values into a register.
        areg = _mm256_maskload_epi32(array, lsmask);
        
        // Split the register into high and low parts.
        areglo = _mm256_extracti128_si256(areg, 0);
        areghi = _mm256_extracti128_si256(areg, 1);
        
        // Compute the comparisons for entries equal to the key.
        creglo = _mm_cmpeq_epi32(areglo, kreg);
        creghi = _mm_cmpeq_epi32(areghi, kreg);
        
        // Combine the low and high comparison registers into one.
        creg = _mm256_set_m128i(creglo, creghi);
        
        // Store the results of the comparison.
        _mm256_maskstore_epi32 (&indices[i], lsmask, creg);
    }
    
    for (i = cutoff; i < length; i += INT32_PER_AVX2_REG) {
        // Load the array entries into a register.
        areg = _mm256_loadu_si256(&array[i]);
        
        // Split the register into high and low parts.
        areglo = _mm256_extracti128_si256(areg, 0);
        areghi = _mm256_extracti128_si256(areg, 1);
        
        // Compute the comparisons for entries equal to the key.
        creglo = _mm_cmpeq_epi32(areglo, kreg);
        creghi = _mm_cmpeq_epi32(areghi, kreg);
        
        // Combine the low and high comparison registers into one.
        creg = _mm256_set_m128i(creglo, creghi);
        
        // Store the results of the comparison.
        _mm256_store_si256(&indices[i], creg);
    }
}

void cmask_comp_ne_int32(const int *array, int length, enum comparison, int key, int *indices, int *nindices)
{
    int i;
    int cutoff = length % INT32_PER_AVX2_REG;
    __m256i areg;
    __m256i creg;
    __m128i areglo, areghi;
    __m128i creglo, creghi;
    __m128i ereglo, ereghi;
    __m128i kreg = _mm_set1_epi32(key);
    __m128i allreg = _mm_set1_epi32(ALL_BITS);
    __m256i lsmask;
    
    if (cutoff > 0) {
        switch (cutoff) {
            case 1:
                lsmask = __m256_set1_epi32(ALL_BITS, 0, 0, 0, 0, 0, 0, 0);
                break;
            case 2:
                lsmask = __m256_set1_epi32(ALL_BITS, ALL_BITS, 0, 0, 0, 0, 0, 0);
                break;
            case 3:
                lsmask = __m256_set1_epi32(ALL_BITS, ALL_BITS, ALL_BITS, 0, 0, 0,
                                           0, 0);
                break;
            case 4:
                lsmask = __m256_set1_epi32(ALL_BITS, ALL_BITS, ALL_BITS, ALL_BITS,
                                           0, 0, 0, 0);
                break;
            case 5:
                lsmask = __m256_set1_epi32(ALL_BITS, ALL_BITS, ALL_BITS, ALL_BITS,
                                           ALL_BITS, 0, 0, 0);
                break;
            case 6:
                lsmask = __m256_set1_epi32(ALL_BITS, ALL_BITS, ALL_BITS, ALL_BITS,
                                           ALL_BITS, ALL_BITS, 0, 0);
                break;
            case 7:
                lsmask = __m256_set1_epi32(ALL_BITS, ALL_BITS, ALL_BITS, ALL_BITS,
                                           ALL_BITS, ALL_BITS, ALL_BITS, 0);
                break;
        }
        
        // Load the resulting integer values into a register.
        areg = _mm256_maskload_epi32(array, lsmask);
        
        // Split the register into high and low parts.
        areglo = _mm256_extracti128_si256(areg, 0);
        areghi = _mm256_extracti128_si256(areg, 1);
        
        // Compute the comparisons for entries equal to the key.
        creglo = _mm_cmpeq_epi32(areglo, kreg);
        creghi = _mm_cmpeq_epi32(areghi, kreg);
        
        // Since we want unequal comparisons, XOR the registers of equality
        // comparisons.
        creglo = _mm_xor_si128(creglo, kreg);
        creghi = _mm_xor_si128(creghi, kreg);
        
        // Combine the low and high comparison registers into one.
        creg = _mm256_set_m128i(creglo, creghi);
        
        // Store the results of the comparison.
        _mm256_maskstore_epi32 (&indices[i], lsmask, creg);
    }
    
    for (i = cutoff; i < length; i += INT32_PER_AVX2_REG) {
        // Load the array entries into a register.
        areg = _mm256_loadu_si256(&array[i]);
        
        // Split the register into high and low parts.
        areglo = _mm256_extracti128_si256(areg, 0);
        areghi = _mm256_extracti128_si256(areg, 1);
        
        // Compute the comparisons for entries equal to the key.
        creglo = _mm_cmpeq_epi32(areglo, kreg);
        creghi = _mm_cmpeq_epi32(areghi, kreg);
        
        // Since we want unequal comparisons, XOR the registers of equality
        // comparisons.
        creglo = _mm_xor_si128(creglo, kreg);
        creghi = _mm_xor_si128(creghi, kreg);
        
        // Combine the low and high comparison registers into one.
        creg = _mm256_set_m128i(creglo, creghi);
        
        // Store the results of the comparison.
        _mm256_store_si256(&indices[i], creg);
    }
}

void cmask_comp_int32_avx2(const int *array, int length, enum comparison, int key, int *indices, int *nindices, int pack_indices)
{
    switch (comparison) {
        case COMP_LESS_THAN:
            cmask_comp_lt_int32(array, length, key, indices, nindices);
            break;
        case COMP_LESS_THAN_EQ:
            cmask_comp_le_int32(array, length, key, indices, nindices);
            break;
        case COMP_GREATER_THAN:
            cmask_comp_gt_int32(array, length, key, indices, nindices);
            break;
        case COMP_GREATER_THAN_EQ:
            cmask_comp_ge_int32(array, length, key, indices, nindices);
            break;
        case COMP_EQUAL:
            cmask_comp_eq_int32(array, length, key, indices, nindices);
            break;
        case COMP_NOT_EQUAL:
            cmask_comp_ne_int32(array, length, key, indices, nindices);
            break;
    }
    
    if (pack_indices) {
        pack_indices_avx2(indices, length);
    }
}

void pack_indices(int *indices, int length)
{
    int nindices = 0;
    
    for (int i = 0; i < length; i++) {
        if (indices[i] == ALL_BITS) {
            indices[nindices] = i;
            
            nindices++;
        }
    }
    
    return nindices;
}

int count_nonzero(__m256i results)
{
    int ones = _mm256_set1_epi32(1);
    __m256i hsum;
    __m256i permuteidx = _mm256_setr_epi32(0, 1, 4, 5, 2, 3, 6, 7);
    
    // Count the number of entries in the index.
    results = _mm256_and_epi32(results, ones);
    
    // Compute the initial horizontal sum that computes
    // hsum = [r0 + r1, r2 + r3,
    //         r0 + r1, r2 + r3,
    //         r4 + r5, r6 + r7,
    //         r4 + r5, r6 + r7]
    hsum = _mm256_hadd_epi32(results, results);
    
    // Swap the entries of hsum to contain
    // hsum = [r0 + r1, r2 + r3,
    //         r4 + r5, r6 + r7,
    //         r0 + r1, r2 + r3,
    //         r4 + r5, r6 + r7]
    hsum = _mm256_permutevar8x32_epi32(hsum, permuteidx);
    
    // Compute the next horizontal sum that computes
    // hsum = [r0 + r1 + r2 + r3, r4 + r5 + r6 + r7,
    //         r0 + r1 + r2 + r3, r4 + r5 + r6 + r7,
    //         r0 + r1 + r2 + r3, r4 + r5 + r6 + r7,
    //         r0 + r1 + r2 + r3, r4 + r5 + r6 + r7]
    hsum = _mm256_hadd_epi32(hsum, hsum);
    
    // Compute the final horizontal sum that computes
    // hsum = [r0 + r1 + r2 + r3 + r4 + r5 + r6 + r7,
    //         r0 + r1 + r2 + r3 + r4 + r5 + r6 + r7,
    //         ...
    //         r0 + r1 + r2 + r3 + r4 + r5 + r6 + r7]
    hsum = _mm256_hadd_epi32(hsum, hsum);
    
    return _mm256_extract_epi32(hsum, 0);
}


// pack_indices_avx2()
//
// Function that packs an array of indices tightly into the same array.
// Returns the number of valid indices that point
int pack_indices_avx2(int *results, int length)
{
    int nindices = 0;
    __m256i creg = _mm256_setr_epi32(0, 1, 2, 3, 4, 5, 6, 7);
    __m256i breg;
    __m256i rreg;
    __m256i ireg;
    __m256i onesreg = _mm256_set1_epi32(1);
    
    for (int i = 0; i < n; i += INT32_PER_AVX2_REG) {
        // Load the results of the comparison.
        rreg = _mm256_loadu_si256(&results[i]);
        
        // Compute the current list of indices.
        breg = _mm256_set1_epi32(i);
        creg = _mm256_add_epi32(creg, breg);
        
        // Keep the indices corresponding to nonzero results.
        ireg = _mm256_and_epi32(creg, rreg);
        
        // Pack the indices to the left.
        rreg = pack_left_helper_avx2(ireg, creg);
        
        // Store the results.
    }
    
    return nindices;
}

__m256i pack_left_helper_avx2(__m256i results, __m256i indices)
{
    __m256i packed;
    int included = 0; // Number of valid indices.
    int excluded = 0; // Number of invalid indices.
    
    // Create permutation indices.
    __m256i permuteidx;
    
    for (int i = 0; i < INT32_PER_AVX2_REG; i++) {
        permuteidx[i] = results[i] & ALL_BITS ? included++ : excluded++;
    }
    
    // Compute the packed indices by shuffling the nonzero indices to the left.
    packed = _mm256_permutevar8x32_epi32(indices, permuteidx);
    
    return packed;
}

//----------------------------------------------------------------------------
// The avx2 routines with packing alongside comparison.
//----------------------------------------------------------------------------

void cmask_pack_comp_lt_int32(const int *, int, int *, int *);
void cmask_pack_comp_gt_int32(const int *, int, int *, int *);
void cmask_pack_comp_le_int32(const int *, int, int *, int *);
void cmask_pack_comp_ge_int32(const int *, int, int *, int *);
void cmask_pack_comp_eq_int32(const int *, int, int *, int *);
void cmask_pack_comp_ne_int32(const int *, int, int *, int *);
void cmask_pack_comp_int32(const int *, int, enum comparison, int *, int *);

//----------------------------------------------------------------------------
// Utilities for generating random data, saving, and loading.
//----------------------------------------------------------------------------

int write_random_data_to_file(int *values, int length, const char *filename)
{
    FILE *outfile = fopen(filename, "wb");
    int write_status = outfile != NULL;
    int i, coeff, temp;
    
    if (write_status) {
        for (i = 0; i < length; i++) {
            temp = 0;
            
            for (j = 0; j < BITS_PER_INT32; j++) {
                temp += (rand() % 2) * (2 << j)
            }
            
            values[i] = temp;
        }
        
        fwrite(&length, sizeof(int), 1, outfile);
        fwrite(values, sizeof(int), length, outfile);
        fclose(outfile);    
    }
    
    return write_status;
}

int load_data_from_file(int **values, int *length, const char *filename)
{
    FILE *infile = fopen(filename, "rb");
    int read_status = outfile != NULL;
    
    if (read_status) {
        fread(length, sizeof(int), 1, infile);
        
        *values = malloc((*length) * sizeof(int));
        read_status = read_status && (*values != NULL);
        
        if (read_status) {
            fread(*values, sizeof(int), *length, infile);
        }
        
        fclose(infile);
    }
    
    return read_status;
}
