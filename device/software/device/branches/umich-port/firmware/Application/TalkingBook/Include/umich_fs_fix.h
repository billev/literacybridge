#ifndef UMICH_FS_FIX_H
#define UMICH_FS_FIX_H
#ifdef __CC_ARM

#define sizeof(X) (sizeof(X)>>1)
#define read(X, Y, Z) read(X, (Y)>>1, Z)
#define write(X, Y, Z) write(X, (Y)>>1, Z)

#endif
#endif
