#include <stdint.h>
#include <stm32f10x.h>
#include <stddef.h>

#include "r503.h"
#include "uart.h"
#include "systick.h"

#define ATTEMT_SCAN 10
#define SWAP_WORD(a) a = (((a & 0x00FF) << 8) | ((a & 0xFF00) >> 8));

#define SWAP_DWORD(a)                                  \
	a = (a & 0x00FF00FF) << 8 | (a & 0xFF00FF00) >> 8; \
	a = (a & 0x0000FFFF) << 16 | (a & 0xFFFF0000) >> 16;

void send_package(CMD_RESULT *res, uint32_t addr, PACKAGE_ID p_id, INSTRUCTION_CODE code,
				  uint8_t *data_in, uint8_t size_in, uint16_t delay_ms_rx);

uint8_t check_sensor(void)
{
	return (uint8_t)((GPIOA->IDR & GPIO_IDR_IDR11) != GPIO_IDR_IDR11);
}

uint8_t check_scan(void)
{
	return (uint8_t)((GPIOA->IDR & GPIO_IDR_IDR12) != GPIO_IDR_IDR12);
}

CONFIRMATION_CODE init_scan(void)
{
	CONFIRMATION_CODE cc;
	uint8_t attemt = ATTEMT_SCAN;

	do
	{
		cc = gen_image(DEFAULT_ADDR);
		if (!cc)
			cc = image_2_tz(DEFAULT_ADDR, CB_1);

		if (!cc)
			cc = gen_image(DEFAULT_ADDR);

		if (!cc)
			cc = image_2_tz(DEFAULT_ADDR, CB_2);

		if (!cc)
			cc = reg_model(DEFAULT_ADDR);

		cc = gen_image(DEFAULT_ADDR);
		if (!cc)
			cc = store_template(DEFAULT_ADDR, CB_1, 1);

		attemt--;

	} while (cc || !attemt);

	if (!cc)
	{
		set_color(DEFAULT_ADDR, ALC_BLUE);
	}

	return cc;
}

CONFIRMATION_CODE search_finger(void)
{
	CONFIRMATION_CODE cc;

	cc = gen_image(DEFAULT_ADDR);
	if (!cc)
		cc = image_2_tz(DEFAULT_ADDR, CB_1);

	if (!cc)
		cc = search(DEFAULT_ADDR, CB_1, 1, 10);

	return cc;
}

CONFIRMATION_CODE verify_pwd(uint32_t addr, uint32_t password)
{
	CMD_RESULT res;
	send_package(&res, addr, PID_CMD, IC_VFY_PWD, (uint8_t *)&password, 4, 10);

	return res.cc;
}

CMD_RESULT *set_pwd(uint32_t addr, uint32_t password)
{
	return NULL;
}

CMD_RESULT *set_addr(uint32_t addr, uint32_t new_addr)
{
	return NULL;
}

CONFIRMATION_CODE set_sys_param(uint32_t addr, PARAM_NUM param_num)
{
	CMD_RESULT cmd_res;
	uint8_t pn = param_num >> 8;
	uint8_t val = param_num & 0xFF;
	uint8_t data[2];

	data[0] = pn;
	data[1] = val;

	send_package(&cmd_res, addr, PID_CMD, IC_SET_SYSPARAM, &data[0], sizeof(data), 500);
	return cmd_res.cc;
}

CONFIRMATION_CODE read_sys_param(SYSTEM_PARAM *sys_param, uint32_t addr)
{
	CMD_RESULT cmd_res;
	send_package(&cmd_res, addr, PID_CMD, IC_READ_SYSPARAM, NULL, 0, 100);
	if (cmd_res.cc)
		return cmd_res.cc;

	*sys_param = *((SYSTEM_PARAM *)cmd_res.data);

	SWAP_WORD((*sys_param).sr16);
	SWAP_WORD((*sys_param).sys_code);
	SWAP_WORD((*sys_param).finger_lib_size);
	SWAP_WORD((*sys_param).security_level);
	SWAP_DWORD((*sys_param).addr);
	SWAP_WORD((*sys_param).data_packet_size);
	SWAP_WORD((*sys_param).baud_rate);

	(*sys_param).baud_rate *= 9600;
	(*sys_param).data_packet_size *= ((uint8_t)(1 << (*sys_param).data_packet_size)) * 32;

	return cmd_res.cc;
}

