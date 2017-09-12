#ifndef __BMP180_H__
#define __BMP180_H__

typedef	int8_t s8;
typedef	int16_t s16;
typedef	int32_t s32;
typedef	int64_t s64;

typedef	uint8_t u8;
typedef	uint16_t u16;
typedef	uint32_t u32;
typedef	uint64_t u64;


#define bmp180_calc_temperature(ut)\
bmp180_get_temperature(ut)

#define bmp180_calc_pressure(up)\
bmp180_get_pressure(up)

#define bmp180_read_ut()\
bmp180_get_ut()

#define bmp180_read_up()\
bmp180_get_up()


#define bmp180_read_cal_param()\
bmp180_get_cal_param()

#define smd500_read_cal_param()\
smd500_get_cal_param()

#define BMP180_BUS_WR_RETURN_TYPE s8
#define BMP180_BUS_WR_PARAM_TYPES u8, u8, u8 *, u8
#define BMP180_BUS_WR_PARAM_ORDER (device_addr, register_addr, register_data, write_length)

#define BMP180_BUS_WRITE_FUNC(device_addr, register_addr, \
register_data, write_length)\
	bus_write(device_addr, register_addr, register_data, write_length)

#define BMP180_BUS_RD_RETURN_TYPE s8
#define BMP180_BUS_RD_PARAM_TYPES (u8, u8, u8 *, u8)
#define BMP180_BUS_RD_PARAM_ORDER (device_addr, register_addr, register_data, read_length)

#define BMP180_BUS_READ_FUNC(device_addr, register_addr, \
register_data, read_length)\
	bus_read(device_addr, register_addr, register_data, read_length)

#define BMP180_WR_FUNC_PTR \
s8 (*bus_write)(u8, u8, u8 *, u8)

#define BMP180_RD_FUNC_PTR \
s8 (*bus_read)(u8, u8, u8 *, u8)

#define BMP180_MDELAY_DATA_TYPE	u32

#define BMP180_I2C_ADDR		(0x77)

#define E_BMP_NULL_PTR				((s8)-127)
#define E_BMP_COMM_RES				((s8)-1)
#define E_BMP_OUT_OF_RANGE			((s8)-2)

#define BMP180_RETURN_FUNCTION_TYPE        s8
#define   BMP180_INIT_VALUE						((u8)0)
#define   BMP180_INITIALIZE_OVERSAMP_SETTING_U8X	((u8)0)
#define   BMP180_INITIALIZE_SW_OVERSAMP_U8X			((u8)0)
#define   BMP180_INITIALIZE_NUMBER_OF_SAMPLES_U8X	((u8)1)
#define   BMP180_GEN_READ_WRITE_DATA_LENGTH			((u8)1)
#define   BMP180_TEMPERATURE_DATA_LENGTH			((u8)2)
#define   BMP180_PRESSURE_DATA_LENGTH				((u8)3)
#define   BMP180_SW_OVERSAMP_U8X					((u8)1)
#define   BMP180_OVERSAMP_SETTING_U8X				((u8)3)
#define   BMP180_2MS_DELAY_U8X			(2)
#define   BMP180_3MS_DELAY_U8X			(3)
#define   BMP180_AVERAGE_U8X			(3)
#define   BMP180_INVALID_DATA			(0)
#define   BMP180_CHECK_DIVISOR			(0)
#define   BMP180_DATA_MEASURE			(3)
#define   BMP180_CALCULATE_TRUE_PRESSURE		(8)
#define   BMP180_CALCULATE_TRUE_TEMPERATURE		(8)
#define BMP180_SHIFT_BIT_POSITION_BY_01_BIT			(1)
#define BMP180_SHIFT_BIT_POSITION_BY_02_BITS		(2)
#define BMP180_SHIFT_BIT_POSITION_BY_04_BITS		(4)
#define BMP180_SHIFT_BIT_POSITION_BY_06_BITS		(6)
#define BMP180_SHIFT_BIT_POSITION_BY_08_BITS		(8)
#define BMP180_SHIFT_BIT_POSITION_BY_11_BITS		(11)
#define BMP180_SHIFT_BIT_POSITION_BY_12_BITS		(12)
#define BMP180_SHIFT_BIT_POSITION_BY_13_BITS		(13)
#define BMP180_SHIFT_BIT_POSITION_BY_15_BITS		(15)
#define BMP180_SHIFT_BIT_POSITION_BY_16_BITS		(16)

#define BMP180_PROM_START__ADDR		(0xAA)
#define BMP180_PROM_DATA__LEN		(22)

#define BMP180_CHIP_ID_REG			(0xD0)
#define BMP180_VERSION_REG			(0xD1)

#define BMP180_CTRL_MEAS_REG		(0xF4)
#define BMP180_ADC_OUT_MSB_REG		(0xF6)
#define BMP180_ADC_OUT_LSB_REG		(0xF7)

#define BMP180_SOFT_RESET_REG		(0xE0)

#define BMP180_T_MEASURE			(0x2E)
#define BMP180_P_MEASURE			(0x34)
#define BMP180_TEMP_CONVERSION_TIME  (5)

