#include "bignum.h"

#ifdef USE_LOGGER
const struct log_t BN_LOG = {BN_LOG_LOCATION, BN_LOG_LEVEL};
#endif

/* The struct containing the bignum data type. The explicit data type cannot be
 * referenced outside this file. We only expose the typedef bignum* bn_t. */
struct bignum {
    uint32_t noBlocks; //The number of bytes that the number
                       //is composed of
    uint8_t* blocks;   //The bytes of the number itself,
                       //least significant bytes first.
    enum sign {        //The sign of the number. We just store
        BN_POSITIVE,   //the sign explicilty rather than working
        BN_NEGATIVE    //with 2s complement on the blocks.
    } sign;
};

/* Constants representing 0 and 1, which are often useful. */
static uint8_t nums0[] = {0};
static struct bignum ZERO = {1, nums0 , BN_POSITIVE};
static uint8_t nums1[] = {1};
static struct bignum ONE = {1, nums1, BN_POSITIVE};
const bn_t BN_ZERO = &ZERO;
const bn_t BN_ONE = &ONE;

/* Initialise the bignum. This must be called before using the bignum.
 * ----------------------------------------------------------------------------
 * bn_t *num -- pointer to the bignum to be initialised.
 *
 * return    -- 1 if the bignum was successfully initialised, 0 otherwise.
 */
int8_t bn_init(bn_t *num) {

    /* Alloc the struct. */
    errno = 0;
    *num = malloc(sizeof(struct bignum));
    if(!num || errno) {
        /* If we couldn't alloc, return as an error. */
        LOG(BN_LOG, LOG_LEVEL_CRIT, "Could not alloc for a bignum.");
        return 0;
    }

    /* Set the initial values for the bignum. */
    (*num)->noBlocks = 0;
    (*num)->blocks = NULL;
    (*num)->sign = BN_POSITIVE;

    /* Return success. */
    return 1;
}

/* Initialise multiple bignums at once.
 * ----------------------------------------------------------------------------
 * int num -- The number of bignums to initialise.
 * ...     -- A list of `num' pointers to bignums.
 *
 * return  -- 1 if all bignums were successfully initialised, 0 otherwise.
 */
int8_t bn_inits(int num, ...) {
    /* Retrieve the pointers. */
    va_list nums;
    va_start(nums, num);

    for(int i=0; i<num; i++) {
        /* Try to initialise each bignum. */
        if(!bn_init(va_arg(nums, bn_t *))) {
            /* If we can't initialise a bignum, return failure. */
            va_end(nums);
            return 0;
        }
    }

    /* Cleanup and return success. */
    va_end(nums);
    return 1;
}

/* All the following functions expect the bignums passed as arguments have been
 * initialised. If they have not, random errors will occur. */

/* Frees the blocks of the bignum. The bignum remains initialised, so can be
 * used again for any given function.
 * ----------------------------------------------------------------------------
 * bn_t num -- The number to free the blocks of.
 */
void bn_free(bn_t num) {
    if(num->blocks) free(num->blocks);
    num->noBlocks = 0;
    num->blocks = NULL;
}
void bn_destroy(bn_t num) { bn_free(num); }

/* Deinitialises the bignum. It can't be used again after this is called,
 * unless it is reinitialised again with bn_init(s).
 * ----------------------------------------------------------------------------
 * bn_t *num -- A pointer to the number to deinitialise.
 */
void bn_deinit(bn_t *num) {
    if(*num) {
        bn_free(*num);
        free(*num);
    }
    *num = NULL;
}
void bn_nuke(bn_t *num) { bn_deinit(num); }

/* Deinitialises multiple bignums at once.
 * ----------------------------------------------------------------------------
 * int num -- The number of bignums to deinitialise.
 * ...     -- A list of `num' pointers to bignums to deinitialise.
 */
void bn_deinits(int num, ...) {
    va_list nums;
    va_start(nums, num);
    for(int i=0; i<num; i++) {
        bn_deinit(va_arg(nums, bn_t *));
    }
    va_end(nums);
}
void bn_nukes(int num, ...) {
    va_list nums;
    va_start(nums, num);
    for(int i=0; i<num; i++) {
        bn_deinit(va_arg(nums, bn_t *));
    }
    va_end(nums);
}

