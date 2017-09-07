#ifndef _ACPI_H_

#define _ACPI_H_

#ifndef _TYPE_H_
#include "../type.h"
#endif

void* find_rsd_ptr();
int initAcpi(void);
void acpiPowerOff(void);



#endif
