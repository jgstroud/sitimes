// SPDX-License-Identifier: GPL-2.0
/*
 * Driver for SiTime SiT95317/SiT95316
 * Copyright (C) 2024
 * Author: SiTime Corporation
 *
 * The SiT95317 has 4 input & 8 output clocks.
 * The SiT95316 has 4 inputs & 16 output clocks.
 */

#include "sit95317.h"

// Global structure ... Specifically to be used for IOCTLs
// Will be removed once IOCTL support is removed
struct drv_si9531x *g_brd = NULL;

//Global VCO Frequency. To be removed later
u64 g_Fvco = FVCO_LOWBAND_FREQ_MIN;

static int si9531x_clkin_set_rate(struct clk_hw *hw, unsigned long rate, unsigned long parent_rate)
{
	printk(KERN_INFO "%s : rate : %ld\n", __func__, rate);
	return SUCCESS;
}

static int si9531x_clkin_is_enabled(struct clk_hw *hw)
{
	unsigned int ret = SUCCESS;
	struct si9531x_clk *clkdata = NULL;

	clkdata = to_clk_si9531x_data(hw);

	printk(KERN_INFO "%s : %d clkName :%s clkNum:%d status:%d \n", __func__, __LINE__, \
			clkdata->clkName, \
			clkdata->reg, \
			clkdata->status);

	ret = clkin_is_enable_disable(clkdata->data->client, clkdata->reg, ON, &ret);

	return ret;
}

static int si9531x_clkin_enable(struct clk_hw *hw)
{
	struct si9531x_clk *clkdata = NULL;
	clkdata = to_clk_si9531x_data(hw);

	printk(KERN_INFO "%s : %d clkName :%s clkNum:%d status:%d \n", __func__, __LINE__, \
			clkdata->clkName, \
			clkdata->reg, \
			clkdata->status);

	clkin_enable_disable(clkdata->data->client, clkdata->reg, ON);

	return SUCCESS;
}

static void si9531x_clkin_disable(struct clk_hw *hw)
{
	struct si9531x_clk *clkdata = NULL;
	clkdata = to_clk_si9531x_data(hw);

	printk(KERN_INFO "%s : %d clkName :%s clkNum:%d status:%d \n", __func__, __LINE__, \
			clkdata->clkName, \
			clkdata->reg, \
			clkdata->status);

	clkin_enable_disable(clkdata->data->client, clkdata->reg, OFF);

	return;
}

static unsigned long si9531x_clkin_recalc_rate(struct clk_hw *hw, unsigned long parent_rate)
{
	struct si9531x_clk *clkdata = NULL;

	clkdata = to_clk_si9531x_data(hw);

	printk(KERN_INFO "%s : %d clkName :%s clkNum:%d status:%d \n", __func__, __LINE__, \
			clkdata->clkName, \
			clkdata->reg, \
			clkdata->status);

	return clkdata->freq;
}

static long si9531x_clkin_round_rate(struct clk_hw *hw, unsigned long drate, unsigned long *prate)
{
	printk(KERN_INFO "%s : %ld\n", __func__, drate);
	//TBD : Need to check and return as per calculated limits
	return drate;
}

/* Powers up and then enables the output */
static int si9531x_clkout_prepare(struct clk_hw *hw)
{
	printk(KERN_INFO "%s : %d\n", __func__, __LINE__);
	return SUCCESS;
}

/* Disables and then powers down the output */
static void si9531x_clkout_unprepare(struct clk_hw *hw)
{
	printk(KERN_INFO "%s : %d\n", __func__, __LINE__);
	return ;
}

static unsigned long si9531x_clkout_recalc_rate(struct clk_hw *hw, unsigned long parent_rate)
{
	struct si9531x_clk *clkdata = NULL;
	unsigned int freq = 0;

	clkdata = to_clk_si9531x_data(hw);

	printk(KERN_INFO "%s : %d clkName :%s clkNum:%d status:%d parent_rate :%ld \n", __func__, __LINE__, \
			clkdata->clkName, \
			clkdata->reg, \
			clkdata->status,\
			parent_rate);

	freq = clkout_set_frequency(clkdata->data->client, clkdata->reg, clkdata->freq); 
	if (freq < 0) {
		printk(KERN_ERR "Set Clock freq failed for clkname : %s\n", clkdata->clkName);
		return 0;
	}

	return freq;
}

static int si9531x_clkout_set_rate(struct clk_hw *hw, unsigned long rate, unsigned long parent_rate)
{
	printk(KERN_INFO "%s : %d rate : %ld parent_rate: %ld \n", __func__, __LINE__, rate, parent_rate);
	return SUCCESS;
}

static int si9531x_clkout_is_enabled(struct clk_hw *hw)
{
	unsigned int ret = SUCCESS;
	struct si9531x_clk *clkdata = NULL;

	clkdata = to_clk_si9531x_data(hw);

	printk(KERN_INFO "%s : %d clkName :%s clkNum:%d status:%d \n", __func__, __LINE__, \
			clkdata->clkName, \
			clkdata->reg, \
			clkdata->status);

	ret = clkout_is_enable_disable(clkdata->data->client, clkdata->reg, ON, &ret);

	return ret;
}

static int si9531x_clkout_enable(struct clk_hw *hw)
{
	struct si9531x_clk *clkdata = NULL;
	clkdata = to_clk_si9531x_data(hw);	

	printk(KERN_INFO "%s : %d clkName :%s clkNum:%d status:%d \n", __func__, __LINE__, \
			clkdata->clkName, \
			clkdata->reg, \
			clkdata->status);

	clkout_enable_disable(clkdata->data->client, clkdata->reg, ON);
	return SUCCESS;
}

static void si9531x_clkout_disable(struct clk_hw *hw)
{
	struct si9531x_clk *clkdata = NULL;
	clkdata = to_clk_si9531x_data(hw);	

	printk(KERN_INFO "%s : %d clkName :%s clkNum:%d status:%d \n", __func__, __LINE__, \
			clkdata->clkName, \
			clkdata->reg, \
			clkdata->status);

	clkout_enable_disable(clkdata->data->client, clkdata->reg, OFF);
	return;
}

static long si9531x_clkout_round_rate(struct clk_hw *hw, unsigned long drate, unsigned long *prate)
{
	printk(KERN_INFO "%s : %ld\n", __func__, drate);
	//TBD : Need to check and return as per calculated limits
	return (long)drate;
}

static const struct clk_ops si9531x_output_clk_ops = {
	.prepare = si9531x_clkout_prepare,
	.unprepare = si9531x_clkout_unprepare,
	.set_rate = si9531x_clkout_set_rate,
	.enable = si9531x_clkout_enable,
	.disable = si9531x_clkout_disable,
	.is_enabled = si9531x_clkout_is_enabled,
	.round_rate = si9531x_clkout_round_rate,
	.recalc_rate = si9531x_clkout_recalc_rate
};

static const struct clk_ops si9531x_input_clk_ops = {
	.set_rate = si9531x_clkin_set_rate,
	.enable = si9531x_clkin_enable,
	.disable = si9531x_clkin_disable,
	.is_enabled = si9531x_clkin_is_enabled,
	.round_rate = si9531x_clkin_round_rate,
	.recalc_rate = si9531x_clkin_recalc_rate

};

long siIoctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	unsigned int ret = SUCCESS;
	unsigned int clkid = 0;
	unsigned int freq_plan_id = 1;
	struct ioctl_data data;
	struct i2c_client *client = g_brd->client;  
	struct freq_config config;

	printk(KERN_INFO"Ioctl Function CMD received :%d\n", cmd);
	switch(cmd)
	{
		case REG_READ:
			// Copy the data structure from user space
			if (copy_from_user(&data, (struct ioctl_data __user *)arg, sizeof(data))) {
				return -EFAULT;
			}

			printk(KERN_INFO "IOCTL Read - Page: 0x%x, Register: 0x%x\n", data.page, data.reg_address);

			// Read data from the specified register
			data.value = i2c_smbus_read_byte_data(client, data.reg_address);
			if (data.value < 0) {
				printk(KERN_ERR "Failed to read byte via SMBus\n");
				return -EIO;
			}

			// Copy the result back to user space
			if (copy_to_user((struct ioctl_data __user *)arg, &data, sizeof(data))) {
				return -EFAULT;
			}

			printk(KERN_INFO "Read value: 0x%x from register: 0x%x on page: 0x%x\n", data.value, data.reg_address, data.page);
			break;

		case REG_WRITE:
			// Copy the data structure from user space
			if (copy_from_user(&data, (struct ioctl_data __user *)arg, sizeof(data))) {
				return -EFAULT;
			}

			printk(KERN_INFO "IOCTL Write - Page: 0x%x, Register: 0x%x, Value: 0x%x\n", data.page, data.reg_address, data.value);

			// Write the data to the specified register
			ret = i2c_smbus_write_byte_data(client, data.reg_address, data.value);
			if (ret < 0) {
				printk(KERN_ERR "Failed to write byte via SMBus\n");
				return -EIO;
			}

			printk(KERN_INFO "Wrote value: 0x%x to register: 0x%x on page: 0x%x\n", data.value, data.reg_address, data.page);
			break;

		case SEL_FREQ_PLAN:
			// Copy the data structure from user space
			if (copy_from_user(&freq_plan_id, (struct ioctl_data __user *)arg, sizeof(freq_plan_id))) 
			{
				return -EFAULT;
			}
			printk(KERN_INFO "SEL_FREQ_PLAN - freq_plan_id: 0x%x \n", freq_plan_id);
			// set predefined frequency plan
			ret = set_frequency_plan(freq_plan_id);
			break;

		case CLK_ENABLE:

			if (copy_from_user(&clkid, (struct si9531x_clk_enable __user *)arg, sizeof(clkid))) 
				return -EFAULT;

			ret = clkout_enable_disable(client, clkid, ON);
			break;

		case CLK_DISABLE:

			if (copy_from_user(&clkid, (struct si9531x_clk_enable __user *)arg, sizeof(clkid))) 
				return -EFAULT;

			ret = clkout_enable_disable(client, clkid, OFF);
			break;

		case SET_FREQUENCY:
			if (copy_from_user(&config, (struct freq_config __user *)arg, sizeof(config))) 
				return -EFAULT;

			ret = clkout_set_frequency(client, config.clkid, config.output_frequency);
			printk(KERN_ERR "set rate return for Output clk: %d\n", ret);
			g_brd->output_clk[config.clkid].freq = ret;
			ret = clk_set_rate(g_brd->output_clk[config.clkid].hw.clk, ret);
			printk(KERN_ERR "set rate return :%d \n", ret);
			break; 		
		case SET_INPUT_FREQ:
			// Copy data from user space
			if (copy_from_user(&config, (struct freq_config __user *)arg, sizeof(config))) {
				return -EFAULT;
			}

			// Call the function to set the input frequency
			ret = clkin_set_frequency(client, config.clkid, config.input_frequency, config.output_frequency);
			if (ret < 0) {
				printk(KERN_ERR "Failed to set input frequency\n");
				return ret;
			}

			printk(KERN_INFO "Input frequency set to: %u Hz, Output frequency: %u Hz\n",
					config.input_frequency, config.output_frequency);
			break;

		default:
			printk(KERN_INFO"Ioctl for Default\n");
			break;
	}
	return SUCCESS;
}

int siOpen(struct inode *inode, struct file *filp)
{
	printk(KERN_INFO"%s: Open: module opened\n", gDrvrName);
	return SUCCESS;
}

int siRelease(struct inode *inode, struct file *filp)
{
	printk(KERN_INFO"%s: Release: module released\n", gDrvrName);
	return SUCCESS;
}

// Aliasing write, read, ioctl, etc...
struct file_operations siFileOpt = {
	.unlocked_ioctl	=	siIoctl,
	.open 		=	siOpen,
	.release 	=	siRelease,
};

static int CreateCharDevice(struct drv_si9531x *brd)
{
	/* Register with the kernel as a character device.*/
	int ret = alloc_chrdev_region(&brd->ldev_node, 0, 1, gDrvrName);
	if (ret < 0) {
		printk(KERN_ERR "Major number allocation is failed for si9531x driver\n");
		return ret;
	}
	brd->majorNum = MAJOR(brd->ldev_node);
	brd->minorNum = MINOR(brd->ldev_node);
	printk(KERN_INFO "The major number for SiTime 9531x CCF driveris %d minor: %d", brd->majorNum, brd->minorNum);

	brd->siT_cdev = cdev_alloc();
	if (!brd->siT_cdev) {

		printk(KERN_ERR "Failed to alloc cdev\n");
		unregister_chrdev_region(brd->ldev_node, 1);
		return -ENOMEM;
	}
	cdev_init(brd->siT_cdev, &siFileOpt);
	ret = cdev_add(brd->siT_cdev, brd->ldev_node, 1);
	if (ret < 0 ) {
		printk(KERN_ERR "Unable to allocate cdev for SiTime 9531x CCF driver dev\n");
		unregister_chrdev_region(brd->ldev_node, 1);
		return ret;
	}

	brd->siT_cl = class_create(THIS_MODULE, gDrvrName);
	if (!brd->siT_cl) {

		cdev_del(brd->siT_cdev);
		unregister_chrdev_region(brd->ldev_node, 1);
		printk(KERN_ERR "Failed to create class\n");
		return -EEXIST;
	}

	if (!device_create(brd->siT_cl, NULL, brd->ldev_node, NULL, gDrvrName)) {
		printk(KERN_ERR "Failed to create device\n");
		class_destroy(brd->siT_cl);
		cdev_del(brd->siT_cdev);
		unregister_chrdev_region(brd->ldev_node, 1);
		return  -EINVAL;
	}
	printk(KERN_ERR "Chardev done\n");
	return SUCCESS;
}

void print_clk_driver(struct drv_si9531x *brd)
{
	int index;

	printk(KERN_INFO "brd->chip_id=%x No of inputs:%d No of Outputs:%d\n", brd->chip_id, brd->num_inputs, brd->num_outputs);


	for (index = 0; index < brd->num_inputs; ++index) {
		printk(KERN_INFO "Input Clk:%s :clk_Index : %d status:%d, linktype:%d, freq=%d accuracy=%d\n",\
				brd->input_clk[index].clkName,	\
				brd->input_clk[index].reg,	\
				brd->input_clk[index].status,	\
				brd->input_clk[index].linktype,	\
				brd->input_clk[index].freq,	\
				brd->input_clk[index].accuracy);
	}

	for (index = 0; index < brd->num_outputs; ++index) {
		printk(KERN_INFO "Output clk:%s clk_Index : %d status:%d, linktype:%d, freq=%d accuracy=%d\n",\
				brd->output_clk[index].clkName, \
				brd->output_clk[index].reg, 	\
				brd->output_clk[index].status, 	\
				brd->output_clk[index].linktype,\
				brd->output_clk[index].freq, 	\
				brd->output_clk[index].accuracy);
	}
}

static int si9531x_dt_parse(struct drv_si9531x *brd)
{
	struct device_node *np = brd->client->dev.of_node;
	struct device_node *child,*xtal;
	unsigned int reg,index;
	unsigned int num;
	const char *mainxtal_name;

	if (of_property_read_u32(np, "reg", &reg) < 0)
		return -EINVAL;

	printk(KERN_INFO "Reg value read from DTS: 0x%x \n", reg);


	//Allocate structure element pointer memory
	brd->input_clk = kzalloc( sizeof(struct si9531x_clk)*brd->num_inputs, GFP_KERNEL);
	brd->output_clk = kzalloc(sizeof(struct si9531x_clk)*brd->num_outputs, GFP_KERNEL);
	brd->xtal_clk = kzalloc(sizeof(struct si9531x_clk), GFP_KERNEL);

	num = of_property_match_string(np, "clock-names", "xtal");
	if (num < 0)
		printk(KERN_INFO "xtal not found \n");
	else
		printk(KERN_INFO "xtal found at num:%d\n",num);

	mainxtal_name = of_clk_get_parent_name(np, num);
	printk(KERN_INFO "xtal name:%s\n", mainxtal_name);

	xtal = of_find_node_by_name(np, mainxtal_name);
	brd->xtal_clk->clkName = xtal->name;
	of_property_read_u32(xtal, "clock-frequency", &brd->xtal_clk->freq);
	printk(KERN_INFO "DTS : child_name:%s   freq read:%d\n",  brd->xtal_clk->clkName, brd->xtal_clk->freq);

	for_each_child_of_node(np, child) {
		printk(KERN_INFO "Node :%s  ChildName: %s \n", np->name, child->name);

		if (of_property_match_string(child, "clktype", "in") == 0) {
			if (of_property_read_u32(child, "reg", &index)) {
				dev_err(&brd->client->dev, "missing reg property of %s\n", child->name);
				break;
			}
			brd->input_clk[index].reg = index;
			brd->input_clk[index].clkName = child->name;	

			if (of_property_read_u32(child, "clock-frequency", &brd->input_clk[index].freq) < 0) 
				printk(KERN_INFO "DTS : %s: Clock freq not set \n", child->name);

			if (of_property_match_string(child, "status", "okay") == 0)
				brd->input_clk[index].status = ON;
			else
				brd->input_clk[index].status = OFF;

			if (of_property_match_string(child, "linktype", "single") == 0)
				brd->input_clk[index].linktype = SINGLE;
			else
				brd->input_clk[index].linktype = DIFFERENTIAL;

			if (of_property_read_u32(child, "clock-accuracy", &brd->input_clk[index].accuracy) < 0)
				printk(KERN_INFO "DTS : %s: Clock Accuracy not set \n",child->name);
		}

		if (of_property_match_string(child, "clktype", "out") == 0) {
			if (of_property_read_u32(child, "reg", &index)) {
				dev_err(&brd->client->dev, "missing reg property of %s\n", child->name);
				break;
			}

			brd->output_clk[index].reg = index;
			brd->output_clk[index].clkName = child->name;

			if (of_property_read_u32(child, "clock-frequency", &brd->output_clk[index].freq) < 0)
				printk(KERN_INFO "DTS : %s: Clock freq not set \n",child->name);

			if (of_property_match_string(child,"status", "okay") == 0)
				brd->output_clk[index].status = ON;
			else
				brd->output_clk[index].status = OFF;

			if (of_property_match_string(child, "linktype", "single") == 0)
				brd->output_clk[index].linktype = SINGLE;
			else
				brd->output_clk[index].linktype = DIFFERENTIAL;

			if (of_property_read_u32(child, "clock-accuracy", &brd->output_clk[index].accuracy) < 0) 
				printk(KERN_INFO "DTS : %s: Clock Accuracy not set \n", child->name);
		}
	}
	return SUCCESS;
}