/* Resize the bignum, so that the total number of blocks is as given.
 * ----------------------------------------------------------------------------
 * bn_t num          -- The number to be resized.
 * uint32_t noBlocks -- The total number of blocks that num will have.
 *
 * return            -- 1 if the bignum was successfully resized, 0 otherwise.
 */
int8_t bn_resize(bn_t num, uint32_t noBlocks) {

    /* If we want to resize to 0 blocks, just free the blocks. */
    if(noBlocks == 0) {
        bn_free(num);
        return 1;
    }

    /* If the new size is less than the old size, don't realloc, just reduce
     * the number of blocks. */
    if(noBlocks <= num->noBlocks) {
        num->noBlocks = noBlocks;
        return 1;
    }

    /* Try to resize the blocks. */
    uint8_t *new_blocks = realloc(num->blocks, noBlocks);
    if(!new_blocks) {
        /* If the call to realloc returned NULL, an error must have occured.
         * Note that in this case, num->blocks should remain unchanged. */
        LOG(BN_LOG, LOG_LEVEL_CRIT, "Could not resize bignum.");
        return 0;
    }

    /* Copy the new pointer (it's probably the same actually). */
    num->blocks = new_blocks;

    /* Make sure all the new blocks are set to 0. */
    memset(num->blocks + num->noBlocks, 0, noBlocks - num->noBlocks);

    /* Update the number of blocks in the number. */
    num->noBlocks = noBlocks;

    return 1;
}

/* Performs a deep copy of the second variable into the first variable.
 * ----------------------------------------------------------------------------
 * bn_t new       -- The bignum to copy into.
 * const bn_t old -- The bignum to copy from.
 *
 * return         -- 1 if the clone was successful, 0 otherwise.
 */
int8_t bn_clone(bn_t new, const bn_t old) {
    /* If the two nums are the same (i.e. they are pointers to the same object)
     * then there is no need to copy. */
    if(new == old) return 1;

    /* If there is nothing in the old bignum, just empty the new bignum. */
    if(bn_isempty(old)) {
        bn_free(new);
        return 1;
    }

    /* Try to copy the blocks. */
    if(!bn_resize(new, old->noBlocks)) {
        /* If we couldn't resize, return failure. */
        return 0;
    }
    memcpy(new->blocks, old->blocks, old->noBlocks);
    new->sign = old->sign;
    return 1;
}

/* Set the number to be zero.
 * ----------------------------------------------------------------------------
 * bn_t num -- The number to set to zero.
 *
 * return   -- 1 if the copy was successful, 0 otherwise.
 */
int8_t bn_setzero(bn_t num) {
    return bn_clone(num, BN_ZERO);
}

/* Swap the numbers in the two variables. We should be able to deprecate this,
 * and just swap the numbers in place, as they are all just pointers.
 * ----------------------------------------------------------------------------
 * bn_t in1 -- The number to swap with the other.
 * bn_t in2 --               -"-
 */
void bn_swap(bn_t in1, bn_t in2) {
    uint32_t tempLen = in1->noBlocks;
    in1->noBlocks = in2->noBlocks;
    in2->noBlocks = tempLen;
    enum sign tempSign = in1->sign;
    in1->sign = in2->sign;
    in2->sign = tempSign;
    uint8_t *tempBlocks = in1->blocks;
    in1->blocks = in2->blocks;
    in2->blocks = tempBlocks;
}

/* Check if the given number is empty. A bignum is empty if it is NULL, its
 * blocks are NULL, or it has no blocks.
 * ----------------------------------------------------------------------------
 * const bn_t num -- The number to check is empty.
 *
 * return         -- 1 if the number is empty, 0 otherwise.
 */
int8_t bn_isempty(const bn_t num) {
    return (!num || num->noBlocks == 0 || !num->blocks) ? 1 : 0;
}

/* Check if the given number is 0. A bignum is 0 if either it is empty, or it
 * has no non-zero blocks.
 * ----------------------------------------------------------------------------
 * const bn_t num -- The number to check is zero.
 *
 * return         -- 1 if the number is 0, 0 otherwise.
 */