#define BMP180_PARAM_MG		(3038)
#define BMP180_PARAM_MH		(-7357)
#define BMP180_PARAM_MI		(3791)

#define	BMP180_TEMPERATURE_DATA_BYTES	(2)
#define	BMP180_PRESSURE_DATA_BYTES		(3)
#define	BMP180_TEMPERATURE_LSB_DATA		(1)
#define	BMP180_TEMPERATURE_MSB_DATA		(0)
#define	BMP180_PRESSURE_MSB_DATA		(0)
#define	BMP180_PRESSURE_LSB_DATA		(1)
#define	BMP180_PRESSURE_XLSB_DATA		(2)

#define	BMP180_CALIB_DATA_SIZE			(22)
#define	BMP180_CALIB_PARAM_AC1_MSB		(0)
#define	BMP180_CALIB_PARAM_AC1_LSB		(1)
#define	BMP180_CALIB_PARAM_AC2_MSB		(2)
#define	BMP180_CALIB_PARAM_AC2_LSB		(3)
#define	BMP180_CALIB_PARAM_AC3_MSB		(4)
#define	BMP180_CALIB_PARAM_AC3_LSB		(5)
#define	BMP180_CALIB_PARAM_AC4_MSB		(6)
#define	BMP180_CALIB_PARAM_AC4_LSB		(7)
#define	BMP180_CALIB_PARAM_AC5_MSB		(8)
#define	BMP180_CALIB_PARAM_AC5_LSB		(9)
#define	BMP180_CALIB_PARAM_AC6_MSB		(10)
#define	BMP180_CALIB_PARAM_AC6_LSB		(11)
#define	BMP180_CALIB_PARAM_B1_MSB		(12)
#define	BMP180_CALIB_PARAM_B1_LSB		(13)
#define	BMP180_CALIB_PARAM_B2_MSB		(14)
#define	BMP180_CALIB_PARAM_B2_LSB		(15)
#define	BMP180_CALIB_PARAM_MB_MSB		(16)
#define	BMP180_CALIB_PARAM_MB_LSB		(17)
#define	BMP180_CALIB_PARAM_MC_MSB		(18)
#define	BMP180_CALIB_PARAM_MC_LSB		(19)
#define	BMP180_CALIB_PARAM_MD_MSB		(20)
#define	BMP180_CALIB_PARAM_MD_LSB		(21)


struct bmp180_calib_param_t {
	s16 ac1;
	s16 ac2;
	s16 ac3;
	u16 ac4;
	u16 ac5;
	u16 ac6;
	s16 b1;
	s16 b2;
	s16 mb;
	s16 mc;
	s16 md;
};

struct bmp180_t {
	struct bmp180_calib_param_t calib_param;    /**<calibration data*/
	u8 mode;                                    /**<power mode*/
	u8 chip_id;                                 /**<chip id*/
	u8 ml_version;                              /**<ml version*/
	u8 al_version;                              /**<al version*/
	u8 dev_addr;                                /**<device address*/
	u8 sensortype;                              /**< sensor type*/

	s32 param_b5;                               /**<pram*/
	s32 number_of_samples;                      /**<sample calculation*/
	s16 oversamp_setting;                       /**<oversampling setting*/
	s16 sw_oversamp;                            /**<software oversampling*/

	BMP180_WR_FUNC_PTR;                         /**< bus write function pointer*/
	BMP180_RD_FUNC_PTR;                         /**< bus read function pointer*/
	void (*delay_msec)(BMP180_MDELAY_DATA_TYPE);/**< delay function pointer*/
};

#define BMP180_CHIP_ID__POS             (0)
#define BMP180_CHIP_ID__MSK             (0xFF)
#define BMP180_CHIP_ID__LEN             (8)
#define BMP180_CHIP_ID__REG             (BMP180_CHIP_ID_REG)

#define BMP180_ML_VERSION__POS          (0)
#define BMP180_ML_VERSION__LEN          (4)
#define BMP180_ML_VERSION__MSK          (0x0F)
#define BMP180_ML_VERSION__REG          (BMP180_VERSION_REG)

#define BMP180_AL_VERSION__POS          (4)
#define BMP180_AL_VERSION__LEN          (4)
#define BMP180_AL_VERSION__MSK          (0xF0)
#define BMP180_AL_VERSION__REG          (BMP180_VERSION_REG)

#define BMP180_GET_BITSLICE(regvar, bitname)\
((regvar & bitname##__MSK) >> (bitname##__POS))


#define BMP180_SET_BITSLICE(regvar, bitname, val)\
((regvar & ~bitname##__MSK) | ((val<<bitname##__POS)&bitname##__MSK))

BMP180_RETURN_FUNCTION_TYPE bmp180_init(struct bmp180_t *bmp180);
s16 bmp180_get_temperature(u32 v_uncomp_temperature_u32);
s32 bmp180_get_pressure(u32 v_uncomp_pressure_u32);
u16 bmp180_get_uncomp_temperature(void);
u32  bmp180_get_uncomp_pressure(void);
BMP180_RETURN_FUNCTION_TYPE bmp180_get_calib_param(void);

/* __BMP180_H__*/
#endif