static int soft_rst_por_chip(void)
{
	int ret  = 0;
	struct drv_si9531x *brd = g_brd;

	ret = siT9531x_write_multiple(brd,chip_rst_cfg, ARRAY_SIZE(chip_rst_cfg));
	if (ret < 0) 
		printk(KERN_INFO "chip reset reg_write failed\n");

	return ret;
}

static int set_frequency_plan(unsigned int plan_id)
{   
	int ret  = 0;
	struct drv_si9531x *brd = g_brd;

	//apply soft reset on chip
	soft_rst_por_chip();

	switch(plan_id)
	{
		case 1 : 	
			ret = siT9531x_write_multiple(brd, siT95317_def1, ARRAY_SIZE(siT95317_def1));
			if (ret < 0) 
				printk(KERN_INFO "Dfreq1_regcfg Write reg failed\n");

			break;

		case 2 : 	
			ret = siT9531x_write_multiple(brd, siT95317_lftdef4, ARRAY_SIZE(siT95317_lftdef4));
			if (ret < 0) 
				printk(KERN_INFO "Dfreq2_regcfg Write reg failed\n");

			break;

		default:
			printk(KERN_INFO "Not a valid frequency plan. select (1 or 2).\n");
			break; 
	} 

	return ret;
}

static int siT9531x_write_multiple(struct drv_si9531x *data,
		const struct  siT9531x_reg_cfg *values, unsigned int num_values)
{
	int res;
	int i;
	for (i = 0; i < num_values; ++i) {
		res = i2c_smbus_write_byte_data(data->client,
				values[i].address, values[i].value);
		if (res < 0)
			printk(KERN_INFO "Failed to write for client write %#x:%#x\n", values[i].address, values[i].value);
	}

	return res;
}

static bool clkin_is_enable_disable(struct i2c_client *client, int clkid, bool is_enabled, unsigned int *value)
{
	bool ret;
	//Make sure we are reading from main_sys page
	if (i2c_smbus_write_byte_data(client, SI95317_PAGE_NUM, Page0_Mainsys) < 0) {
		printk(KERN_ERR "Failed to write byte via SMBus\n");
		return -EIO;
	}

	//read clk input en/disable reg
	*value = i2c_smbus_read_byte_data(client, SI95317_INPUT_ENABLE);
	if(*value < 0) {
		printk(KERN_ERR "Failed to read byte via SMBus\n");
		return -EIO;

	}

	if (*value & (1 << clkid))
		ret = ON;
	else
		ret = OFF;

	printk(KERN_INFO "%s :value:%d clkid:%d ret:%d \n", __func__, *value, clkid, ret);
	return ret;
}

static int clkin_enable_disable(struct i2c_client *client, int clkid, bool is_enabled)
{
	int ret = SUCCESS;
	unsigned int value = 0;
	const unsigned int clkin_regvalue[4] = {0x01, 0x02, 0x04, 0x08};

	ret = clkin_is_enable_disable(client, clkid, is_enabled, &value);

	if (is_enabled) 
		value = value | clkin_regvalue[clkid];		// set bit of reg by clk_id
	else 
		value = value & (~clkin_regvalue[clkid]); 		// set bit of reg by clk_id

	/* unlock debug registers for writing */
	if (i2c_smbus_write_byte_data(client, SI95317_DEBUG_REG, SI95317_UNLOCK_DEBUG_REG) < 0) {
		printk(KERN_INFO "I2C_Write Error : input clk enabled failed\n");
		return -EIO;
	}

	/* Change state from Active to escape_to_PRG_CMD */
	if (i2c_smbus_write_byte_data(client, SI95317_PRG_DIR_GEN_0, SI95317_PRGCMD_STATE) < 0) {
		printk(KERN_INFO "I2C_Write Error : input clk enabled failed\n");
		return -EIO;
	}

	// write to enable/disable the input clock
	if (i2c_smbus_write_byte_data(client, SI95317_INPUT_ENABLE, value) < 0) {
		printk(KERN_INFO "I2C_Write Error :input clk enabled failed\n");
		return -EIO;
	}

	// update nvm bank & change to READY state
	if (i2c_smbus_write_byte_data(client, SI95317_PRG_DIR_GEN_0, SI95317_UPDATE_NVM) < 0) {
		printk(KERN_INFO "I2C_Write Error :input clk enabled failed\n");
		return -EIO;
	}

	// proceed to loop lock
	if (i2c_smbus_write_byte_data(client, SI95317_PRG_DIR_GEN_0, SI95317_LOOPLOCK) < 0) {
		printk(KERN_INFO "I2C_Write Error :input clk enabled failed\n");
		return -EIO;
	}

	msleep(100);

	return SUCCESS;
}

static bool clkout_is_enable_disable(struct i2c_client *client, int clkid, bool is_enabled, unsigned int* value)
{
	unsigned int reg_address;
	bool ret;

	//TBD : Define 0x25 and 0x24 as #defines
	if (clkid < 5)
		reg_address = 0x25;
	else
		reg_address = 0x24;


	//Make sure we are reading from main_sys page
	if (i2c_smbus_write_byte_data(client, SI95317_PAGE_NUM, Page0_Mainsys) < 0) {
		printk(KERN_ERR "Failed to write byte via SMBus\n");
		return -EIO;
	}

	//read clk out en/disable reg
	*value = i2c_smbus_read_byte_data(client, reg_address); 
	if (*value < 0) {
		printk(KERN_ERR "Failed to read byte via SMBus\n");
		return -EIO;

	}

	if (!((*value & clkout_regvalue[clkid]) ^ clkout_regvalue[clkid]))
		ret = ON;
	else
		ret = OFF;	
	printk(KERN_INFO "%s :value:%d\n", __func__, *value);
	return ret;
}

static int clkout_enable_disable(struct i2c_client *client, int clkid, bool is_enabled)
{
	int ret = SUCCESS;
	unsigned int value = 0;
	unsigned int reg_address = 0;

	printk(KERN_INFO "is_enabled :%d clkid:%d clkout_regvalue: %d \n", is_enabled, clkid, clkout_regvalue[clkid]);


	if (clkid < 5)
		reg_address = 0x25;
	else
		reg_address = 0x24;

	ret = clkout_is_enable_disable(client, clkid, is_enabled, &value);

	if (is_enabled) 
		value = value | clkout_regvalue[clkid];		// set bit of reg by clk_id
	else 
		value = value & (~clkout_regvalue[clkid]); 		// set bit of reg by clk_id

	/* unlock debug registers for writing */
	if (i2c_smbus_write_byte_data(client, SI95317_DEBUG_REG, SI95317_UNLOCK_DEBUG_REG) < 0) {
		printk(KERN_INFO "I2C_Write Error : output clk enabled failed\n");
		return -EIO;
	}

	/* Change state from Active to escape_to_PRG_CMD */
	if (i2c_smbus_write_byte_data(client, SI95317_PRG_DIR_GEN_0, SI95317_PRGCMD_STATE) < 0) {
		printk(KERN_INFO "I2C_Write Error : output clk enabled failed\n");
		return -EIO;
	}

	// write to enable/disable the out clock
	if (i2c_smbus_write_byte_data(client, reg_address, value) < 0) {
		printk(KERN_INFO "I2C_Write Error :output clk enabled failed\n");
		return -EIO;
	}

	// update nvm bank & change to READY state
	if (i2c_smbus_write_byte_data(client, SI95317_PRG_DIR_GEN_0, SI95317_UPDATE_NVM) < 0) {
		printk(KERN_INFO "I2C_Write Error :utput clk enabled failed\n");
		return -EIO;
	}

	// proceed to loop lock
	if (i2c_smbus_write_byte_data(client, SI95317_PRG_DIR_GEN_0, SI95317_LOOPLOCK) < 0) {
		printk(KERN_INFO "I2C_Write Error :utput clk enabled failed\n");
		return -EIO;
	}

	msleep(100);

	return SUCCESS;
}