int8_t bn_iszero(const bn_t num) {
    if(bn_isempty(num)) return 1;
    for(int i=0; i < num->noBlocks; i++) {
        if(num->blocks[i] != 0) return 0;
    }
    return 1;
}

/* Set the given number to have the given number of blocks, each with random
 * entries.
 * ----------------------------------------------------------------------------
 * bn_t num          -- The number to randomize.
 * uint32_t noBlocks -- The number of blocks in the randomized number.
 *
 * return            -- 1 if we successfully generated a random number, 0
 *                      otherwise.
 */
int8_t bn_rand_blocks(bn_t num, uint32_t noBlocks) {
    /* If the number wants to have 0 blocks, is must be empty. */
    if(noBlocks == 0) {
        bn_free(num);
        return 1;
    }

    /* Try to resize the number with the given number of blocks. */
    if(!bn_resize(num, noBlocks)) {
        /* If we couldn't resize the number, return failure. */
        return 0;
    }

    /* Fill the blocks with random numbers. */
    for(uint32_t i=0; i<num->noBlocks; i++) {
        num->blocks[i] = rand()%256;
    }

    /* Remove any leading zeros. This may mean the length is less than
     * the requested length. */
    bn_removezeros(num);

    return 1;
}

/* Set the output to be a randomly generated number less than the input.
 * ----------------------------------------------------------------------------
 * bn_t out      -- The resulting random number.
 * const bn_t in -- The number the output must be less than.
 *
 * return        -- 1 if we successfully generated a random number, 0 otherwise
 */
int8_t bn_rand(bn_t out, const bn_t in) {

    /* If the bound is empty, the result must be empty. */
    if(bn_isempty(in)) {
        bn_free(out);
        return 1;
    }

    /* If the bound is 0, the result must be 0. */
    if(bn_iszero(in)) {
        bn_setzero(out);
        return 1;
    }

    /* Resize the output to be the same length as the input. */
    uint32_t len = bn_trueLength(in);
    if(!bn_resize(out, len)) {
        /* If we couldn't resize, return failure. */
        return 0;
    }

    int8_t newIsSmaller = 0;
    uint8_t newByte;

    /* Randomly generate the blocks from msb to lsb. */
    for(uint32_t i=0; i<len; i++) {
        if(!newIsSmaller) {
            /* If the new number is not yet less than the bound, generate
             * a block less than or equal to the block in the bound. */
            newByte = rand()%((in->blocks[len-i])+1);

            if(newByte < in->blocks[len-i])
                /* If the new block is strictly less than the block in the
                 * bound, the new number is less than the bound. */
                newIsSmaller = 1;
        } else {
            /* Otherwise, generate any random byte. */
            newByte = rand()%256;
        }
        out->blocks[len-i] = newByte;
    }

    /* Remove any leading zeros. */
    bn_removezeros(out);

    return 1;
}

/* Adds one block to the given number, which is the new msb, and is initialised
 * to 0.
 * ----------------------------------------------------------------------------
 * bn_t num -- The number to add a new msb to.
 *
 * return   -- 1 if a new msb was successfully added, 0 otherwise.
 */
int8_t inline bn_addblock(bn_t num) {
    return bn_resize(num, num->noBlocks+1);
}

/* Adds a number of blocks as msb's to the given number, all of which are
 * initialised as 0.
 * ----------------------------------------------------------------------------
 * bn_t num          -- The number to add new msb's to.
 * uint32_t noBlocks -- The number of msb's to add.
 *
 * return            -- 1 if the new msb's were added, 0 otherwise.
 */
int8_t inline bn_addblocks(bn_t num, uint32_t noBlocks) {
    return bn_resize(num, num->noBlocks+noBlocks);
}

/* Shifts the number by the given number of blocks. That is multiplies or
 * divides the number by 256 the given number of times. If `amount' is positive
 * the number is left shifted, i.e. multiplied by 256, and if `amount' is
 * negative, the number is right shifted, i.e. (integer) divided by 256.
 * ----------------------------------------------------------------------------
 * bn_t num       -- The number whose blocks to shift.
 * int32_t amount -- The number of blocks to shift. If positive, multiply, and
 *                   if negative divide.
 *
 * return         -- 1 if succesful, 0 otherwise.
 */
