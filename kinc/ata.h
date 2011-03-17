#ifndef ATA_H
#define ATA_H

#include "stddef.h"
#include "stdbool.h"

bool ata_read_sector(uchar disk, uint lba, void* buffer);
bool ata_write_sector(uchar disk, uint lba, void* buffer);

bool ata_read_sectors(uchar disk, uint lba, uint sectors, void* buffer);
bool ata_write_sectors(uchar disk, uint lba, uint sectors, void* buffer);

#endif