static int clkout_set_frequency(struct i2c_client *client, int clkid, unsigned int frequency)
{
	unsigned int FIXED_POINT_PRECISION_VALUE = 1000;
	unsigned int page_num = 0;
	unsigned int reg_address = 0;
	unsigned int div4reg_value = 0;
	int i,j;
	u8 ODRx_DIV[5] = {0};
	u64 vco_freq_band;
	u64 DIVO;  // Use DIVO for the output divider
	u64 divo_temp;


	static const unsigned int siT95317Output2siT95316RegMap[] = {0, 3, 4, 5, 7, 8, 9, 11};

	/* Set page to MAIN OUTSYS (Page3_Outsys) */
	if (i2c_smbus_write_byte_data(client, SI95317_PAGE_NUM, Page3_Outsys) < 0) {
		printk(KERN_ERR "Failed to write byte via SMBus\n");
		return -EIO;
	}

	/* Unlock debug registers for writing */
	if (i2c_smbus_write_byte_data(client, SI95317_DEBUG_REG, SI95317_UNLOCK_DEBUG_REG) < 0) {
		printk(KERN_INFO "I2C_Write Error: output clk enable failed\n");
		return -EIO;
	}

	/* Change state from Active to escape_to_PRG_CMD */
	if (i2c_smbus_write_byte_data(client, SI95317_PRG_DIR_GEN_0, SI95317_PRGCMD_STATE) < 0) {
		printk(KERN_INFO "I2C_Write Error: output clk enable failed\n");
		return -EIO;
	}

	/* Switch to page 4 if output clock id is greater than 5 */
	if (siT95317Output2siT95316RegMap[clkid] > 5) {
		if (i2c_smbus_write_byte_data(client, SI95317_PAGE_NUM, Page4_Outsys) < 0) {
			printk(KERN_INFO "I2C_Write Error: output clk enable failed\n");
			return -EIO;
		}
	}

	if (( clkid == 0 ) || ( clkid == 1 ) || ( clkid == 4 ) || ( clkid == 5 )) {
		vco_freq_band = FVCO_HIGHBAND_FREQ_MIN;
	}
	else if (( clkid == 2 ) || ( clkid == 3 ) || ( clkid == 6 ) || ( clkid == 7 ))
	{
		vco_freq_band = FVCO_LOWBAND_FREQ_MIN;
	}
	/* Calculate the DIVO value */
	divo_temp = (vco_freq_band * FIXED_POINT_PRECISION_VALUE) / frequency;
	DIVO = divo_temp / FIXED_POINT_PRECISION_VALUE;
	if ((divo_temp % FIXED_POINT_PRECISION_VALUE) > 0) {
		DIVO = DIVO + 1;
	}

	g_Fvco = DIVO * frequency;

	/* Prepare the divider values for register write */
	ODRx_DIV[4] = (DIVO >> 32) & 0xff;
	ODRx_DIV[3] = (DIVO >> 24) & 0xff;
	ODRx_DIV[2] = (DIVO >> 16) & 0xff;
	ODRx_DIV[1] = (DIVO >> 8) & 0xff;
	ODRx_DIV[0] = (DIVO >> 0) & 0xff;

	reg_address = clkout_odr_divn_regvalue[siT95317Output2siT95316RegMap[clkid] % 6];

	/* Read the existing value for the high byte of the divider */
	div4reg_value = i2c_smbus_read_byte_data(client, reg_address + 4);
	if (div4reg_value < 0) {
		printk(KERN_ERR "Failed to read byte via SMBus\n");
		return -EIO;
	}

	ODRx_DIV[4] |= div4reg_value;  // Combine with the new value

	/* Debug output */
	printk(KERN_INFO "clkid: %d\t Fvco: %lld\t frequency to set: %d\tcalculated DIVO: %llu\n", clkid, g_Fvco, frequency, DIVO);
	printk(KERN_INFO "Page_num: %d reg_addr: %02x\n", page_num, reg_address);


	/* Write the divider values to the registers */
	for (j = 0; j < 5; j++) {
		if (i2c_smbus_write_byte_data(client, reg_address - j, ODRx_DIV[j]) < 0) {
			printk(KERN_INFO "I2C_Write Error: output clk enable failed\n");
			return -EIO;
		}
	}

	/* Set page back to MAIN OUTSYS (Page3_Outsys) */
	if (i2c_smbus_write_byte_data(client, SI95317_PAGE_NUM, Page3_Outsys) < 0) {
		printk(KERN_ERR "Failed to write byte via SMBus\n");
		return -EIO;
	}

	/* Update the NVM bank and change to READY state */
	if (i2c_smbus_write_byte_data(client, SI95317_PRG_DIR_GEN_0, SI95317_UPDATE_NVM) < 0) {
		printk(KERN_INFO "I2C_Write Error: output clk enable failed\n");
		return -EIO;
	}

	/* Proceed to loop lock */
	if (i2c_smbus_write_byte_data(client, SI95317_PRG_DIR_GEN_0, SI95317_LOOPLOCK) < 0) {
		printk(KERN_INFO "I2C_Write Error: output clk enable failed\n");
		return -EIO;
	}

	msleep(100);  // Delay for lock to take effect

	return frequency;
}


static int switch_to_PRGCmd_state(struct i2c_client *client,unsigned int page_num) 
{
	int ret = 0;

	/* Set page to MAIN INPUTSYS (Page2_Inputsys) */
	if (i2c_smbus_write_byte_data(client, SI95317_PAGE_NUM, page_num) < 0) {
		printk(KERN_ERR "Failed to write byte via SMBus\n");
		return -EIO;
	}

	/* Unlock debug registers for writing */
	if (i2c_smbus_write_byte_data(client, SI95317_DEBUG_REG, SI95317_UNLOCK_DEBUG_REG) < 0) {
		printk(KERN_INFO "I2C_Write Error: output clk enable failed\n");
		return -EIO;
	}

	/* Change state from Active to escape_to_PRG_CMD */
	if (i2c_smbus_write_byte_data(client, SI95317_PRG_DIR_GEN_0, SI95317_PRGCMD_STATE) < 0) {
		printk(KERN_INFO "I2C_Write Error: output clk enable failed\n");
		return -EIO;
	}

	return ret;
}

static int do_updatenvm_lockloop(struct i2c_client *client)
{
	int ret = 0;

	/* Update the NVM bank and change to READY state */
	if (i2c_smbus_write_byte_data(client, SI95317_PRG_DIR_GEN_0, SI95317_UPDATE_NVM) < 0) {
		printk(KERN_INFO "I2C_Write Error: output clk enable failed\n");
		return -EIO;
	}

	/* Proceed to loop lock */
	if (i2c_smbus_write_byte_data(client, SI95317_PRG_DIR_GEN_0, SI95317_LOOPLOCK) < 0) {
		printk(KERN_INFO "I2C_Write Error: output clk enable failed\n");
		return -EIO;
	}

	msleep(100);  // Delay for lock to take effect

	return ret; 
}

static u16 calculate_divn1(unsigned int input_freq_val)
{
	// Step 1: Ensure the input frequency is divided down to <= 10 MHz for the PLL phase detector
	u16 divn1_int = 1;
	unsigned int divn1_frac;

	// Step 2: Configure the PLL input divider (DIVN1) for input frequency
	if (input_freq_val > FIN_PLL_FREQ_MAX) {
		divn1_int = (input_freq_val / FIN_PLL_FREQ_MAX);

		divn1_frac = divn1_int % 10;
		if (divn1_frac > 0) {
			divn1_int = divn1_int + 1;
		}
	}

	return divn1_int;
}