int8_t bn_blockshift(bn_t num, int32_t amount) {
    /* If the amount to shift is 0, or the input is empty,
     * there is nothin to move. */
    if(amount == 0 || bn_isempty(num))
        return 1;

    /* If the amount is negative, and larger than the number of blocks,
     * shifting will make the result empty. */
    if(amount < 0 && -amount >= num->noBlocks) {
        bn_free(num);
        return 1;
    }

    /* If the number has no non-zero blocks, just change the length. */
    if(bn_iszero(num)) {
        return bn_resize(num, num->noBlocks + amount);
    }

    if(amount<0) {
        /* If we are right shifting, move everything down the given amount. */
        num->noBlocks += amount;  //Note amount is -ve, so this is subtracting.
        num->blocks = memmove(num->blocks, num->blocks-amount, num->noBlocks);
    } else {

        /* If we are left shifting, increase the length. */
        if(!bn_resize(num, num->noBlocks+amount)) {
            /* If we can't resize, return failure. */
            return 0;
        }
        /* Move the lsb to the msb, and set the lsb to 0. */
        memmove(num->blocks + amount, num->blocks, num->noBlocks - amount);
        memset(num->blocks, 0, amount);
    }

    return 1;
}

/* Copy the `length' lsb of `num' into `out'. For example, if `num' were
 * {1,2,3,4}, so 1 is the lsb, and `length' were 2, then out would be {1,2}.
 * If `length' is more than the length of `num', the whole number is copied.
 * ----------------------------------------------------------------------------
 * const bn_t num  -- The number whose lower blocks are to be copied.
 * uint32_t length -- The number of lower blocks to copy from `num'
 * bn_t out        -- The number in which the lower blocks of `num' are stored.
 *
 * return          -- 1 if the lower blocks were copied, 0 otherwise.
 */
int8_t bn_lowerblocks(const bn_t num, uint32_t length, bn_t out) {
    /* If num is empty, or the number of blocks to copy is 0, the output
     * should be empty too. */
    if(bn_isempty(num) || length == 0) {
        bn_free(out);
        return 1;
    }

    /* Copy the whole of the input into the output. */
    if(!bn_clone(out, num)) {
        /* If we couldn't copy, return failure. */
        return 0;
    }

    /* If the length of the output is longer than the number of blocks
     * required, just chop of the higher blocks. */
    if(length<num->noBlocks)
        out->noBlocks = length;

    return 1;
}

/* Copy the `length' number of bytes, starting from `index' (little-endian),
 * into `out'. For example, if `num' were {1,2,3,4}, `length' were 2, and
 * `index' were 1, then `out' would be {2,3}. If the index is out of range, 0
 * is copied, and if index + length is out of bounds, it is just returned
 * up to the end. Note bn_lowerblocks(length) = bn_innerblocks(length, 0),
 * and bn_upperblocks(length) = bn_innerblocks(length, bn_length(num) - length)
 * ----------------------------------------------------------------------------
 * const bn_t num  -- The number whose inner blocks are to be copied.
 * uint32_t length -- The number of blocks to copy.
 * uint32_t index  -- The index from which to copy.
 * bn_t out        -- The number in which the blocks are to be stored.
 *
 * return          -- 1 if successful, 0 otherwise.
 */
int8_t bn_innerblocks(const bn_t num, uint32_t length, uint32_t index, bn_t out) {
    /* If the input is 0, or there is nothing we can copy, copy 0 and return */
    if(bn_iszero(num) || length == 0 || index >= num->noBlocks) {
        return bn_setzero(out);
    }
    /* If index is 0, we are just copying lower blocks. */
    if(index == 0)
        return bn_lowerblocks(num, length, out);

    /* If the request length is out of bounds, just copy as many as possible.*/
    if(length + index >= num->noBlocks) {
        return bn_upperblocks(num, num->noBlocks - index, out);
    }

    /* */
    if(!bn_resize(out, length)) return 0;

    memmove(out->blocks, num->blocks+index, length);

    return 1;
}

