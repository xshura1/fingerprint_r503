#ifndef R503_H_
#define R503_H_

#include <stdint.h>

#define HEADER 0xEF01
#define DEFAULT_ADDR 0xFFFFFFFF
#define DELAY_RECEIVE_PACKET_MS 500 // 200 // 7

#define HEADER_LO (uint8_t)(HEADER & 0xFF)
#define HEADER_HI (uint8_t)(HEADER >> 8)

typedef enum
{
	PID_CMD = 0x01,
	PID_DATA = 0x02,
	PID_ACK = 0x07,
	PID_END = 0x08,
} PACKAGE_ID;

typedef enum
{
	IC_GEN_IMG = 0x01,			// Collect finger image
	IC_IMG_2TZ = 0x02,			// To generate character file from image
	IC_MATCH = 0x03,			// Carry out precise matching of two templates;
	IC_SEARCH = 0x04,			// Search the finger library
	IC_REG_MODEL = 0x05,		// To combine character files and generate template
	IC_STORE = 0x06,			// To store template;
	IC_LOAD_CHAR = 0x07,		// to read/load template
	IC_UP_CHAR = 0x08,			// to upload template
	IC_DOWN_CHR = 0x09,			// to download template
	IC_UP_IMAGE = 0x0A,			// To upload image
	IC_DOWN_IMAGE = 0x0B,		// To download image
	IC_DELETE_CHAR = 0x0C,		// to delete tempates
	IC_EMPTY = 0x0D,			// to empty the library
	IC_SET_SYSPARAM = 0x0E,		// To set system Parameter
	IC_READ_SYSPARAM = 0x0F,	// To read system Parameter
	IC_SET_PWD = 0x12,			// To set password
	IC_VFY_PWD = 0x13,			// To verify password
	IC_GET_RANDOM_CODE = 0x14,	// to get random code
	IC_SET_ADDER = 0x15,		// To set device address
	IC_READ_INF_PAGE = 0x16,	// Read information page
	IC_CONTROL = 0x17,			// Port control
	IC_WRITE_NOTEPAD = 0x18,	// to write note pad
	IC_READ_NOTEPAD = 0x19,		// To read note pad
	IC_TEMPLATE_NUM = 0x1D,		// To read finger template numbers
	IC_READ_INDEX_TABLE = 0x1F, // Read fingerprint template index table
	IC_GET_IMAGE_EX = 0x28,		// Fingerprint image collection extension command
	IC_CANCEL = 0x30,			// Cancel instruction
	IC_HANDSHAKE = 0x40,		// HandShake
	IC_CHECK = 0x36,			// Sensor CheckSensor
	IC_GET_ALG_VER = 0x39,		// Get the algorithm library version
	IC_GET_FW_VER = 0x3A,		// Get the firmware version
	IC_READ_PROD_INFO = 0x3C,	// Read product information
	IC_SOFT_RST = 0x3D,			// Soft reset
	IC_AURA_CONTROL = 0x35,		// control AuraLedConfig
} INSTRUCTION_CODE;