static int write_divn1_to_registers(struct i2c_client *client,unsigned int clkid,u16 divn1_value)
{
	int i;
	int ret = 0;
	unsigned int base_reg_address[4] = {0x10, 0x20, 0x30, 0x40};
	unsigned int base_reg_address_num[4] = {0x12, 0x22, 0x32, 0x42};
	unsigned int base_reg_address_den[4] = {0x16, 0x26, 0x36, 0x46};
	
	
	u8 divn1_bytes[2];
	divn1_bytes[0] = divn1_value & 0xff;
	divn1_bytes[1] = (divn1_value >> 8) & 0xff;

	printk(KERN_INFO "DIVN1 bytes: [%02x %02x]\n", divn1_bytes[0], divn1_bytes[1]);


	// Escape to PRGCmd State
	ret = switch_to_PRGCmd_state(client,Page2_Inputsys);
	if (ret < 0) {
		printk(KERN_ERR "Failed to Escape to PRGCmd State");
		return ret;
	}

	// e.g. the register base address for DIVN1 is 0x40 for CLKIN3
	// write 0x00 to numerator and 0xff to denominator
	for (i = 0; i < 2; i++) {
		printk(KERN_INFO "Writing DIVN1 byte %d: 0x%02x to register 0x%02x\n", i, divn1_bytes[i], base_reg_address[clkid] + i);
		ret = i2c_smbus_write_byte_data(client, base_reg_address[clkid] + i, divn1_bytes[i]);
		if (ret < 0) {
			printk(KERN_ERR "Failed to write DIVN1 value at index %d\n", i);
			return ret;
		}
	}
	
	// USING INTEGER DIVN1. FRAC PART IS 0
	for (i = 0; i < 4; i++) {
		ret = i2c_smbus_write_byte_data(client, base_reg_address_num[clkid] + i, 0x00);
		if (ret < 0) {
			printk(KERN_ERR "Failed to write DIVN1 numerator value at index %d\n", i);
			return ret;
		}
	}

	for (i = 0; i < 4; i++) {
		ret = i2c_smbus_write_byte_data(client, base_reg_address_den[clkid] + i, 0xff);
		if (ret < 0) {
			printk(KERN_ERR "Failed to write DIVN1 denominator value at index %d\n", i);
			return ret;
		}
	}  

	//update NVM Bank & Proceed to Loop Lock
	ret = do_updatenvm_lockloop(client);
	if (ret < 0) {
		printk(KERN_ERR "Failed to update NVM Bank and Lock Loop\n");
		return ret;
	}

	return SUCCESS;
}


static struct divider_data calculate_divn(unsigned int Fref_value, u64 vco_freq)
{

	unsigned int FIXED_POINT_PRECISION_VALUE = 1000;
	unsigned int divn_int;
	u64 divn_fracn, divn_fracd;
	u64 DIVN;
	struct divider_data divn;

	DIVN = ( vco_freq * FIXED_POINT_PRECISION_VALUE ) / Fref_value;

	divn_int = DIVN / FIXED_POINT_PRECISION_VALUE;

	divn_fracn = DIVN % FIXED_POINT_PRECISION_VALUE;
	divn_fracd = FIXED_POINT_PRECISION_VALUE;

	divn.int_part = divn_int;
	divn.fracn = divn_fracn;
	divn.fracd = divn_fracd;

	return divn;
}


static int write_divn_to_registers(struct i2c_client *client, unsigned int pllid, struct divider_data *divn)
{
	int i;
	int ret = 0;

	u8 divn_int_byte;  
	u8 divn_fracn_bytes[4] = {0};
	u8 divn_fracd_bytes[4] = {0};
	u64 DIVN_INT, DIVN_FRACN, DIVN_FRACD;

 
	unsigned int reg_address = 0x30;
	unsigned int reg_address_num = 0x32;
	unsigned int reg_address_den = 0x38;

	//Integer
	DIVN_INT   = divn->int_part;
	divn_int_byte = (DIVN_INT >> 0) & 0xff;

	//Numerator
	DIVN_FRACN = divn->fracn;

	divn_fracn_bytes[3] = (DIVN_FRACN >> 24) & 0xff;
	divn_fracn_bytes[2] = (DIVN_FRACN >> 16) & 0xff;
	divn_fracn_bytes[1] = (DIVN_FRACN >> 8) & 0xff;
	divn_fracn_bytes[0] = (DIVN_FRACN >> 0) & 0xff;

	//Denominator
	DIVN_FRACD = divn->fracd;

	divn_fracd_bytes[3] = (DIVN_FRACD >> 24) & 0xff;
	divn_fracd_bytes[2] = (DIVN_FRACD >> 16) & 0xff;
	divn_fracd_bytes[1] = (DIVN_FRACD >> 8) & 0xff;
	divn_fracd_bytes[0] = (DIVN_FRACD >> 0) & 0xff;

	// Escape to PRGCmd State
	ret = switch_to_PRGCmd_state(client, PageA_PLLA + pllid);
	if (ret < 0) {
		printk(KERN_ERR "Failed to Escape to PRGCmd State");
		return ret;
	}

	//write divn to registers
	ret = i2c_smbus_write_byte_data(client, reg_address, divn_int_byte);
	if (ret < 0) {
		printk(KERN_ERR "Failed to write DIVN value\n");
		return ret;
	}

	for (i = 0; i < 4; i++) {
		ret = i2c_smbus_write_byte_data(client, reg_address_num + i, divn_fracn_bytes[i]);
		if (ret < 0) {
			printk(KERN_ERR "Failed to write DIVN1 numerator value at index %d\n", i);
			return ret;
		}
	}
	for (i = 0; i < 4; i++) {
		ret = i2c_smbus_write_byte_data(client, reg_address_den + i, divn_fracd_bytes[i]);
		if (ret < 0) {
			printk(KERN_ERR "Failed to write DIVN1 denominator value at index %d\n", i);
			return ret;
		}
	}  

	//update NVM Bank & Proceed to Loop Lock
	ret = do_updatenvm_lockloop(client);
	if (ret < 0) {
		printk(KERN_ERR "Failed to update NVM Bank and Lock Loop\n");
		return ret;
	}

	return SUCCESS;
}

static struct divider_data calculate_divn2(unsigned int Fin_value, u64 vco_freq)
{

	unsigned int FIXED_POINT_PRECISION_VALUE = 1000;
	unsigned int divn2_int;
	struct divider_data divn2;
	u64 divn2_fracn, divn2_fracd;
	u64 DIVN2;

	DIVN2 = (vco_freq * FIXED_POINT_PRECISION_VALUE) / Fin_value;

	divn2_int = DIVN2 / FIXED_POINT_PRECISION_VALUE;

	divn2_fracn = DIVN2 % FIXED_POINT_PRECISION_VALUE;
	divn2_fracd = FIXED_POINT_PRECISION_VALUE;

	divn2.int_part = divn2_int;
	divn2.fracn = divn2_fracn;
	divn2.fracd = divn2_fracd;

	return divn2;
}


static int write_divn2_to_registers(struct i2c_client *client, unsigned int pllid, struct divider_data *divn2)
{
	int i;
	int ret = 0;
	u8 divn2_int_bytes[5] = {0};  
	u8 divn2_fracd_bytes[4] = {0};

	u8 divn2_fracn_bytes[4] = {0};
	u64 DIVN2_INT, DIVN2_FRACN, DIVN2_FRACD;
	unsigned int divn2int_reg4_value;
	unsigned int reg_address = 0x3e;
	unsigned int reg_address_num = 0x43;
	unsigned int reg_address_den = 0x49;

	//Integer
	DIVN2_INT   = divn2->int_part;

	divn2_int_bytes[0] = (DIVN2_INT >> 0) & 0xff;
	divn2_int_bytes[1] = (DIVN2_INT >> 8) & 0xff;
	divn2_int_bytes[2] = (DIVN2_INT >> 16) & 0xff;
	divn2_int_bytes[3] = (DIVN2_INT >> 24) & 0xff;
	divn2_int_bytes[4] = (DIVN2_INT >> 32) & 0xff;

	divn2int_reg4_value = i2c_smbus_read_byte_data(client, 0x42);
	if (divn2int_reg4_value < 0) {
		printk(KERN_ERR "Failed to read byte via SMBus\n");
		return -EIO;
	}

	divn2_int_bytes[4] |= divn2int_reg4_value;  

	printk(KERN_INFO "DIVN2 bytes: [");
	for (i = 0 ; i < 5; i++) {
		printk(KERN_INFO " %02x", divn2_int_bytes[i]);
	}
	printk(KERN_INFO "]\n");    

	//Numerator
	DIVN2_FRACN = divn2->fracn;

	divn2_fracn_bytes[3] = (DIVN2_FRACN >> 24) & 0xff;
	divn2_fracn_bytes[2] = (DIVN2_FRACN >> 16) & 0xff;
	divn2_fracn_bytes[1] = (DIVN2_FRACN >> 8) & 0xff;
	divn2_fracn_bytes[0] = (DIVN2_FRACN >> 0) & 0xff;

	//Denominator
	DIVN2_FRACD = divn2->fracd;

	divn2_fracd_bytes[3] = (DIVN2_FRACD >> 24) & 0xff;
	divn2_fracd_bytes[2] = (DIVN2_FRACD >> 16) & 0xff;
	divn2_fracd_bytes[1] = (DIVN2_FRACD >> 8) & 0xff;
	divn2_fracd_bytes[0] = (DIVN2_FRACD >> 0) & 0xff;

	// Escape to PRGCmd State
	ret = switch_to_PRGCmd_state(client, PageA_PLLA + pllid);
	if (ret < 0) {
		printk(KERN_ERR "Failed to Escape to PRGCmd State");
		return ret;
	}

	// write divn2 to registers
	for (i = 0; i < 5; i++) {
		ret = i2c_smbus_write_byte_data(client, reg_address + i, divn2_int_bytes[i]);
		if (ret < 0) {
			printk(KERN_ERR "Failed to write DIVN2 value at index %d\n", i);
			return ret;
		}
	}

	for (i = 0; i < 4; i++) {
		ret = i2c_smbus_write_byte_data(client, reg_address_num + i, divn2_fracn_bytes[i]);
		if (ret < 0) {
			printk(KERN_ERR "Failed to write DIVN2 numerator value at index %d\n", i);
			return ret;
		}
	}

	for (i = 0; i < 4; i++) {
		ret = i2c_smbus_write_byte_data(client, reg_address_den + i, divn2_fracd_bytes[i]);
		if (ret < 0) {
			printk(KERN_ERR "Failed to write DIVN2 denominator value at index %d\n", i);
			return ret;
		}
	}  

	//update NVM Bank & Proceed to Loop Lock
	ret = do_updatenvm_lockloop(client);
	if (ret < 0) {
		printk(KERN_ERR "Failed to update NVM Bank and Lock Loop\n");
		return ret;
	}

	return SUCCESS;
}