/* Copy the `length' msb of `num' into `out', so that the lsb of the range
 * being copied is the zero block of the output. For example, if `num' were
 * {1,2,3,4}, so 4 is the msb, and `length' were 2, then out would be {3,4}.
 * If `length' is more than the length of `num', the whole number is copied.
 * ----------------------------------------------------------------------------
 * const bn_t num  -- The number whose upper blocks are to be copied.
 * uint32_t length -- The number of upper blocks to copy from `num'
 * bn_t out        -- The number in which the upper blocks of `num' are stored.
 *
 * return          -- 1 if the upper blocks were copied, 0 otherwise.
 */
int8_t bn_upperblocks(const bn_t num, uint32_t length, bn_t out) {
    /* If num is empty, or the number of blocks to copy is 0, the output
     * should be empty too. */
    if(bn_isempty(num) || length == 0) {
        bn_free(out);
        return 1;
    }

    /* Copy the whole of the input into the output. */
    if(!bn_clone(out, num)) {
        /* If we couldn't copy num into out, return failure. */
        return 0;
    }

    /* If the number of blocks to copy is less than the total number of blocks,
     * shift the blocks downward, so we have the correct number of blocks. */
    if(length<num->noBlocks)
        bn_blockshift(out, -(num->noBlocks-length));

    return 1;
}

/* Splits the given bignum in to a sequence of bignums, such that each of the
 * new bignums has length equal to `subnum_length', except the final one which
 * may be shorter. The blocks of the first bignum are the lowest
 * `subnum_length' blocks of the input, the blocks of the second the next
 * lowest blocks, and so on, til the final bignum, whose blocks are the highest
 * blocks of the input. If the number of blocks in the input is not a multiple
 * of `subnum_length', the final bignum will have in.noBlocks % `subnum_length'
 * blocks instead.
 * ----------------------------------------------------------------------------
 * const bn_t in        -- The bignum to be split up.
 * size_t subnum_length -- The length of the bignums to be split into.
 * bn_t **outs          -- A pointer to an array of bignums where the outputs
 *                         are stored. This, and the contained bignums, will
 *                         be allocated within the function, so need to be
 *                         freed later.
 *
 * return               -- The number of bignums the input was split up into,
 *                         i.e. the size of the array `outs'. Returns 0 if the
 *                         number could not be split up.
 */
size_t bn_split_length(const bn_t in, size_t subnum_length, bn_t **outs) {
    /* If the input is empty, or the out blocks want to be of size zero, or the
     * output is NULL, then we can't split it. */
    if(bn_isempty(in) || subnum_length == 0 || outs == NULL) return 0;

    /* The number of bignums is just the size of the input over the required
     * length, plus one if the remainder is non-zero. */
    uint8_t remainder = (in->noBlocks % subnum_length == 0) ? 0 : 1;
    size_t subnum_count = (in->noBlocks / subnum_length) + remainder;

    /* Alloc the array of bignums. */
    *outs = malloc(subnum_count * sizeof(bn_t));
    for(size_t i = 0; i < subnum_count - remainder; i++) {
        if(!bn_init(&((*outs)[i]))) return 0;

        /* Set the size of the next bignum, then copy the next blocks from the
         * input. */
        bn_t next_bn = (*outs)[i];
        next_bn->noBlocks = subnum_length;
        if(!(next_bn->blocks = malloc(subnum_length))) return 0;
        memcpy(next_bn->blocks, in->blocks + (i * subnum_length),
            subnum_length);
    }

    if(remainder) {
        /* If the remainder is non-zero, the final output has that many blocks
         * instead. */
        if(!bn_init(&((*outs)[subnum_count - 1]))) return 0;

        bn_t next_bn = (*outs)[subnum_count - 1];
        size_t next_bn_len = in->noBlocks % subnum_length;
        next_bn->noBlocks = next_bn_len;
        if(!(next_bn->blocks = malloc(next_bn_len))) return 0;
        memcpy(next_bn->blocks,
            in->blocks + ((subnum_count - 1) * subnum_length), next_bn_len);
    }

    /* Return the number of bignums that the input was split into. */
    return subnum_count;
}

