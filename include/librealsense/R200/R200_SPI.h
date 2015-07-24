#pragma once

#ifndef LIBREALSENSE_R200_SPI_H
#define LIBREALSENSE_R200_SPI_H

#include "libuvc/libuvc.h"
#include "R200_CalibrationIntrinsics.h"
#include "R200_XU.h"

// SPI
#define SPI_FLASH_PAGE_SIZE_IN_BYTES                        0x100
#define SPI_FLASH_SECTOR_SIZE_IN_BYTES                      0x1000
#define SPI_FLASH_SIZE_IN_SECTORS                           256
#define SPI_FLASH_TOTAL_SIZE_IN_BYTES                       (SPI_FLASH_SIZE_IN_SECTORS * SPI_FLASH_SECTOR_SIZE_IN_BYTES)
#define SPI_FLASH_PAGES_PER_SECTOR                          (SPI_FLASH_SECTOR_SIZE_IN_BYTES / SPI_FLASH_PAGE_SIZE_IN_BYTES)
#define SPI_FLASH_LENGTH_IN_PAGES(N_BYTES)                  ((N_BYTES + 0xFF) / SPI_FLASH_PAGE_SIZE_IN_BYTES)

#define SPI_FLASH_SECTORS_RESERVED_FOR_FIRMWARE             160
#define SPI_FLASH_START_OF_SECTORS_NOT_FOR_FIRMWARE         (SPI_FLASH_SECTORS_RESERVED_FOR_FIRMWARE * SPI_FLASH_SECTOR_SIZE_IN_BYTES)

#define SPI_FLASH_SECTORS_RESERVED_FOR_ROUTINES             64
#define SPI_FLASH_FIRST_ROUTINE_SECTOR                      (SPI_FLASH_SIZE_IN_SECTORS - SPI_FLASH_SECTORS_RESERVED_FOR_ROUTINES)

// 1 Mb total
#define NV_STORAGE_IN_BYTES                                 (SPI_FLASH_SECTOR_SIZE_IN_BYTES * SPI_FLASH_SIZE_IN_SECTORS)
#define NV_NON_FIRMWARE_START                               (SPI_FLASH_SECTORS_RESERVED_FOR_FIRMWARE * SPI_FLASH_SECTOR_SIZE_IN_BYTES)

#define NV_ADMIN_DATA_N_ENTRIES                             9
#define NV_CALIBRATION_DATA_ADDRESS_INDEX                   0
#define NV_IFFLEY_ROUTINE_TABLE_ADDRESS_INDEX               1

#define NV_NON_FIRMWARE_ROOT_ADDRESS                        NV_NON_FIRMWARE_START

#define UNUSED_ROUTINE(ENTRY)                               (ENTRY == UNINITIALIZED_ROUTINE_ENTRY || ENTRY == DELETED_ROUTINE_ENTRY)
typedef unsigned short RoutineDescription;

#define MAX_ROUTINES                                        256

#define SIZEOF_ROUTINE_DESCRIPTION_TABLE                    (MAX_ROUTINES * sizeof(RoutineDescription))
#define SIZEOF_ERASED_TABLE                                 (SPI_FLASH_SECTORS_RESERVED_FOR_ROUTINES * sizeof(unsigned short))
#define SIZEOF_PRESERVE_TABLE                               (SPI_FLASH_SECTORS_RESERVED_FOR_ROUTINES * sizeof(unsigned short))

#define SIZEOF_ROUTINE_DESCRIPTION_ERASED_AND_PRESERVE_TABLE (SIZEOF_ROUTINE_DESCRIPTION_TABLE + SIZEOF_ERASED_TABLE + SIZEOF_PRESERVE_TABLE)

#define ROUTINE_DESCRIPTION_OFFSET                          0
#define ERASED_TABLE_OFFSET                                 SIZEOF_ROUTINE_DESCRIPTION_TABLE
#define PRESERVE_TABLE_OFFSET                               (ERASED_TABLE_OFFSET + SIZEOF_ERASED_TABLE)

typedef struct
{
    RoutineDescription rd[MAX_ROUTINES]; // Partition Table
    unsigned short erasedTable[SPI_FLASH_SECTORS_RESERVED_FOR_ROUTINES];
    unsigned short preserveTable[SPI_FLASH_SECTORS_RESERVED_FOR_ROUTINES];
} RoutineStorageTables;

// Bus group
#define COMMAND_MODIFIER_DIRECT 0x00000010

#define CAM_INFO_BLOCK_LEN 2048

//@tofix error handling all up in this bitch
class SPI_Interface
{
    CameraCalibrationParameters parameters;
    
    uint8_t cameraHeader[SPI_FLASH_SECTOR_SIZE_IN_BYTES];
    uint8_t calibrationData[CAM_INFO_BLOCK_LEN];
    
    void ReadCalibrationSector();
    
    uvc_device_handle_t * deviceHandle;
    
public:
    
    RoutineStorageTables rst;
    
    SPI_Interface(uvc_device_handle_t * deviceHandle);
    void Initialize();
    
    void PrintHeaderInfo();

    CameraCalibrationParameters GetRectifiedParameters() { return parameters; }
    
    RectifiedIntrinsics GetZIntrinsics(int mode = 0);
    
};

#endif