static int select_pll_page(struct i2c_client *client, int pll_id) {
	int ret;
	u8 pll_page;

	// Select the appropriate page based on pll_id
	switch (pll_id) {
		case 0:  // PLL A
			pll_page = PageA_PLLA;  // Page A
			break;
		case 1:  // PLL B
			pll_page = PageB_PLLB;  // Page B
			break;
		case 2:  // PLL C
			pll_page = PageC_PLLC;  // Page C
			break;
		case 3:  // PLL D
			pll_page = PageD_PLLD;  // Page D
			break;
		default:
			printk(KERN_ERR "Invalid PLL ID: %d\n", pll_id);
			return -EINVAL;
	}

	// Write to the page selection register (assumed register is 0x00 for page select)
	printk(KERN_INFO "Selecting PLL page: 0x%02x for PLL ID: %d\n", pll_page, pll_id);
	ret = i2c_smbus_write_byte_data(client, 0x00, pll_page);
	if (ret < 0) {
		printk(KERN_ERR "Failed to select PLL page 0x%02x for PLL ID: %d\n", pll_page, pll_id);
		return ret;
	}

	return SUCCESS;
}

static int set_fast_frequency(struct i2c_client *client, unsigned int input_frequency) {
	int ret;
	int i;

	// Find the correct entry in the frequency table
	const struct freq_range *range = NULL;
	for (i = 0; i < ARRAY_SIZE(freq_table); i++) {
		if (input_frequency >= freq_table[i].min_freq && input_frequency <= freq_table[i].max_freq) {
			range = &freq_table[i];
			break;
		}
	}

	if (!range) {
		printk(KERN_ERR "Input frequency %u Hz is out of range\n", input_frequency);
		return -EINVAL;
	}

	printk(KERN_INFO "Setting fast frequency for input %u Hz with values: 0x%02x, 0x%02x, 0x%02x\n",
			input_frequency, range->reg_13_value, range->reg_14_value, range->reg_15_value);

	// Write to register 0x13
	ret = i2c_smbus_write_byte_data(client, 0x13, range->reg_13_value);
	if (ret < 0) {
		printk(KERN_ERR "Failed to write to register 0x13\n");
		return ret;
	}

	// Write to register 0x14
	ret = i2c_smbus_write_byte_data(client, 0x14, range->reg_14_value);
	if (ret < 0) {
		printk(KERN_ERR "Failed to write to register 0x14\n");
		return ret;
	}

	// Write to register 0x15
	ret = i2c_smbus_write_byte_data(client, 0x15, range->reg_15_value);
	if (ret < 0) {
		printk(KERN_ERR "Failed to write to register 0x15\n");
		return ret;
	}

	printk(KERN_INFO "Fast frequency successfully set for input frequency %u Hz\n", input_frequency);
	return SUCCESS;
}

static int set_normal_frequency(struct i2c_client *client, unsigned int input_frequency) {
	int ret;
	int i;

	// Find the correct entry in the frequency table
	const struct freq_range *range = NULL;
	for (i = 0; i < ARRAY_SIZE(freq_table); i++) {
		if (input_frequency >= freq_table[i].min_freq && input_frequency <= freq_table[i].max_freq) {
			range = &freq_table[i];
			break;
		}
	}

	if (!range) {
		printk(KERN_ERR "Input frequency %u Hz is out of range\n", input_frequency);
		return -EINVAL;
	}

	printk(KERN_INFO "Setting normal frequency for input %u Hz with values: 0x%02x, 0x%02x, 0x%02x\n",
			input_frequency, range->normal_reg_10_value, range->normal_reg_11_value, range->normal_reg_12_value);

	// Write to register 0x10
	ret = i2c_smbus_write_byte_data(client, 0x10, range->normal_reg_10_value);
	if (ret < 0) {
		printk(KERN_ERR "Failed to write to register 0x10\n");
		return ret;
	}

	// Write to register 0x11
	ret = i2c_smbus_write_byte_data(client, 0x11, range->normal_reg_11_value);
	if (ret < 0) {
		printk(KERN_ERR "Failed to write to register 0x11\n");
		return ret;
	}

	// Write to register 0x12
	ret = i2c_smbus_write_byte_data(client, 0x12, range->normal_reg_12_value);
	if (ret < 0) {
		printk(KERN_ERR "Failed to write to register 0x12\n");
		return ret;
	}

	printk(KERN_INFO "Normal frequency successfully set for input frequency %u Hz\n", input_frequency);
	return SUCCESS;
}

static int set_accuracy(struct i2c_client *client, unsigned int accuracy) {
	int ret;
	u8 threshold_value = 0;

	if (accuracy <= 100) {
		threshold_value = PPM_100;
	} else if (accuracy <= 200) {
		threshold_value = PPM_200;
	} else if (accuracy <= 300) {
		threshold_value = PPM_300;
	} else if (accuracy <= 400) {
		threshold_value = PPM_400;
	} else if (accuracy <= 500) {
		threshold_value = PPM_500;
	} else if (accuracy <= 600) {
		threshold_value = PPM_600;
	} else if (accuracy <= 700) {
		threshold_value = PPM_700;
	} else if (accuracy <= 800) {
		threshold_value = PPM_800;
	} else if (accuracy <= 900) {
		threshold_value = PPM_900;
	} else if (accuracy <= 1000) {
		threshold_value = PPM_1000;
	} else if (accuracy <= 1100) {
		threshold_value = PPM_1100;
	} else if (accuracy <= 1200) {
		threshold_value = PPM_1200;
	} else if (accuracy <= 1300) {
		threshold_value = PPM_1300;
	} else if (accuracy <= 1400) {
		threshold_value = PPM_1400;
	} else if (accuracy <= 1500) {
		threshold_value = PPM_1500;
	} else {
		threshold_value = PPM_1600;
	}
        
        if (i2c_smbus_write_byte_data(client, SI95317_PAGE_NUM, Page6_Clkmon) < 0) {
		printk(KERN_ERR "Failed to write byte via SMBus\n");
		return -EIO;
	}
        	/* Unlock debug registers for writing */
	if (i2c_smbus_write_byte_data(client, SI95317_DEBUG_REG, SI95317_UNLOCK_DEBUG_REG) < 0) {
		printk(KERN_INFO "Failed to write byte via SMBus\n\n");
		return -EIO;
	}

	/* Change state from Active to escape_to_PRG_CMD */
	if (i2c_smbus_write_byte_data(client, SI95317_PRG_DIR_GEN_0, SI95317_PRGCMD_STATE) < 0) {
		printk(KERN_INFO "Failed to write byte via SMBus\n\n");
		return -EIO;
	}   


	ret = i2c_smbus_write_byte_data(client, 0x40, (threshold_value << 4));
	if (ret < 0) {
		printk(KERN_ERR "Failed to write coarse drift threshold\n");
		return ret;
	}        

	/* Update the NVM bank and change to READY state */
	if (i2c_smbus_write_byte_data(client, SI95317_PRG_DIR_GEN_0, SI95317_UPDATE_NVM) < 0) {
		printk(KERN_INFO "Failed to write byte via SMBus\n\n");
		return -EIO;
	}

	/* Proceed to loop lock */
	if (i2c_smbus_write_byte_data(client, SI95317_PRG_DIR_GEN_0, SI95317_LOOPLOCK) < 0) {
		printk(KERN_INFO "Failed to write byte via SMBus\n\n");
		return -EIO;
	}

    msleep(100);
        
	printk(KERN_INFO "Coarse drift threshold set to %u ppm (accuracy: %u)\n", threshold_value * 100, accuracy);
	return SUCCESS;
}