CMD_RESULT *set_port_control(uint32_t addr, PORT_CONTROL pc)
{
	return NULL;
}

CMD_RESULT *read_index_table(uint32_t addr, INDEX_PAGE index_page)
{
	return NULL;
}

// Fingerprint
CONFIRMATION_CODE gen_image(uint32_t addr)
{
	CMD_RESULT cmd_res;
	send_package(&cmd_res, addr, PID_CMD, IC_GEN_IMG, NULL, 0, 500);
	return cmd_res.cc;
}

CMD_RESULT *upload_image(uint32_t addr)
{
	return NULL;
}

CMD_RESULT *download_image(uint32_t addr)
{
	return NULL;
}

CONFIRMATION_CODE image_2_tz(uint32_t addr, CHAR_BUFFER buf_id)
{
	CMD_RESULT cmd_res;
	uint8_t data[1];

	data[0] = buf_id;

	send_package(&cmd_res, addr, PID_CMD, IC_IMG_2TZ, &data[0], 1, 600);
	return cmd_res.cc;
}

CONFIRMATION_CODE reg_model(uint32_t addr)
{
	CMD_RESULT cmd_res;
	send_package(&cmd_res, addr, PID_CMD, IC_REG_MODEL, NULL, 0, 500);
	return cmd_res.cc;
}
CMD_RESULT *upload_template(uint32_t addr, CHAR_BUFFER buf_id)
{
	return NULL;
}
CMD_RESULT *download_template(uint32_t addr, CHAR_BUFFER buf_id)
{
	return NULL;
}

CONFIRMATION_CODE store_template(uint32_t addr, CHAR_BUFFER buf_id, uint16_t page_id)
{
	CMD_RESULT cmd_res;
	uint8_t data[3];

	data[0] = buf_id;
	data[1] = (uint8_t)(page_id >> 8);
	data[2] = (uint8_t)(page_id & 0xFF);

	send_package(&cmd_res, addr, PID_CMD, IC_STORE, &data[0], sizeof(data), 1000);
	return cmd_res.cc;
}

CMD_RESULT *read_template(uint32_t addr, CHAR_BUFFER buf_id, uint16_t page_id)
{
	return NULL;
}

CMD_RESULT *delete_template(uint32_t addr, uint16_t page_id)
{
	return NULL;
}

CMD_RESULT *delete_all_templates(uint32_t addr)
{
	return NULL;
}

CMD_RESULT *match(uint32_t addr)
{
	return NULL;
}

CONFIRMATION_CODE search(uint32_t addr, CHAR_BUFFER buf_id, uint16_t start_page, uint16_t page_num)
{
	CMD_RESULT cmd_res;
	uint8_t data[5];

	data[0] = buf_id;
	data[1] = start_page >> 8;
	data[2] = start_page & 0xFF;
	data[3] = page_num >> 8;
	data[4] = page_num & 0xFF;

	send_package(&cmd_res, addr, PID_CMD, IC_SEARCH, &data[0], sizeof(data), 500);
	return cmd_res.cc;
}

// Others
CMD_RESULT *get_random_code(uint32_t addr)
{
	return NULL;
}
CMD_RESULT *read_inf_page(uint32_t addr)
{
	return NULL;
}
CMD_RESULT *write_notepad(uint32_t addr, uint8_t page_num)
{
	return NULL;
}
CMD_RESULT *read_notepad(uint32_t addr, uint8_t page_num)
{
	return NULL;
}

CONFIRMATION_CODE template_num(uint32_t addr, uint16_t *t_num)
{
	CMD_RESULT cmd_res;
	send_package(&cmd_res, addr, PID_CMD, IC_TEMPLATE_NUM, NULL, 0, 500);
	if (cmd_res.cc)
		return cmd_res.cc;

	*t_num = *((uint16_t *)cmd_res.data);

	SWAP_WORD(*t_num);

	return cmd_res.cc;
}

CMD_RESULT *get_image_ex(uint32_t addr)
{
	return NULL;
}

CMD_RESULT *cancel(uint32_t addr)
{
	return NULL;
}

