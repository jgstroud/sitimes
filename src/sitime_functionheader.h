// SPDX-License-Identifier: GPL-2.0
/*
 * Header file for Driver function declarations for SiTime Labs SiT95317/SiT95316/SIT95211
 *
 * Copyright (C) 2024
 * Author: Ali Rouhi
 * 	   Serhiy Vlasenko
 *
 * The SiT95317 has 4 input & 8 output clocks.
 * The SiT95316 has 4 inputs & 16 output clocks.
 */

const struct siT9531x_reg_cfg chip_rst_cfg[] = {
        {  0xfe, 0x01 },    // apply soft reset on chip
        {  0xfe, 0x00 },    // disable reset
};

const struct siT9531x_device device_support[] = {
        { SIT95316, SI95316_MAX_NUM_INPUTS, SI95316_MAX_NUM_OUTPUTS, "SiT95316"},
        { SIT95317, SI95317_MAX_NUM_INPUTS, SI95317_MAX_NUM_OUTPUTS, "SiT95317"},
        { SIT95211, SI95211_MAX_NUM_INPUTS, SI95211_MAX_NUM_OUTPUTS, "SiT95211"},
};

char gDrvrName[] = "SiT9531xDrv";

/*..............Function Declarations.............*/
static int  si9531x_dt_parse(struct drv_si9531x *);
static int  CreateCharDevice(struct drv_si9531x *);

/*...................Ops Inputs....................*/
static int  si9531x_clkin_set_rate(struct clk_hw *, unsigned long, unsigned long);
static int  si9531x_clkin_is_enabled(struct clk_hw *);
static int  si9531x_clkin_enable(struct clk_hw *);
static void si9531x_clkin_disable(struct clk_hw *);
static unsigned long si9531x_clkin_recalc_rate(struct clk_hw *, unsigned long);

/*...................Ops Outputs...................*/
static int  si9531x_clkout_prepare(struct clk_hw *);
static int  si9531x_clkout_set_rate(struct clk_hw *, unsigned long, unsigned long);
static int  si9531x_clkout_is_enabled(struct clk_hw *);
static int  si9531x_clkout_enable(struct clk_hw *);
static void si9531x_clkout_disable(struct clk_hw *);
static void si9531x_clkout_unprepare(struct clk_hw *);
static unsigned long si9531x_clkout_recalc_rate(struct clk_hw *, unsigned long);

/*....................... Change frequency plans ..................*/
static int  set_frequency_plan(unsigned int);
static int  siT9531x_write_multiple(struct drv_si9531x *,const struct  siT9531x_reg_cfg *, unsigned int);
static int  soft_rst_por_chip(void);
static int  clkout_set_frequency(struct i2c_client *, int, int, unsigned int);
static int  clkin_set_frequency(struct i2c_client *, int, unsigned int, unsigned int);
static int  clkout_enable_disable(struct i2c_client *, int, bool);
static int  clkin_enable_disable(struct i2c_client *, int , bool);
static int  switch_to_PRGCmd_state(struct i2c_client *,unsigned int);
static int  do_updatenvm_lockloop(struct i2c_client *client);
static int  write_divn1_to_registers(struct i2c_client *, unsigned int, u16 );
static int  write_divn_to_registers(struct i2c_client *, unsigned int, struct divider_data *);
static int  write_divn2_to_registers(struct i2c_client *, unsigned int, struct divider_data *);
static bool clkout_is_enable_disable(struct i2c_client *, int, bool, unsigned int *);
static bool clkin_is_enable_disable(struct i2c_client *, int, bool, unsigned int *);
static u16  calculate_divn1(unsigned int);
static struct divider_data calculate_divn(unsigned int, u64);
static struct divider_data calculate_divn2(unsigned int, u64);
static int set_fast_frequency(struct i2c_client *, unsigned int);
static int set_normal_frequency(struct i2c_client *, unsigned int);
static int set_accuracy(struct i2c_client *, unsigned int);

/*....................... Input Settings .........................*/
static int  clkin_set_doubler(struct i2c_client *,unsigned int);

/*....................... Validation Functions ...................*/
static int  validate_xtalfreq_range(unsigned int);
static int  validate_inputfreq_range(unsigned int);
static int  validate_outputfreq_range(unsigned int);
static int  validate_inputclkid_range(unsigned int, unsigned int);
static int  validate_outputclkid_range(unsigned int, unsigned int) ;
static int  validate_normalbw_range(unsigned int);
static int  validate_fastbw_range(unsigned int);
static int  validate_normalbw(unsigned int,unsigned int);
static int  validate_fastbw(unsigned int,unsigned int);

/*.................. Input/Output Link Settings.................*/
static int clkin_set_linktype(struct i2c_client *, int, const struct inclk_linkcfg*);
static int set_inputlinktype(struct i2c_client *, int, unsigned int);
static int set_inputmode(struct i2c_client *, int,unsigned int, unsigned int);

static int clkout_set_linktype(struct i2c_client *, int, const struct outclk_linkcfg*);
static int set_outputlinktype(struct i2c_client *, int, unsigned int);
static int set_differential_mode(struct i2c_client *, int, unsigned int, unsigned int);
static int set_swing_voltage(struct i2c_client *, int, unsigned int);

static int write_outclk_pll_bits(struct i2c_client *, unsigned int , u8, u8);

/*................. Config Device Settings......................*/
static int write_default_configuration(u16);
