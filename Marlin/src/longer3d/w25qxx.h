#pragma once

#include "stdint.h"

#define FLASH_WRITE_VAR(addr, value)  spiFlash.W25QXX_Write(reinterpret_cast<uint8_t *>(&value), uint32_t(addr), sizeof(value))
#define FLASH_READ_VAR(addr, value)   spiFlash.W25QXX_Read(reinterpret_cast<uint8_t *>(&value), uint32_t(addr), sizeof(value))

class W25QXX
{
public:
    void W25QXX_Init(void);
    uint16_t W25QXX_ReadID();
    void W25QXX_Write(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);   //write data to flash
    void W25QXX_Read(uint8_t* pBuffer,uint32_t ReadAddr,uint16_t NumByteToRead);     //read data in flash
    void W25QXX_Erase_Sector(uint32_t Dst_Addr);	  //erase sector
    void W25QXX_Write_NoCheck(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);  //write data to falsh is not check
    void W25QXX_Write_Enable(void);    //write enable
    void W25QXX_Write_Disable(void);   //write disable (protect)
    void W25QXX_Wait_Busy(void);     //wait for idle
    uint8_t	 W25QXX_ReadSR(void);    //read status register
    void W25QXX_Write_SR(uint8_t sr);  	//write status register
    void W25QXX_Write_Page(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);  
    void W25QXX_Erase_Chip(void);  	  //wipe the whole piece
    void W25QXX_WAKEUP(void);	      //wake up
    void W25QXX_PowerDown(void);    //go into power loss protection				

};

extern W25QXX spiFlash;