static int clkin_set_frequency(struct i2c_client *client, int clkid, unsigned int input_frequency, unsigned int output_frequency)
{

	int ret, pllid;
	u16 divn1_value;
	u64 Fin;

	struct divider_data divn;
	struct divider_data divn2;

	printk(KERN_INFO "clkin_set_frequency called for clkid: %d\n", clkid);
	printk(KERN_INFO "Input frequency: %u Hz, Output frequency: %u Hz\n", input_frequency, output_frequency);


	if (clkid == 0 || clkid == 1) {
		pllid = 0x03;//PLLD
	}
	else if (clkid == 2 || clkid == 3) {
		pllid = 0x02; //PLLC
	}
	else if (clkid == 4 || clkid == 5) {
		pllid = 0x01; //PLLB
	}
	else if (clkid == 6 || clkid == 7) {
		pllid = 0x00; // PLLA
	}

	divn1_value = calculate_divn1(input_frequency);
	write_divn1_to_registers(client, pllid, divn1_value);

	clkout_set_frequency(client, clkid, output_frequency);

	divn = calculate_divn(g_brd->xtal_clk->freq,g_Fvco);
	write_divn_to_registers(client, pllid, &divn);

	Fin = input_frequency / divn1_value;
	divn2 = calculate_divn2(Fin,g_Fvco);
	write_divn2_to_registers(client, pllid, &divn2);


	ret = set_fast_frequency(client, input_frequency);
	if (ret < 0) {
		printk(KERN_ERR "Failed to set fast lock bandwidth\n");
		return ret;
	}
	ret = set_normal_frequency(client, input_frequency);
	if(ret < 0) {
		return ret;
	}

	ret = set_accuracy(client, g_brd->input_clk[3].accuracy);

	return SUCCESS;
}

static int clkin_set_doubler(struct i2c_client *client, unsigned int xtal_ref_freq)
{
	int ret;
	// double enabled : 0 ; doubler disabled : 1
	int doubler, mask;
	unsigned int value;
	unsigned int xo2_generic_rdvalue;


	if ((xtal_ref_freq >= DOUBLER_XTAL_FREQ_MIN) && (xtal_ref_freq <= DOUBLER_XTAL_FREQ_MAX)) {
		printk(KERN_INFO "doubler enabled for %d xtal frequency as recommended\n", xtal_ref_freq);
		doubler = 0; 
		mask = 0x7f;
	}
	else {

		doubler = 1;
		mask    = 0xff;
	}


	//Make sure we are reading from main_sys page
	if (i2c_smbus_write_byte_data(client, SI95317_PAGE_NUM, Page0_Mainsys) < 0) {
		printk(KERN_ERR "Failed to write byte via SMBus\n");
		return -EIO;
	}

	xo2_generic_rdvalue = i2c_smbus_read_byte_data(client, 0x2d);
	if (xo2_generic_rdvalue < 0) {
		printk(KERN_ERR "Failed to read byte via SMBus\n");
		return -EIO;
	}

	value = xo2_generic_rdvalue & mask;

	/* Set page to MAIN PAGE0 (Page0_Mainsys) */
	if (i2c_smbus_write_byte_data(client, SI95317_PAGE_NUM, Page0_Mainsys) < 0) {
		printk(KERN_ERR "Failed to write byte via SMBus\n");
		return -EIO;
	}

	/* Unlock debug registers for writing */
	if (i2c_smbus_write_byte_data(client, SI95317_DEBUG_REG, SI95317_UNLOCK_DEBUG_REG) < 0) {
		printk(KERN_INFO "Failed to write byte via SMBus\n\n");
		return -EIO;
	}

	/* Change state from Active to escape_to_PRG_CMD */
	if (i2c_smbus_write_byte_data(client, SI95317_PRG_DIR_GEN_0, SI95317_PRGCMD_STATE) < 0) {
		printk(KERN_INFO "Failed to write byte via SMBus\n\n");
		return -EIO;
	}   


	ret = i2c_smbus_write_byte_data(client, SIT95317_XO2_GENERIC_REG, value);
	if (ret < 0) {
		printk(KERN_ERR "Failed to set doubler bit\n");
		return ret;
	}         

	/* Update the NVM bank and change to READY state */
	if (i2c_smbus_write_byte_data(client, SI95317_PRG_DIR_GEN_0, SI95317_UPDATE_NVM) < 0) {
		printk(KERN_INFO "Failed to write byte via SMBus\n\n");
		return -EIO;
	}

	/* Proceed to loop lock */
	if (i2c_smbus_write_byte_data(client, SI95317_PRG_DIR_GEN_0, SI95317_LOOPLOCK) < 0) {
		printk(KERN_INFO "Failed to write byte via SMBus\n\n");
		return -EIO;
	}

	return SUCCESS;
}

static int validate_xtalfreq_range(unsigned int xtal_ref_freq)
{
	if ((xtal_ref_freq < XTAL_FREQ_MIN) && (xtal_ref_freq > XTAL_FREQ_MAX)) {
		return XTAL_OUT_OF_RANGE_ERROR;
	}

	return SUCCESS;
}

static int validate_inputfreq_range(unsigned int input_ref_freq)
{

	if ((input_ref_freq < INPUT_SE_FREQ_MIN) && (input_ref_freq > INPUT_SE_FREQ_MAX)) {
		return INPUT_SE_OUT_OF_RANGE_ERROR;
	}

	return SUCCESS;
}

static int validate_outputfreq_range(unsigned int output_ref_freq)
{

	if ((output_ref_freq < OUTPUT_SE_FREQ_MIN) && (output_ref_freq > OUTPUT_SE_FREQ_MAX)) {
		return OUTPUT_SE_OUT_OF_RANGE_ERROR;
	}

	return SUCCESS;
}

static int validate_inputclkid_range(unsigned int in_clkid, unsigned int max_Support)
{
	if ((in_clkid < 0) && (in_clkid > max_Support-1)) {
		printk(KERN_INFO "input clkid %d out of range [%d %d] \n",in_clkid, 0, max_Support - 1);  
		return INPUTCLKID_OUT_OF_RANGE_ERROR;
	}

	return SUCCESS;
}

static int validate_outputclkid_range(unsigned int out_clkid, unsigned int max_Support)
{
	if ((out_clkid < 0) && (out_clkid > max_Support-1)) {
		printk(KERN_INFO "output clkid %d out of range [%d %d] \n", out_clkid, 0, max_Support - 1);   
		return OUTCLKID_OUT_OF_RANGE_ERROR;
	}

	return SUCCESS;
}

static int validate_normalbw_range(unsigned int normalbw_val)
{
	if ((normalbw_val < PLL_NORMAL_FAST_BW_MIN) && (normalbw_val > PLL_NORMAL_FAST_BW_MAX)) {
		printk(KERN_INFO "pll normalbw  %d out of range [%lf %d] \n", normalbw_val, PLL_NORMAL_FAST_BW_MIN, PLL_NORMAL_FAST_BW_MAX);    
		return PLLNORMALBW_OUT_OF_RANGE_ERROR;
	}

	return SUCCESS;
}

static int validate_fastbw_range(unsigned int fastbw_val)
{
	if ((fastbw_val < PLL_NORMAL_FAST_BW_MIN) && (fastbw_val > PLL_NORMAL_FAST_BW_MAX)) {
		printk(KERN_INFO "pll fastbw  %d out of range [%lf %d] \n", fastbw_val, PLL_NORMAL_FAST_BW_MIN, PLL_NORMAL_FAST_BW_MAX);  
		return PLLFASTBW_OUT_OF_RANGE_ERROR;
	}

	return SUCCESS;
}

static int validate_normalbw(unsigned int normalbw_val, unsigned int fin_pll)
{

	if (50 <= fin_pll / normalbw_val) {
		printk(KERN_INFO "The normal bw for pll should meet constrain of 50<= ( fin_pll[%d] / normal[%d]) to ensure pll stability \n",fin_pll,normalbw_val);
		return NORMAL_BW_CONSTRAINT_ERROR;
	}

	return SUCCESS;
}

static int validate_fastbw(unsigned int fastbw_val,unsigned int fin_pll)
{

	if (50 <= fin_pll / fastbw_val) {
		printk(KERN_INFO "The fast bw for pll should meet constrain of 50<= ( fin_pll[%d] / normal[%d]) to ensure pll stability \n",fin_pll,fastbw_val);  
		return FAST_BW_CONSTRAINT_ERROR;
	}

	return SUCCESS;
}

