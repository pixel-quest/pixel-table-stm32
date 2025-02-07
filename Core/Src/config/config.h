#ifndef __CONFIG_H
#define __CONFIG_H

typedef struct {
	// General
	uint16_t Special_Chars;
	uint8_t Click_Dupl_Per;
	uint8_t Click_Off_Dupl_Msgs;

	// Sensors
	uint8_t RC_Filter_K;
	uint16_t Sensor_Click_Threshold;
	uint16_t Sensor_Click_Hysteresis;
	uint16_t Sensor_Coeff[NUM_PIXELS];
	uint16_t Sensor_Offset[NUM_PIXELS];
	bool Sensor_Defect[NUM_PIXELS];
} Config_t;

typedef struct {
	uint32_t data32[FLASH_CONFIG_SIZE_WORDS-1];
	uint32_t CRC32;
} FLASH_Sector_t;

union ConfigNVRAM {
	Config_t config;
	FLASH_Sector_t sector;
	uint32_t data32[FLASH_CONFIG_SIZE_WORDS];
};

extern union ConfigNVRAM GlobalConfig;

void Read_Global_Config();
void Save_Global_Config();

bool Read_Config_From_Page(union ConfigNVRAM *, uint8_t);
void Set_Default_Config(union ConfigNVRAM *);
void Save_Config_On_Page(union ConfigNVRAM *, uint8_t);

#endif /* __CONFIG_H */