typedef enum
{
	CC_OK = 0x00,				  // Command execution is complete
	CC_PACKET_RECIEVE_ERR = 0x01, // Error when receiving data package
	CC_NO_FINGER = 0x02,		  // No finger on the sensor
	CC_IMAGE_FAIL = 0x03,		  // Failed to enroll the finger
	CC_IMAGE_MESS = 0x06,		  // Failed to generate character file due to overly disorderly fingerprint image
	CC_FEATURE_FAIL =
		0x07,						// Failed to generate character file due to the lack of character point or small fingerprint image
	CC_NO_MATCH = 0x08,				// Finger doesn't match
	CC_NOT_FOUND = 0x09,			// Failed to find matching finger
	CC_ENROLL_MISMATCH = 0x0A,		// Failed to combine the character files
	CC_BAD_LOCATION = 0x0B,			// Addressed PageID is beyond the finger library
	CC_DBRANGE_FAIL = 0x0C,			// Error when reading template from library or invalid template
	CC_UPLOAD_FEATURE_FAIL = 0x0D,	// Error when uploading template
	CC_PACKET_RESPONSE_FAIL = 0x0E, // Module failed to receive the following data packages
	CC_UPLOAD_FAIL = 0x0F,			// Error when uploading image
	CC_DELETE_FAIL = 0x10,			// Failed to delete the template
	CC_DB_CLEAR_FAIL = 0x11,		// Failed to clear finger library
	CC_PASS_FAIL = 0x13,			// Find whether the fingerprint passed or failed
	CC_INVALID_IMAGE = 0x15,		// Failed to generate image because of lac of valid primary image
	CC_FLASH_ERR = 0x18,			// Error when writing flash
	CC_UNKNOWN = 0x19,				// Unknown error
	CC_INVALID_REG_NUM = 0x1A,		// Invalid register number
	CC_INCORRECT_CONFIG_REG = 0x1B, // Invalid register number
	CC_WRONG_NOTEPAD_PNUM = 0x1C,	// Invalid register number
	CC_FAIL_OPERATE_PORT = 0x1D,	// Invalid register number
} CONFIRMATION_CODE;

typedef struct
{
	CONFIRMATION_CODE cc;
	void *data;
	uint8_t size;
} CMD_RESULT;

#pragma pack(push, 2)
typedef struct
{
	unsigned busy : 1;
	unsigned pass : 1;
	unsigned pwd : 1;
	unsigned img_buf_stat : 1;
	unsigned reserved : 12;
} STATUS_REGISTER;

typedef struct
{
	union
	{
		STATUS_REGISTER sr;
		uint16_t sr16;
	};

	uint16_t sys_code;
	uint16_t finger_lib_size;
	uint16_t security_level;
	uint32_t addr;
	uint16_t data_packet_size;
	uint16_t baud_rate;
} SYSTEM_PARAM;

typedef enum
{
	PN_UART_SPEED_9600 = 0x0401,
	PN_UART_SPEED_19200 = 0x0402,
	PN_UART_SPEED_38400 = 0x0404,
	PN_UART_SPEED_57600 = 0x0406,
	PN_UART_SPEED_115200 = 0x040C,

	PN_SECURITY_LEVEL_1 = 0x0501,
	PN_SECURITY_LEVEL_2 = 0x0502,
	PN_SECURITY_LEVEL_3 = 0x0503,
	PN_SECURITY_LEVEL_4 = 0x0504,
	PN_SECURITY_LEVEL_5 = 0x0505,

	PN_DATA_PACKAGE_LENGTH_32 = 0x0600,
	PN_DATA_PACKAGE_LENGTH_64 = 0x0601,
	PN_DATA_PACKAGE_LENGTH_128 = 0x0602,
	PN_DATA_PACKAGE_LENGTH_256 = 0x0603,
} PARAM_NUM;

typedef enum
{
	PC_OFF = 0,
	PC_ON = 1,
} PORT_CONTROL;

typedef enum
{
	IP_0 = 0,
	IP_1 = 1,
	IP_2 = 2,
	IP_3 = 3,
} INDEX_PAGE;

typedef enum
{
	CB_1 = 0x01,
	CB_2 = 0x02,
} CHAR_BUFFER;

typedef struct
{
	char model[16];			// Module type, ASCII
	char batch_num[4];		// Module batch number, ASCII
	char serial[8];			// Module serial number, ASCII
	uint16_t hard_ver;		// For the hardware version, the first byte represents the main version and the second byte
							// represents the sub-version
	char sensor_type[8];	// Sensor type, ASCII
	uint16_t image_width;	// Sensor image width
	uint16_t image_height;	// Sensor image height
	uint16_t template_size; // Template size
	uint16_t database_size; // Fingerprint database size
} PRODUCT_INFO;