static int configure_clk(struct drv_si9531x *brd)
{
	int ret = SUCCESS;
	int i, j;

	u16 divn1_value;
	u64 Fin;

	struct divider_data divn;
	struct divider_data divn2;


    ret = validate_xtalfreq_range(brd->xtal_clk->freq);
   	printk(KERN_INFO "xtal frequency is %d",brd->xtal_clk->freq);
   	
   ret = clkin_set_doubler(brd->client, brd->xtal_clk->freq);

	for (i = 0; i < brd->num_inputs; ++i) {
		ret = validate_inputclkid_range(brd->input_clk[i].reg, brd->num_inputs);
		if(ret < 0)
			printk(KERN_INFO "Input clock ID  not in range. %d",brd->input_clk[i].reg);

		printk(KERN_INFO "clkid for %s is %d",brd->input_clk[i].clkName, brd->input_clk[i].reg);		
		ret = validate_inputfreq_range(brd->input_clk[i].freq);
		if(ret < 0) 
			printk(KERN_INFO "Input freq  not in range for %s is %d", brd->input_clk[i].clkName, brd->input_clk[i].freq);

		printk(KERN_INFO "freq for %s is %d",brd->input_clk[i].clkName, brd->input_clk[i].freq);			
	}

	//validate output clocks
	printk(KERN_INFO "validating output clocks...");
	for (j = 0; j < brd->num_outputs; ++j) {

		ret = validate_outputclkid_range(brd->output_clk[j].reg, brd->num_outputs);
		if (ret < 0) 
			printk(KERN_INFO "Output clkid failed for %s is %d", brd->output_clk[j].clkName, brd->output_clk[j].reg);

		printk(KERN_INFO "Output clkid for %s is %d",brd->output_clk[j].clkName,brd->output_clk[j].reg);

		ret = validate_outputfreq_range(brd->output_clk[j].freq);
		if (ret < 0) 
			printk(KERN_INFO "Output freq failed for %s is %d", brd->output_clk[j].clkName, brd->output_clk[j].freq);

		printk(KERN_INFO "freq for %s is %d", brd->output_clk[j].clkName, brd->output_clk[j].freq);
	}

	printk(KERN_INFO "input clock validation done\n");

	for (i = 0; i < brd->num_inputs; ++i) {
		ret = clkin_enable_disable(brd->client, i, brd->input_clk[i].status);
		if (ret)
			return ret;
	}

	for (j = 0; j < brd->num_outputs; ++j) {
		ret = clkout_enable_disable(brd->client, j, brd->output_clk[j].status);
		if (ret)
			return ret;
	}

	//set input dividers
    for(i=0; i < brd->num_inputs; ++i)
	{
       divn1_value = calculate_divn1(brd->input_clk[i].freq);
	   ret = write_divn1_to_registers(brd->client,i,divn1_value);
       if (ret < 0) {
	       printk(KERN_ERR "Failed to write DIVN1 for clkin %d\n",i);
	   }
	}
    
	//set output dividers
	for (i = 0; i < brd->num_outputs; ++i) {
		// set output frequency
		clkout_set_frequency(brd->client, i, brd->output_clk[i].freq);
	}
    
      
    for(i=0; i < brd->num_inputs; ++i)
	{
	    ret = set_fast_frequency(brd->client, brd->input_clk[i].freq);
	    if (ret < 0) {
		    printk(KERN_ERR "Failed to set fast lock bandwidth\n");
		}
	    ret = set_normal_frequency(brd->client, brd->input_clk[i].freq);
	    if(ret < 0) {
			printk(KERN_ERR "Failed to set normal lock bandwidth\n");
		}	   
        ret = set_accuracy(brd->client, brd->input_clk[i].accuracy);
        if (ret < 0) {
	       printk(KERN_ERR "Failed to write accuracy for clkin %d\n",i);
	    }
	}
	return ret;
}

static int si9531x_i2c_probe(struct i2c_client *client,const struct i2c_device_id *id)
{
	int ret = SUCCESS;
	int err = 0;
	struct drv_si9531x *brd = NULL;
	struct clk_init_data init;
	struct clk *clk = NULL;
	int i, j;

	printk(KERN_INFO "Probed device :%ld Device name chip_id detected:0x%x\n", id->driver_data, client->addr);

	memset(&init, 0, sizeof(init));

	brd = devm_kzalloc(&client->dev, sizeof(*brd), GFP_KERNEL);

	brd->client = client;
	brd->chip_id = client->addr;

	switch(brd->chip_id)
	{
		case SIT95316:
			brd->chip_name  = device_support[0].chip_name;
			brd->num_inputs = device_support[0].num_inputs;
			brd->num_outputs= device_support[0].num_outputs;
			break;
		case SIT95317:
			brd->chip_name  = device_support[1].chip_name;
			brd->num_inputs = device_support[1].num_inputs;
			brd->num_outputs= device_support[1].num_outputs;
			break;
		default:	printk(KERN_ERR "UnSuported Chip Identified \n");
				return FAILED;
	}


	ret = si9531x_dt_parse(brd);
	if (ret)
		return ret;		

	print_clk_driver(brd);
	i2c_set_clientdata(client, brd);
	g_brd = brd;

	/* Wrinting Default Register configuration */
	ret = siT9531x_write_multiple(brd, siT95317_def, ARRAY_SIZE(siT95317_def));
	if (ret < 0)
		printk(KERN_INFO "Default Registration Write reg failed\n");

	/* Register Input Clocks */
	for (i = 0; i < brd->num_inputs; i++) {
		init.name = si9531x_input_names[i];
		init.ops = &si9531x_input_clk_ops;
		brd->input_clk[i].hw.init = &init;
		brd->input_clk[i].data = brd;
		clk = devm_clk_register(&client->dev, &brd->input_clk[i].hw);
		if (IS_ERR(clk)) {
			printk(KERN_ERR "failed clk register ops for Input clk : %d\n", i);
			return ret;
		}

		err = devm_of_clk_add_hw_provider(&client->dev, of_clk_hw_simple_get,
				&brd->input_clk[i].hw);
		if (err) {
			printk(KERN_ERR "unable to add clk provider\n");
			return err;
		}
	}

	/* Register Output Clocks */
	for (j = 0; j < brd->num_outputs; j++) {
		// Reset init structure for each clock
		memset(&init, 0, sizeof(init));

		// Set the name and ops for the output clock
		init.name = si9531x_output_names[j];
		init.ops = &si9531x_output_clk_ops;
		init.flags = CLK_SET_RATE_PARENT | CLK_GET_RATE_NOCACHE;  

		// Assign init data to the hardware structure
		brd->output_clk[j].hw.init = &init;
		brd->output_clk[j].data = brd;

		// Register the clock
		brd->clk = devm_clk_register(&client->dev, &brd->output_clk[j].hw);
		if (IS_ERR(clk)) {
			printk(KERN_ERR "Failed clk register ops for Output clk: %d\n", j);
			return PTR_ERR(clk); // Proper error return
		}

		// Add the clock to the clock provider
		err = devm_of_clk_add_hw_provider(&client->dev, of_clk_hw_simple_get, &brd->output_clk[j].hw);
		if (err) {
			printk(KERN_ERR "Unable to add clk provider for Output clk: %d\n", j);
			return err;
		}
	}

	err = configure_clk(brd);
	if (err) {
		printk(KERN_ERR "Failed to configure clk for Output clk: \n" );
		return err;
	}

	ret = CreateCharDevice(brd);
	if (ret) {
		printk(KERN_ERR "Failed to create char device\n");
		return ret;
	}

	printk(KERN_INFO "Character device created successfully\n");
	return 0;
}

static int si9531x_i2c_remove(struct i2c_client *client)
{
	printk(KERN_INFO "%s:%d\n", __func__, __LINE__);
	return 0;
}

static const struct i2c_device_id si9531x_i2c_id[] = {
	{"siT95316", 0},					//Need to check here for i2c address.
	{"siT95317", 1},					//Need to check here for i2c address.
	{ }
};

MODULE_DEVICE_TABLE(i2c, si9531x_i2c_id);

static const struct of_device_id si9531x_dt_id[] = {
	{ .compatible = "SiTime,siT95316" },
	{ .compatible = "siTime,siT95317" },
	{ }
};

MODULE_DEVICE_TABLE(of, si9531x_dt_id);

static struct i2c_driver si9531x_driver = {
	.driver = {
		.name = "siT9531x",
		.of_match_table = of_match_ptr(si9531x_dt_id),
	},
	.probe = si9531x_i2c_probe,
	.remove = si9531x_i2c_remove,
	.id_table = si9531x_i2c_id,
};

module_i2c_driver(si9531x_driver);
MODULE_AUTHOR("SiTime Corporation");
MODULE_DESCRIPTION("SiTime SiT9531x CCF driver");
MODULE_LICENSE("GPL");
