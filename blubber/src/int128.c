#include <stdio.h>
#ifdef _WIN
#include "intrin.h"
#endif
#include "int128.h"


int128 int128_or(int128 v1, int128 v2){
    int128 r;
    r.v0 = v1.v0 | v2.v0;
    r.v1 = v1.v1 | v2.v1;
    return r;
}
int128 int128_xor(int128 v1, int128 v2){
    int128 r;
    r.v0 = v1.v0 ^ v2.v0;
    r.v1 = v1.v1 ^ v2.v1;
    return r;
}
int128 int128_and(int128 v1, int128 v2){
    int128 r;
    r.v0 = v1.v0 & v2.v0;
    r.v1 = v1.v1 & v2.v1;
    return r;
}
int128 int128_not(int128 v1){
    int128 r;
    r.v0 = ~v1.v0;
    r.v1 = ~v1.v1;
    return r;
}
int int128_popcnt(int128 v){
#ifndef _WIN
    return __builtin_popcountll(v.v0) + __builtin_popcountll(v.v1);
#else
	return __popcnt64(v.v0) + __popcnt64(v.v1);
#endif
}
int128 int128_setBit(int index){
    int128 r;
    r.v0 = 0;
    r.v1 = 0;
    if(index < 64){
        r.v0 = (unsigned long long)1 << index;
    }
    else{
        r.v1 = (unsigned long long)1 << (index - 64);
    }
    return r;
}
int int128_ffs(int128 value){
    int id;
#ifndef _WIN
    if((id = __builtin_ffsll(value.v0)))
        return id;
    else if((id = __builtin_ffsll(value.v1)))
        return id + 64;
#else
	unsigned long ret = 0;
	unsigned char res = _BitScanForward64(&ret, value.v0); // Long.numberOfTrailingZeros(low);
	if (res == 0)
	{
		res = _BitScanForward64(&ret, value.v1);
		if (res == 0)
			return 0;
		else
			ret += 64;
		return ret+1;
	}
#endif
    return ret+1;
}
int int128_getBit(int128 value, int id){
    if(id < 64){
        return (value.v0 >> id) & 1;
    }
    else{
        return (value.v1 >> (id-64)) & 1;
    }
}
int int128_notNull(int128 data){
    return (data.v0 != 0) || (data.v1 != 0);
}
void int128_debugWrite(int128 value){
    char buffer[220];
    int i = 0;
    for(int y = 9; y >= 0; y--){
        for(int x = 0; x < 10; x++){
            buffer[i] = int128_getBit(value, y * 10 + x) == 0 ? '0' : '1';
            i++;
            buffer[i] = ' ';
            i++;
        }
        buffer[i] = '\n';
        i++;
    }
    buffer[i] = 0;
    printf("%s", buffer);
    buffer[i] = 0;
}


