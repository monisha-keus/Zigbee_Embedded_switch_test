/*******************************************
 * ***********KEUS_MEMORY.H*****************
 * *****************************************/

#ifndef KEUS_MEMORY_H_INCLUDED
#define KEUS_MEMORY_H_INCLUDED

#include "ZComDef.h"
#include "OSAL_Nv.h"
#include "keus_task.h"

#define NVIC_MEMORY_POSITION 0x10

bool KeusThemeSwitchMiniMemoryInit(void);
bool KeusThemeSwitchMiniReadConfigDataIntoMemory(void);
bool KeusThemeSwitchMiniWriteConfigDataIntoMemory(void);

#endif  //KEUS_MEMORY_H_INCLUDED