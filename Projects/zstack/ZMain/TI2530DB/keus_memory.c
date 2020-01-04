/*************************************************
 * ***********KEUS_MEMORY.C**********************
 * **********************************************/

#include "keus_memory.h"

 extern CONFIG_INFO_t config_data;


/*******************************************************
 * @fn          KeusThemeSwitchMiniMemoryInit
 * @brief       nvic memory init
 * @return
 * @param
 * *****************************************************/
bool KeusThemeSwitchMiniMemoryInit(void)
{
    //  for (uint8 i = 0; i < CLICK_TYPES; i++)
    //  {
    //    themeManager.btnThemeMap[i] = 255;
    //  }

    uint8 res = osal_nv_item_init(NVIC_MEMORY_POSITION, sizeof(&config_data), (void *)&config_data);

    if (res == SUCCESS || res == NV_ITEM_UNINIT)
    {
        return true;
    }
    else
    {
        return false;
    }
}
/*******************************************************
 * @fn      KeusThemeSwitchMiniReadConfigDataIntoMemory
 * @brief    
 * @return
 * @param  
 * ****************************************************/
bool KeusThemeSwitchMiniReadConfigDataIntoMemory(void)
{
    uint8 res = osal_nv_read(NVIC_MEMORY_POSITION, 0, sizeof(&config_data), (void *)&config_data);

    if (res == SUCCESS)
    {
        return true;
    }
    else
    {
        return false;
    }
}
/********************************************************
 * @fn  KeusThemeSwitchMiniWriteConfigDataIntoMemory
 * @brief   
 * @return
 * @param
 * *****************************************************/
bool KeusThemeSwitchMiniWriteConfigDataIntoMemory(void)
{
 uint8 res = osal_nv_write(NVIC_MEMORY_POSITION, 0, sizeof(&config_data), (void *)&config_data);

 if (res == SUCCESS)
 {
   return true;
 }
 else
 {
   return false;
 }
}