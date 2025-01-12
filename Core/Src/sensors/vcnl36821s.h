#ifndef SRC_SENSORS_VCNL36821S_H_
#define SRC_SENSORS_VCNL36821S_H_

// Defines
#define VCNL36821S_I2C_HANDLE 		hi2c1
#define VCNL36821S_I2C_TIMEOUT 		10
#define VCNL36821S_SLAVE_ADDRESS	0x60 << 1

// Table 1 Registers
#define VCNL36821S_PS_CONF1      	0x00 // R/W
#define VCNL36821S_PS_CONF2      	0x03 // R/W
#define VCNL36821S_PS_CONF3      	0x04 // R/W
#define VCNL36821S_PS_CONF4      	0x04 // R/W
#define VCNL36821S_PS_THDL     		0x05 // R/W
#define VCNL36821S_PS_THDH    		0x06 // R/W
#define VCNL36821S_PS_CANC     		0x07 // R/W
#define VCNL36821S_PS_AC_L      	0x08 // R/W
#define VCNL36821S_PS_LPPERI      	0x08 // R/W

#define VCNL36821S_PS_DATA      	0xF8 // R
#define VCNL36821S_INT_FLAG      	0xF9 // R
#define VCNL36821S_ID		      	0xFA // R
#define VCNL36821S_PS_AC_DATA      	0xFB // R

// Table 2 PS_CONF1_LOW
#define VCNL36821S_PS_CONF1_LOW_PS_INIT			0x01<<7 // must be set to 1 when power on ready
#define VCNL36821S_PS_CONF1_LOW_PS_ON			0x01<<1
#define VCNL36821S_PS_CONF1_LOW_PS_RESERVERD	0x01<<0 // must be always 1

// Table 3 PS_CONF1_HIGH
#define VCNL36821S_PS_CONF1_HIGH_RESERVED		0x01<<1 // must be set to 1 when power on ready

// Table 4 PS_CONF2_LOW
#define VCNL36821S_PS_PERIOD_10ms   0x00<<6
#define VCNL36821S_PS_PERIOD_20ms  	0x01<<6
#define VCNL36821S_PS_PERIOD_40ms  	0x02<<6
#define VCNL36821S_PS_PERIOD_80ms  	0x03<<6
#define VCNL36821S_PS_PERS_1     	0x00<<4
#define VCNL36821S_PS_PERS_2     	0x01<<4
#define VCNL36821S_PS_PERS_3     	0x02<<4
#define VCNL36821S_PS_PERS_4     	0x03<<4
#define VCNL36821S_PS_INT_DIS    	0x00<<2
#define VCNL36821S_PS_INT_LOGIC    	0x01<<2
#define VCNL36821S_PS_INT_FIRST_H   0x02<<2
#define VCNL36821S_PS_INT_EN   		0x03<<2
#define VCNL36821S_PS_SMART_PERS_EN 0x01<<1
#define VCNL36821S_PS_ST_STOP   	0x01<<0
#define VCNL36821S_PS_ST_START      0x00<<0

// Table 5 PS_CONF2_HIGH
#define VCNL36821S_PS_1T       		0x00<<6
#define VCNL36821S_PS_2T       		0x01<<6
#define VCNL36821S_PS_4T       		0x02<<6
#define VCNL36821S_PS_8T       		0x03<<6
#define VCNL36821S_PS_MPS_1      	0x00<<4
#define VCNL36821S_PS_MPS_2      	0x01<<4
#define VCNL36821S_PS_MPS_4      	0x02<<4
#define VCNL36821S_PS_MPS_8      	0x03<<4
#define VCNL36821S_PS_ITB_25     	0x00<<3
#define VCNL36821S_PS_ITB_50     	0x01<<3
#define VCNL36821S_PS_HG_EN     	0x01<<2

// Table 6 PS_CONF3
#define VCNL36821S_PS_AF_Force      0x01<<6
#define VCNL36821S_PS_AF_Auto     	0x00<<6
#define VCNL36821S_PS_FOR_Trig   	0x01<<5
#define VCNL36821S_PS_FORCENUM   	0x01<<4
#define VCNL36821S_PS_SP_INT   		0x01<<2

// Table 7 PS_CONF4

// Table 8 PS_THDL

// Table 9 PS_THDH

// Table 10 PS_CANC

// Table 11 PS_AC

// Table 12 PS_LP

// Exports
bool vcnl36821s_init();
bool vcnl36821s_read(uint16_t *data);

#endif /* SRC_SENSORS_VCNL36821S_H_ */
