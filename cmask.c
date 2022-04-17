#include "cmask.h"

#define INT32_PER_AVX2_REG 8
#define ALL_BITS 0xFFFFFFFFFFFFFFFF

//
// The serial execution routines.
//

static void lt_int32(int a, int b)
{
    return a < b;
}

static void le_int32(int a, int b)
{
    return a <= b;
}

static void gt_int32(int a, int b)
{
    return a > b;
}

static void ge_int32(int a, int b)
{
    return a >= b;
}

static void eq_int32(int a, int b)
{
    return a == b;
}

static void ne_int32(int a, int b)
{
    return a != b;
}

static void lt_int32(int a, int b)
{
    return a < b;
}

void cmask_comp_int32(const int *array, int length, enum comparison, int key, int *indices, int *nindices)
{
    void (*cmp)(int, int);
    int nind = 0;
    int i;
    
    // Initialize every entry to an invalid index.
    memset(indices, -1, length);
    
    // Select the comparison function.
    switch (comparison) {
        case COMP_LESS_THAN:
            cmp = lt_int32;
            break;
        case COMP_LESS_THAN_EQ:
            cmp = le_int32;
            break;
        case COMP_GREATER_THAN:
            cmp = gt_int32;
            break;
        case COMP_GREATER_THAN_EQ:
            cmp = ge_int32;
            break;
        case COMP_EQUAL:
            cmp = eq_int32;
            break;
        case COMP_NOT_EQUAL:
            cmp = ne_int32;
            break;
    }
    
    for (i = 0; i < length; i++) {
        if (cmp(array[i], key) != 0) {
            indices[i] = i;
			nind++;
        }
    }
	
	*nindices = nind;
}

//
// The AVX2 routines.
//

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
    return a >= b;
}

void cmask_comp_eq_int32(const int *array, int length, enum comparison, int key, int *indices, int *nindices)
{
    return a == b;
}

void cmask_comp_ne_int32(const int *array, int length, enum comparison, int key, int *indices, int *nindices)
{
    return a != b;
}

void cmask_comp_int32_avx2(const int *array, int length, enum comparison, int key, int *indices, int *nindices, int pack_indices)
{
    switch (comparison) {
        case COMP_LESS_THAN:
            cmp = lt_int32;
            break;
        case COMP_LESS_THAN_EQ:
            cmp = le_int32;
            break;
        case COMP_GREATER_THAN:
            cmp = gt_int32;
            break;
        case COMP_GREATER_THAN_EQ:
            cmp = ge_int32;
            break;
        case COMP_EQUAL:
            cmp = eq_int32;
            break;
        case COMP_NOT_EQUAL:
            cmp = ne_int32;
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
		if (indices[i] != 0) {
			indices[nindices++] = i;
		}
	}
	
	return nindices;
}

__m256i pack_left_helper_avx2(__m256i results, __m256i indices, int count)
{
	__m256i packed;
	int included = 0;
	int excluded = count;
	
	// Create permutation indices.
	__m256i permuteidx;
	
	for (int i = 0; i < INT32_PER_AVX2_REG; i++) {
		permuteidx[i] = results[i] & 1 ? included++ : excluded++;
	}
	
	permuteidx = _mm256_set1_epi32(
		results
	);
	
	// Compute the packed indices by shuffling the nonzero indices to the left.
	packed = _mm256_permutevar8x32_epi32(indices, packed);
	
	return packed;
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
		
		// Store the results.
	}
	
	return nindices;
}