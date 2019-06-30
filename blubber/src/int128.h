#ifndef __INT128_H__
#define __INT128_H__

#define OR128(a, b) ({})

typedef	struct{
    unsigned long long v0;
    unsigned long long v1;
}int128;
/**
 * equivalent to |
 * @param v1
 * @param v2
 * @return v1 | v2
 */
int128 int128_or(int128 v1, int128 v2);
/**
 * equivalent to ^
 * @param v1
 * @param v2
 * @return v1 ^ v2
 */
int128 int128_xor(int128 v1, int128 v2);
/**
 * equivalent to &
 * @param v1
 * @param v2
 * @return v1 & v2
 */
int128 int128_and(int128 v1, int128 v2);
/**
 * equivalent to ~
 * @param v1
 * @param v2
 * @return ~v1
 */
int128 int128_not(int128 v1);
/**
 * Count the number of set Bit's
 * @param v
 * @return the number of set Bit's
 */
int int128_popcnt(int128 v);
/**
 * Creates an int128 there the indes'th bit is set
 * @param index
 * @return 
 */
int128 int128_setBit(int index);

void int128_setBitInVal(int index, int128* val);


/**
 * Returns data != 0
 * @param data
 * @return 
 */
int int128_notNull(int128 data);
/**
 * Returns the index of the first set Bit + 1
 * @param value
 * @return 
 */
int int128_ffs(int128 value);
/**
 * Writes a Human Readable output to stdout
 * @param value
 */
void int128_debugWrite(int128 value);
#endif