CMD_RESULT *handshake(uint32_t addr)
{
	return NULL;
}

CMD_RESULT *check(uint32_t addr)
{
	return NULL;
}

CMD_RESULT *get_alg_ver(uint32_t addr)
{
	return NULL;
}

CMD_RESULT *get_firmware_ver(uint32_t addr)
{
	return NULL;
}

CONFIRMATION_CODE read_product_info(PRODUCT_INFO *p_info, uint32_t addr)
{
	CMD_RESULT cmd_res;
	send_package(&cmd_res, addr, PID_CMD, IC_READ_PROD_INFO, NULL, 0, 200);
	if (cmd_res.cc)
		return cmd_res.cc;

	*p_info = *((PRODUCT_INFO *)cmd_res.data);

	SWAP_WORD((*p_info).hard_ver);
	SWAP_WORD((*p_info).image_width);
	SWAP_WORD((*p_info).image_height);
	SWAP_WORD((*p_info).template_size);
	SWAP_WORD((*p_info).database_size);

	return cmd_res.cc;
}

CMD_RESULT *soft_reset(uint32_t addr)
{
	return NULL;
}

CONFIRMATION_CODE aura_led_config(uint32_t addr, AURA_CONTROL_CODE ctrl_code, uint8_t speed,
								  AURA_LED_COLOR color, uint8_t cnt)
{
	CMD_RESULT cmd_res;
	uint8_t data[4];

	data[0] = ctrl_code;
	data[1] = speed;
	data[2] = color;
	data[3] = cnt;

	send_package(&cmd_res, addr, PID_CMD, IC_AURA_CONTROL, &data[0], sizeof(data), 50);
	return cmd_res.cc;
}

void set_color(uint32_t addr, AURA_LED_COLOR color)
{
	aura_led_config(addr, ACC_LIGHT_ALWAYS_ON, 0, color, 0);
}

void light_off(uint32_t addr)
{
	aura_led_config(addr, ACC_LIGHT_ALWAYS_OFF, 0, ALC_BLUE, 0);
}

void send_package(CMD_RESULT *res, uint32_t addr, PACKAGE_ID p_id, INSTRUCTION_CODE code,
				  uint8_t *data_in, uint8_t size_in, uint16_t delay_ms_rx)
{

	uint8_t tx_buf[256];
	uint8_t tx_size = 0;

	uint8_t rx_buf[256];
	uint8_t rx_size = 0;

	uint16_t crc = 0;
	uint16_t len = 0;

	res->cc = CC_UNKNOWN;
	res->data = NULL;
	res->size = 0;

	tx_buf[0] = HEADER_HI;
	tx_buf[1] = HEADER_LO;

	tx_buf[2] = (uint8_t)(addr >> 24);
	tx_buf[3] = (uint8_t)(addr >> 16);
	tx_buf[4] = (uint8_t)(addr >> 8);
	tx_buf[5] = (uint8_t)(addr & 0xFF);
	tx_buf[6] = p_id;

	len = size_in + 3;
	tx_buf[7] = (uint8_t)(len >> 8);
	tx_buf[8] = (uint8_t)(len & 0xFF);
	tx_buf[9] = code;

	for (size_t i = 0; i < size_in; i++)
	{
		tx_buf[10 + i] = *data_in++;
	}

	crc = len + code + p_id;

	for (uint8_t i = 0; i < size_in; i++)
	{
		crc += tx_buf[10 + i];
	}

	tx_buf[10 + size_in] = (uint8_t)(crc >> 8);
	tx_buf[11 + size_in] = (uint8_t)(crc & 0xFF);

	tx_size = size_in + 12;

	USART1_send_data(&tx_buf[0], tx_size);
	delay_ms(delay_ms_rx);
	USART1_get_data(&rx_buf[0], &rx_size);

	if (rx_size < 12)
	{
		res->cc = CC_PACKET_RECIEVE_ERR;
		res->data = NULL;
		res->size = 0;
		return;
	}

	res->cc = (CONFIRMATION_CODE)rx_buf[9];
	len = rx_buf[7] << 8;
	len |= rx_buf[8];
	len = len - 3;
	if (len)
	{
		res->data = (uint8_t *)(&rx_buf[10]);
		res->size = len;
	}
}
