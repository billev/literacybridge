#ifndef __UMICH_FIX_H
#define __UMICH_FIX_H
#ifdef __CC_ARM

#include <stdlib.h>

#define asm(X)
#define __asm__(X)

extern long foobarbaz;


#define BodyInit(...)
#define ChangeCodePage(...)
#define USB_INSERT_PTR foobarbaz
#define USB_ISR_PTR foobarbaz

#endif /* __CC_ARM */
#endif /* __UMICH_FIX_H */