/* Splits the given bignum in to a sequence of `subnum_count' bignums, all of
 * which have the same length, except the final one, which may be shorter. The
 * blocks of the first bignum are the lowest blocks of the input, the blocks of
 * the second the next lowest blocks, and so on, til the final bignum, whose
 * blocks are the highest blocks of the input. If the number of blocks in the
 * input is not a multiple of `subnum_count', the final bignum will have fewer
 * blocks than the rest.
 * ----------------------------------------------------------------------------
 * const bn_t in        -- The bignum to be split up.
 * size_t subnum_length -- The number of bignums to be split into. i.e. the
 *                         length of the array `*outs'.
 * bn_t **outs          -- A pointer to an array of bignums where the outputs
 *                         are stored. This, and the contained bignums, will
 *                         be allocated within the function, so need to be
 *                         freed later. As we know the length of the array
 *                         beforehand in this case, we could alloc before, but
 *                         this way the type signatures of split_length and
 *                         split_count match.
 *
 * return               -- The length of the bignums the input was split up
 *                         into. Returns 0 if the number could not be split up.
 */
size_t bn_split_count(const bn_t in, size_t subnum_count, bn_t **outs) {
    /* If the input is empty, or we want 0 new numbers, or the output is NULL,
     * then we can't split it. */
    if(bn_isempty(in) || subnum_count == 0 || outs == NULL) return 0;

    /* Get the length of the numbers. */
    size_t subnum_length = in->noBlocks / subnum_count;
    size_t final_length;
    if(in->noBlocks % subnum_count != 0) {
        /* If there are some remaining blocks, add one to each block size,
         * except the highest number, which gets the left over. */
        subnum_length++;
        final_length = subnum_length + (in->noBlocks % subnum_count) -
            (subnum_count - 1);
    } else {
        /* Otherwise, the final number has the same size as the rest. */
        final_length = subnum_length;
    }

    /* Alloc the output. */
    *outs = malloc(subnum_count * sizeof(bn_t));
    for(size_t i = 0; i < subnum_count - 1; i++) {
        if(!bn_init(&((*outs)[i]))) return 0;

        /* Set the size of the next bignum, then copy the next blocks from the
         * input. */
        bn_t next_bn = (*outs)[i];
        next_bn->noBlocks = subnum_length;
        if(!(next_bn->blocks = malloc(subnum_length))) return 0;
        memcpy(next_bn->blocks, in->blocks + (i * subnum_length),
            subnum_length);
    }

    /* Copy the final number, which may have a different number of blocks. */
    if(!bn_init(&((*outs)[subnum_count - 1]))) return 0;
    bn_t final_bn = (*outs)[subnum_count - 1];
    if(!(final_bn->blocks = malloc(final_length))) return 0;
    memcpy(final_bn->blocks, in->blocks + ((subnum_count - 1) * subnum_length),
        final_length);

    /* Return the length of (most of) the numbers. */
    return subnum_length;
}

/* Remove the leading zeros from the number. If the number has no non-zero
 * blocks, the final one is not removed (i.e. the length becomes 1, rather than
 * 0). This simply updates the length of the number, and doesn't realloc to
 * remove them.
 * ----------------------------------------------------------------------------
 * bn_t in -- The number to remove leading zeros from.
 */
void bn_removezeros(bn_t in) {
    /* If there are fewer than 2 blocks, or the msb is non-zero, there
     * can be no leading zeros to remove*/
    if( in->noBlocks <= 1 || in->blocks[in->noBlocks-1] !=0 )
        return;

    /* Keep decrementing the length, until either the msb is non-zero, or
     * the length is just 1. */
    while( in->noBlocks > 1 && in->blocks[in->noBlocks-1] == 0 )
        in->noBlocks--;
}

/* Get the number of leading zeros of the number. If the number has no non-zero
 * blocks, this is one less than the number of blocks.
 * ----------------------------------------------------------------------------
 * const bn_t in -- The number to count the leading zeros of.
 *
 * return        -- The number of leading zeros of the number.
 */
