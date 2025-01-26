#include "main.h"
#include "config.h"

union ConfigNVRAM GlobalConfig;

// Дополнительный контроль адекватности конфига
// помогает проверять также при сохранении
#define SPECIAL_CHARS (uint16_t)0xABCD

void Read_Global_Config() {
	bool goodPages[FLASH_CONFIG_PAGES];
	bool atLeastOneSuccess = false;

	for (uint8_t i=0;i<FLASH_CONFIG_PAGES;i++) {
		goodPages[i] = Read_Config_From_Page(&GlobalConfig, i);
		atLeastOneSuccess |= goodPages[i];
	}

	// If bad values set to default
	if (GlobalConfig.config.Special_Chars != SPECIAL_CHARS ||
			GlobalConfig.config.Click_Dupl_Per == 0 ||
			GlobalConfig.config.Click_Off_Dupl_Msgs == 0 ||
			GlobalConfig.config.Sensor_Click_Threshold == 0)
		atLeastOneSuccess = false;

	if (!atLeastOneSuccess)
		Set_Default_Config(&GlobalConfig);

	for(uint8_t i=0;i<FLASH_CONFIG_PAGES;i++)
		if (!goodPages[i])
			Save_Config_On_Page(&GlobalConfig, i);
}

bool Read_Config_From_Page(union ConfigNVRAM *configNVRAM, uint8_t PAGE) {
	union ConfigNVRAM tempConfig;
	uint32_t START_ADDRESS = FLASH_CONFIG_START_ADDR + PAGE * FLASH_PAGE_SIZE;
	uint32_t address = START_ADDRESS;
	uint32_t index = 0;

	while(address < START_ADDRESS + FLASH_PAGE_SIZE) {
		tempConfig.data32[index] = *(__IO uint32_t *)address;
		index = index + 1;
		address = address + 4;
	}

	uint32_t calculatedCRC32 = HAL_CRC_Calculate(&hcrc, tempConfig.sector.data32, FLASH_CONFIG_SIZE_WORDS-1);
	if (calculatedCRC32 == tempConfig.sector.CRC32) {
		memcpy(configNVRAM->data32, tempConfig.data32, sizeof(configNVRAM->data32)); // sizeof in bytes
		return true;
	}
	return false;
}

void Set_Default_Config(union ConfigNVRAM *configNVRAM) {
	memset(configNVRAM->data32, 0, sizeof(configNVRAM->data32)); // sizeof in bytes

	// General
	configNVRAM->config.Special_Chars = SPECIAL_CHARS;
	configNVRAM->config.Click_Dupl_Per = DEFAULT_CLICK_DUPLICATE_PER;
	configNVRAM->config.Click_Off_Dupl_Msgs = DEFAULT_CLICK_OFF_DUPLICATE_MESSAGES;

	// Sensors
	configNVRAM->config.RC_Filter_K = DEFAULT_RC_FILTER_K;
	configNVRAM->config.Sensor_Click_Threshold = DEFAULT_CLICK_THRESHOLD;
	configNVRAM->config.Sensor_Click_Hysteresis = DEFAULT_CLICK_HYSTERESIS;

	for (uint8_t i=0; i<NUM_PIXELS; i++) {
		configNVRAM->config.Sensor_Coeff[i] = DEFAULT_SENSOR_COEFF;
		configNVRAM->config.Sensor_Defect[i] = false;
	}
}

void Save_Global_Config() {
	if (GlobalConfig.config.Special_Chars != SPECIAL_CHARS) { // конфиг поврежден
		HAL_NVIC_SystemReset(); // перезагрузимся от греха подальше
		return;
	}

	// Save the config on several pages for reliability
	for (uint8_t i=0; i<FLASH_CONFIG_PAGES; i++) {
		Save_Config_On_Page(&GlobalConfig, i);
	}
}

void Save_Config_On_Page(union ConfigNVRAM *configNVRAM, uint8_t PAGE) {
	HAL_FLASH_Unlock();

	static FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t START_ADDRESS = FLASH_CONFIG_START_ADDR + PAGE * FLASH_PAGE_SIZE;
	uint32_t address = START_ADDRESS;
	uint32_t index = 0;

	// Erase one page at a time
	EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.PageAddress = START_ADDRESS;
	EraseInitStruct.NbPages = 1;

	uint32_t PageError;
	HAL_FLASHEx_Erase(&EraseInitStruct, &PageError);

	if (PageError == START_ADDRESS) {
		HAL_FLASH_Lock();
		Error_Handler();
	}

	configNVRAM->sector.CRC32 = HAL_CRC_Calculate(&hcrc, configNVRAM->sector.data32, FLASH_CONFIG_SIZE_WORDS-1);
	while(address < START_ADDRESS + FLASH_PAGE_SIZE) {
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, configNVRAM->data32[index]) != HAL_OK) {
			Error_Handler();
		}
		index = index + 1;
		address = address + 4;
	}

	HAL_FLASH_Lock();
}