typedef enum
{
	ACC_BREATHING_LIGHT = 0x01,
	ACC_FLASHING_LIGHT = 0x02,
	ACC_LIGHT_ALWAYS_ON = 0x03,
	ACC_LIGHT_ALWAYS_OFF = 0x04,
	ACC_LIGHT_GRADUALLY_ON = 0x05,
	ACC_LIGHT_GRADUALLY_OFF = 0x06,
} AURA_CONTROL_CODE;

typedef enum
{
	ALC_RED = 0x01,
	ALC_BLUE = 0x02,
	ALC_PURPLE = 0x03,
} AURA_LED_COLOR;

// System
CONFIRMATION_CODE verify_pwd(uint32_t addr, uint32_t password);
CMD_RESULT *set_pwd(uint32_t addr, uint32_t password);
CMD_RESULT *set_addr(uint32_t addr, uint32_t new_addr);
CONFIRMATION_CODE set_sys_param(uint32_t addr, PARAM_NUM param_num);
CONFIRMATION_CODE read_sys_param(SYSTEM_PARAM *sys_param, uint32_t addr);
CMD_RESULT *set_port_control(uint32_t addr, PORT_CONTROL pc);
CMD_RESULT *read_index_table(uint32_t addr, INDEX_PAGE index_page);

//  Fingerprint
CONFIRMATION_CODE gen_image(uint32_t addr);
CMD_RESULT *upload_image(uint32_t addr);
CMD_RESULT *download_image(uint32_t addr);
CONFIRMATION_CODE image_2_tz(uint32_t addr, CHAR_BUFFER buf_id);
CONFIRMATION_CODE reg_model(uint32_t addr);
CMD_RESULT *upload_template(uint32_t addr, CHAR_BUFFER buf_id);
CMD_RESULT *download_template(uint32_t addr, CHAR_BUFFER buf_id);
CONFIRMATION_CODE store_template(uint32_t addr, CHAR_BUFFER buf_id, uint16_t page_id);
CMD_RESULT *read_template(uint32_t addr, CHAR_BUFFER buf_id, uint16_t page_id);
CMD_RESULT *delete_template(uint32_t addr, uint16_t page_id);
CMD_RESULT *delete_all_templates(uint32_t addr);
CMD_RESULT *match(uint32_t addr);
CONFIRMATION_CODE search(uint32_t addr, CHAR_BUFFER buf_id, uint16_t start_page, uint16_t page_num);

// Others
CMD_RESULT *get_random_code(uint32_t addr);
CMD_RESULT *read_inf_page(uint32_t addr);
CMD_RESULT *write_notepad(uint32_t addr, uint8_t page_num);
CMD_RESULT *read_notepad(uint32_t addr, uint8_t page_num);
CONFIRMATION_CODE template_num(uint32_t addr, uint16_t *t_num);
CMD_RESULT *get_image_ex(uint32_t addr);
CMD_RESULT *cancel(uint32_t addr);
CMD_RESULT *handshake(uint32_t addr);
CMD_RESULT *check(uint32_t addr);
CMD_RESULT *get_alg_ver(uint32_t addr);
CMD_RESULT *get_firmware_ver(uint32_t addr);
CONFIRMATION_CODE read_product_info(PRODUCT_INFO *p_info, uint32_t addr);
CMD_RESULT *soft_reset(uint32_t addr);
CONFIRMATION_CODE aura_led_config(uint32_t addr, AURA_CONTROL_CODE ctrl_code, uint8_t speed,
								  AURA_LED_COLOR color, uint8_t cnt);

void set_color(uint32_t addr, AURA_LED_COLOR color);
void light_off(uint32_t addr);
uint8_t check_sensor(void);
uint8_t check_scan(void);
CONFIRMATION_CODE init_scan(void);
CONFIRMATION_CODE search_finger(void);

#endif /*R503_H_*/
