#ifndef APP_H
#define APP_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/drivers/PIN.h>

#include <bcomdef.h>

extern void Haldi_createTask(void);

#ifdef __cplusplus
}
#endif

#endif /* APP_H */