uint32_t bn_leadingZeros(const bn_t in) {
    /* If the number has no blocks, it can't have any leading zeros. */
    if(bn_isempty(in)) return 0;

    uint32_t numZeros = 0;
    uint32_t noBlocks=bn_length(in);

    /* If the number of blocks is 1, or the msb is non-zero, it has no leading
     * zeros. */
    if(noBlocks == 1 || bn_getBlock(in, noBlocks - 1) != 0) return 0;

    /* Increment the number of leading zeros while the current index is not
     * not zero, and the block at that index is 0. */
    while(noBlocks > 1 && bn_getBlock(in, --noBlocks)==0) numZeros++;

    return numZeros;
}

/* Get the number of blocks of the number.
 * ----------------------------------------------------------------------------
 * const bn_t num -- The number to check its length.
 *
 * return         -- The number of blocks in the number.
 */
uint32_t bn_length(const bn_t num) {
    return num->noBlocks;
}

/* Get the number of blocks of the number, not including leading zeros. If the
 * number has no non-zero blocks, this will return 1.
 * ----------------------------------------------------------------------------
 * const bn_t num -- The number to find the number of non-zero blocks of.
 *
 * return         -- The number of blocks not including leading zeros.
 */
uint32_t bn_trueLength(const bn_t num) {
    uint32_t len = bn_length(num) - bn_leadingZeros(num);
    return len;
}

/* Get the block at the specified index in the number. 0 is the smallest index
 * block, num->noBlocks-1 is the largest. If the index is out of range, errno
 * is set to ERANGE, and -1 is returned. -1 (255) is a valid return value, so
 * errno should be checked in this case.
 * ----------------------------------------------------------------------------
 * const bn_t num -- The number to retrieve the block from.
 * uint32_t index -- The index of the block to retrieve.
 *
 * return         -- The value of the block at the given index, or -1 if the
 *                   index is out of range.
 */
uint8_t bn_getBlock(const bn_t num, uint32_t index) {
    if(index>=num->noBlocks) {
        errno = ERANGE;
        return 0;
    }
    errno = 0;
    return num->blocks[index];
}

/* Set the block at the given index in the number to the specified value. If
 * the index is out of range, nothing occurs.
 * ----------------------------------------------------------------------------
 * bn_t num       -- The number whose value is to be set.
 * uint32_t index -- The index of the block to set.
 * uint8_t val    -- The new value of the block at the given index.
 */
void bn_setBlock(bn_t num, uint32_t index, uint8_t val) {
    if(index<num->noBlocks)
        num->blocks[index] = val;
}

/* Checks if the given number is positive.
 * ----------------------------------------------------------------------------
 * const bn_t num -- The number to check is positive.
 *
 * return         -- 1 if the number is positive, 0 otherwise.
 */
int8_t bn_ispositive(const bn_t num) {
    return (num->sign == BN_POSITIVE) ? 1 : 0;
}

/* Checks if the given number is negative.
 * ----------------------------------------------------------------------------
 * const bn_t num -- The number to check is negative.
 *
 * return         -- 1 if the number is negative, 0 otherwise.
 */
int8_t bn_isnegative(const bn_t num) {
    return (num->sign == BN_NEGATIVE) ? 1 : 0;
}

/* Set the number to be positive.
 * ----------------------------------------------------------------------------
 * bn_t num -- The number to be set to be positive.
 */
void bn_setpositive(bn_t num) {
    num->sign = BN_POSITIVE;
}

/* Set the number to be negative.
 * ----------------------------------------------------------------------------
 * bn_t num -- The number to be set negative.
 */
void bn_setnegative(bn_t num) {
    num->sign = BN_NEGATIVE;
}

/* Swap the sign of the number. If the sign is positive, it'll be made negative
 * and vice versa.
 * ----------------------------------------------------------------------------
 * bn_t num -- The number to swap sign.
 */
void bn_togglesign(bn_t num) {
    switch(num->sign) {
        case BN_POSITIVE:
            num->sign = BN_NEGATIVE;
            break;
        case BN_NEGATIVE:
            num->sign = BN_POSITIVE;
            break;
    }
}
