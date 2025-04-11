// SPDX-License-Identifier: GPL-2.0
/*
 * Driver for SiTime Labs SiT95317/SiT95316
 * Copyright (C) 2024
 * Author: Ali Rouhi
 * 	   Serhiy Vlasenko
 *
 * The SiT95317 has 4 input & 8 output clocks.
 * The SiT95316 has 4 inputs & 16 output clocks.
 */

#include "siT95317.h"
#include "sitime_functionheader.h"
// Global structure ... Specifically to be used for IOCTLs
// Will be removed once IOCTL support is removed
struct drv_si9531x * g_brd = NULL;

//Global VCO Frequency. To be removed later
u64 g_Fvco = FVCO_LOWBAND_FREQ_MIN;

//extern sysfs_output_clk function for clk sysfs control
extern int configure_sysfs_outputclk(unsigned int chip_id);
extern void remove_sysfs_outputclk(void);

int get_outclk_pll(unsigned int clkid)
{
	int pllid = PLLA;

	if (g_brd->output_clk[clkid].pllA == 1) {
		pllid = PLLA;
	} else if ( g_brd->output_clk[clkid].pllB == 1) {
		pllid = PLLB;
	} else if (g_brd->output_clk[clkid].pllC == 1) {
		pllid = PLLC;
	} else if (g_brd->output_clk[clkid].pllD == 1) {
		pllid = PLLD;
	}

	return pllid;
}

void clkoutput_frequency(const char *buff, unsigned int clkid)
{
	int ret,pllid;
	unsigned int  freq;

	ret = kstrtouint(buff, 10, &freq);

	pllid = get_outclk_pll(clkid);

	printk(KERN_INFO " %s : clkid: %d pll: %c frequency: %d\n", __func__, clkid, 65 + pllid, freq);

	freq = clkout_set_frequency(g_brd->client, clkid, pllid, freq);

	g_brd->output_clk[clkid].freq = freq;
}

void clkoutput_status(const char *buff, unsigned int clkid)
{
	unsigned int status;
	int ret;

	ret = kstrtouint(buff, 10, &status);

	printk(KERN_INFO " %s : clkid: %d status: %d\n", __func__, clkid, status);

	ret = clkout_enable_disable(g_brd->client, clkid, status);

	g_brd->output_clk[clkid].status = status;
}

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
	int pllid;		

	clkdata = to_clk_si9531x_data(hw);

	printk(KERN_INFO "%s : %d clkName :%s clkNum:%d status:%d parent_rate :%ld \n", __func__, __LINE__, \
			clkdata->clkName, \
			clkdata->reg, \
			clkdata->status,\
			parent_rate);

	if (clkdata->pllA == 1) {
		pllid = PLLA;
	}
	else if (clkdata->pllB == 1) {
		pllid = PLLB;
	}
	else if (clkdata->pllC == 1) {
		pllid = PLLC;
	}
	else if (clkdata->pllD == 1) {
		pllid = PLLD;
	}

	freq = clkout_set_frequency(clkdata->data->client, clkdata->reg, pllid, clkdata->freq); 
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

	struct input_clock_config in_cfg;
	struct output_clock_config out_cfg;

	struct outclk_linkcfg outclk_cfg;
	struct inclk_linkcfg inclk_cfg;
	int pllid = PLLA;

	printk(KERN_INFO"Ioctl Function CMD received :%d\n", cmd);
	switch(cmd)
	{
		case REG_READ:
			if (copy_from_user(&data, (struct ioctl_data __user *)arg, sizeof(data))) {
				return -EFAULT;
			}

			printk(KERN_INFO "IOCTL Read - Page: 0x%x, Register: 0x%x\n", data.page, data.reg_address);

			data.value = i2c_smbus_read_byte_data(client, data.reg_address);
			if (data.value < 0) {
				printk(KERN_ERR "Failed to read byte via SMBus\n");
				return -EIO;
			}

			if (copy_to_user((struct ioctl_data __user *)arg, &data, sizeof(data))) {
				return -EFAULT;
			}

			printk(KERN_INFO "Read value: 0x%x from register: 0x%x on page: 0x%x\n", data.value, data.reg_address, data.page);
			break;

		case REG_WRITE:
			if (copy_from_user(&data, (struct ioctl_data __user *)arg, sizeof(data))) {
				return -EFAULT;
			}

			printk(KERN_INFO "IOCTL Write - Page: 0x%x, Register: 0x%x, Value: 0x%x\n", data.page, data.reg_address, data.value);

			ret = i2c_smbus_write_byte_data(client, data.reg_address, data.value);
			if (ret < 0) {
				printk(KERN_ERR "Failed to write byte via SMBus\n");
				return -EIO;
			}

			printk(KERN_INFO "Wrote value: 0x%x to register: 0x%x on page: 0x%x\n", data.value, data.reg_address, data.page);
			break;

		case SEL_FREQ_PLAN:
			if (copy_from_user(&freq_plan_id, (struct ioctl_data __user *)arg, sizeof(freq_plan_id))) {
				return -EFAULT;
			}
			printk(KERN_INFO "SEL_FREQ_PLAN - freq_plan_id: 0x%x \n", freq_plan_id);

			ret = set_frequency_plan(freq_plan_id);
			break;

		case CLK_ENABLE:

			if (copy_from_user(&clkid, (struct si9531x_clk_enable __user *)arg, sizeof(clkid))) {
				return -EFAULT;
			}

			ret = clkout_enable_disable(client, clkid, ON);
			break;

		case CLK_DISABLE:

			if (copy_from_user(&clkid, (struct si9531x_clk_enable __user *)arg, sizeof(clkid))) {
				return -EFAULT;
			}

			ret = clkout_enable_disable(client, clkid, OFF);
			break;

		case SET_FREQUENCY:

			if (copy_from_user(&config, (struct freq_config __user *)arg, sizeof(config))) {
				return -EFAULT;
			}

			if(g_brd->output_clk[config.clkid].pllA == 1) {
				pllid = PLLA;
			}
			else if (g_brd->output_clk[config.clkid].pllB == 1) {
				pllid = PLLB;
			}
			else if (g_brd->output_clk[config.clkid].pllC == 1) {
				pllid = PLLC;
			}
			else if (g_brd->output_clk[config.clkid].pllD == 1) {
				pllid = PLLD;
			}

			ret = clkout_set_frequency(client, config.clkid, pllid, config.output_frequency);
			printk(KERN_ERR "set rate return for Output clk: %d\n", ret);

			g_brd->output_clk[config.clkid].freq = ret;
			ret = clk_set_rate(g_brd->output_clk[config.clkid].hw.clk, ret);
			printk(KERN_ERR "set rate return :%d \n", ret);

			break;

		case SET_INPUT_FREQ:
			if (copy_from_user(&config, (struct freq_config __user *)arg, sizeof(config))) {
				return -EFAULT;
			}

			ret = clkin_set_frequency(client, config.clkid, config.input_frequency, config.output_frequency);
			if (ret < 0) {
				printk(KERN_ERR "Failed to set input frequency\n");
				return ret;
			}

			printk(KERN_INFO "Input frequency set to: %u Hz, Output frequency: %u Hz\n",
					config.input_frequency, config.output_frequency);
			break;

		case CONFIGURE_OUTPUT_CLOCK_LINK_TYPE: // New case
			if (copy_from_user(&out_cfg, (struct output_clock_config __user *)arg, sizeof(out_cfg))) {
				return -EFAULT;
			}

			outclk_cfg.linktype     = out_cfg.linktype;
			outclk_cfg.difflinktype = out_cfg.difflinktype;
			outclk_cfg.mode         = out_cfg.mode;
			outclk_cfg.swing        = out_cfg.swing;
			outclk_cfg.itresistor   = out_cfg.itresistor;

			ret = clkout_set_linktype(client, out_cfg.clkid, &outclk_cfg); //call the function from where we read input registers
			if (ret < 0) {
				printk(KERN_ERR "Failed to configure output clock ID: %d\n", config.clkid);
				return ret;
			}

			break;

		case CONFIGURE_INPUT_CLOCK_LINK_TYPE: // New case
			if (copy_from_user(&in_cfg, (struct input_clock_config __user *)arg, sizeof(in_cfg))) {
				return -EFAULT;
			}

			//printk(KERN_INFO  in_cfg.clkid,in_cfg.inclk_linkcfg clockin_linkinfo );

			inclk_cfg.linktype     = in_cfg.linktype;
			inclk_cfg.difflinktype = in_cfg.difflinktype;
			inclk_cfg.mode         = in_cfg.mode;

			ret = clkin_set_linktype(client, in_cfg.clkid, &inclk_cfg); //call the function from where we read input registers
			if (ret < 0) {
				printk(KERN_ERR "Failed to configure output clock ID: %d\n", in_cfg.clkid);
				return ret;
			}

			printk(KERN_INFO "Input clock ID: %d successfully configured.\n",  in_cfg.clkid);

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
	const char *readmode = NULL;
	const char *diffmode = NULL;

	if (of_property_read_u32(np, "reg", &reg) < 0) {
		return -EINVAL;
	}

	printk(KERN_INFO "Reg value read from DTS: 0x%x \n", reg);

	//Allocate structure element pointer memory
	brd->input_clk 	= kzalloc( sizeof(struct si9531x_clk)*brd->num_inputs, GFP_KERNEL);
	brd->output_clk = kzalloc(sizeof(struct si9531x_clk)*brd->num_outputs, GFP_KERNEL);
	brd->xtal_clk 	= kzalloc(sizeof(struct si9531x_clk), GFP_KERNEL);
	brd->clkin 	= kzalloc( sizeof(struct  clk *)*brd->num_inputs, GFP_KERNEL);
	brd->clkout 	= kzalloc(sizeof(struct clk *)*brd->num_outputs, GFP_KERNEL);

	num = of_property_match_string(np, "clock-names", "xtal");
	if (num < 0) {
		printk(KERN_INFO "xtal not found \n");
	} else {
		printk(KERN_INFO "xtal found at num:%d\n",num);
	}

	mainxtal_name = of_clk_get_parent_name(np, num);
	printk(KERN_INFO "xtal name:%s\n", mainxtal_name);

	xtal = of_find_node_by_name(NULL, mainxtal_name);
	if (xtal == NULL) {
		printk(KERN_INFO "Unable to find dt node %s\n", mainxtal_name);
		return -EINVAL;
	}

	brd->xtal_clk->clkName = (char *)xtal->name;
	of_property_read_u32(xtal, "clock-frequency", &brd->xtal_clk->freq);
	printk(KERN_INFO "DTS : child_name:%s   freq read:%d\n",  brd->xtal_clk->clkName, brd->xtal_clk->freq);

	if (of_property_match_string(np, "eeprom-override", "yes") == 0) {
		brd->eeprom_override = ON;
	} else {
		brd->eeprom_override = OFF; 
	}

	printk(KERN_INFO "EEPROM-OVERRIDE FLAG : %d\n", brd->eeprom_override);

	for_each_child_of_node(np, child) {
		printk(KERN_INFO "Node :%s  ChildName: %s \n", np->name, child->name);

		if (of_property_match_string(child, "clktype", "in") == 0) {
			if (of_property_read_u32(child, "reg", &index)) {
				dev_err(&brd->client->dev, "missing reg property of %s\n", child->name);
				break;
			}
			brd->input_clk[index].reg = index;
			brd->input_clk[index].clkName = (char *)child->name;	

			if (of_property_read_u32(child, "clock-frequency", &brd->input_clk[index].freq) < 0) {
				printk(KERN_INFO "DTS : %s: Clock freq not set \n", child->name);
			}

			if (of_property_match_string(child, "status", "okay") == 0) {
				brd->input_clk[index].status = ON;
			} else {
				brd->input_clk[index].status = OFF;
			}

			if (of_property_match_string(child, "linktype", "single") == 0) {
				brd->input_clk[index].linktype = SINGLE;
			} else {
				brd->input_clk[index].linktype = DIFFERENTIAL;
			}

			if (of_property_match_string(child, "mode", "DC") == 0) {
				brd->input_clk[index].mode = DC;
			} else {
				brd->input_clk[index].mode = AC;		//set AC as default
			}

			if (of_property_match_string(child, "linkmode", "N") == 0) {
				brd->input_clk[index].difflinktype = N_TYPE;
			} else {
				brd->input_clk[index].difflinktype = P_TYPE;	//set p as default 
			}

			if (of_property_match_string(child, "PLL", "PLLA") < 0) {
				brd->input_clk[index].pllA = OFF;
			} else {
				brd->input_clk[index].pllA = ON;
			}

			if (of_property_match_string(child, "PLL", "PLLB") < 0) {
				brd->input_clk[index].pllB = OFF;
			} else {
				brd->input_clk[index].pllB = ON;
			}

			if (of_property_match_string(child, "PLL", "PLLC") < 0) {
				brd->input_clk[index].pllC = OFF;
			} else {
				brd->input_clk[index].pllC = ON;
			}

			if (of_property_match_string(child, "PLL", "PLLD") < 0) {
				brd->input_clk[index].pllD = OFF;
			} else {
				brd->input_clk[index].pllD = ON;
			}

			if (of_property_read_u32(child, "clock-accuracy", &brd->input_clk[index].accuracy) < 0) {
				printk(KERN_INFO "DTS : %s: Clock Accuracy not set \n",child->name);
			}
		}

		if (of_property_match_string(child, "clktype", "out") == 0) {
			if (of_property_read_u32(child, "reg", &index)) {
				dev_err(&brd->client->dev, "missing reg property of %s\n", child->name);
				break;
			}

			brd->output_clk[index].reg = index;
			brd->output_clk[index].clkName = (char *)child->name;

			if (of_property_read_u32(child, "clock-frequency", &brd->output_clk[index].freq) < 0) {
				printk(KERN_INFO "DTS : %s: Clock freq not set \n", child->name);
			}

			if (of_property_match_string(child,"status", "okay") == 0) {
				brd->output_clk[index].status = ON;
			} else {
				brd->output_clk[index].status = OFF;
			}

			if (of_property_match_string(child, "linktype", "single") == 0) {
				brd->output_clk[index].linktype = SINGLE;
			} else {
				brd->output_clk[index].linktype = DIFFERENTIAL;
			}

			if (of_property_read_string(child, "difflinktype", &diffmode) == 0) {
				if(strcmp(diffmode, "EXT_TERM") == 0) {
					brd->output_clk[index].difflinktype = EXT_TERM;
				} else if(strcmp(diffmode, "INT_PULL_DN") == 0) {
					brd->output_clk[index].difflinktype = INT_PULL_DN;
				} else if(strcmp(diffmode, "INT_PULL_UP") == 0) {
					brd->output_clk[index].difflinktype = INT_PULL_UP;
				} else if(strcmp(diffmode, "CMOS_OUTP_NO_OUTN") == 0) {
					brd->output_clk[index].difflinktype = CMOS_OUTP_NO_OUTN;
				} else if(strcmp(diffmode, "NO_OUTP_CMOS_OUTN") == 0) {
					brd->output_clk[index].difflinktype = NO_OUTP_CMOS_OUTN;
				} else if(strcmp(diffmode, "CMOS_OUTP_CMOS_OUTN") == 0) {
					brd->output_clk[index].difflinktype = CMOS_OUTP_CMOS_OUTN;
				}
			}

			if (of_property_read_u32(child, "swingvoltage", &brd->output_clk[index].swingvoltage) < 0) {
				printk(KERN_INFO "DTS : %s: Clock differencial swingvoltage not set \n", child->name);
			}

			if (of_property_match_string(child, "LVDSterm", "enable") == 0) {
				brd->output_clk[index].terminationreg = ON;
			} else {
				brd->output_clk[index].terminationreg = OFF;
			}

			if (of_property_read_string(child, "mode", &readmode) == 0) {
				if(strcmp(readmode, "LVDS")==0) {
					brd->output_clk[index].mode=LVDS;
				} else if(strcmp(readmode, "LVPECL")==0) {
					brd->output_clk[index].mode=LVPECL;
				} else if(strcmp(readmode, "CML")==0) {
					brd->output_clk[index].mode=CML;
				} else if(strcmp(readmode, "HCSL")==0) {
					brd->output_clk[index].mode=HCSL;
				}
			}

			if (of_property_read_string(child, "PLL", &readmode) == 0) {
				if (strcmp(readmode, "PLLA") == 0) {
					brd->output_clk[index].pllA = ON;
					brd->output_clk[index].pllB = \
								      brd->output_clk[index].pllC = \
								      brd->output_clk[index].pllD = OFF ;
				} else if (strcmp(readmode, "PLLB") == 0) {
					brd->output_clk[index].pllB = ON;
					brd->output_clk[index].pllA = \
								      brd->output_clk[index].pllC = \
								      brd->output_clk[index].pllD = OFF ;
				} else if (strcmp(readmode, "PLLC") == 0) {
					brd->output_clk[index].pllC = ON;
					brd->output_clk[index].pllA = \
								      brd->output_clk[index].pllB = \
								      brd->output_clk[index].pllD = OFF ;
				} else if (strcmp(readmode, "PLLD") == 0) {
					brd->output_clk[index].pllD = ON;
					brd->output_clk[index].pllA = \
								      brd->output_clk[index].pllB = \
								      brd->output_clk[index].pllC = OFF ;
				}
			}

			if (of_property_read_u32(child, "clock-accuracy", &brd->output_clk[index].accuracy) < 0) {
				printk(KERN_INFO "DTS : %s: Clock Accuracy not set \n", child->name);
			}

		}
	}
	return SUCCESS;
}

static int soft_rst_por_chip(void)
{
	int ret  = 0;
	struct drv_si9531x *brd = g_brd;

	ret = siT9531x_write_multiple(brd, chip_rst_cfg, ARRAY_SIZE(chip_rst_cfg));
	if (ret < 0) {
		printk(KERN_INFO "chip reset reg_write failed\n");
	}

	return ret;
}

static int set_frequency_plan(unsigned int plan_id)
{   
	int ret  = 0;
	struct drv_si9531x *brd = g_brd;

	//apply soft reset on chip
	soft_rst_por_chip();

	switch( plan_id )
	{
		case 1 : 	
			ret = siT9531x_write_multiple(brd, siT95317_def1, ARRAY_SIZE(siT95317_def1));
			if (ret < 0) {
				printk(KERN_INFO "Dfreq1_regcfg Write reg failed\n");
			}
			break;

		case 2 : 	
			ret = siT9531x_write_multiple(brd, siT95317_lftdef4, ARRAY_SIZE(siT95317_lftdef4));
			if (ret < 0) {
				printk(KERN_INFO "Dfreq2_regcfg Write reg failed\n");
			}
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
		if (res < 0) {
			printk(KERN_INFO "Failed to write for client write %#x:%#x\n", values[i].address, values[i].value);
		}
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

	if (*value & (1 << clkid)) {
		ret = ON;
	} else {
		ret = OFF;
	}

	printk(KERN_INFO "%s :value:%d clkid:%d ret:%d \n", __func__, *value, clkid, ret);
	return ret;
}

static int clkin_enable_disable(struct i2c_client *client, int clkid, bool is_enabled)
{
	int ret = SUCCESS;
	unsigned int value = 0;
	const unsigned int clkin_regvalue[4] = {0x01, 0x02, 0x04, 0x08};

	ret = clkin_is_enable_disable(client, clkid, is_enabled, &value);

	if (is_enabled) {
		value = value | clkin_regvalue[clkid];		// set bit of reg by clk_id
	} else {
		value = value & ( ~clkin_regvalue[clkid] ); 		// set bit of reg by clk_id
	}

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

	if (clkid < 5) {
		reg_address = OUTPUT_REG1;
	} else {
		reg_address = OUTPUT_REG2;
	}

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

	if (!(( *value & clkout_regvalue[clkid]) ^ clkout_regvalue[clkid])) {
		ret = ON;
	} else {
		ret = OFF;	
	}

	printk(KERN_INFO "%s :value:%d\n", __func__, *value);
	return ret;
}

static int clkout_enable_disable(struct i2c_client *client, int clkid, bool is_enabled)
{
	int ret = SUCCESS;
	unsigned int value = 0;
	unsigned int reg_address = 0;

	printk(KERN_INFO "is_enabled :%d clkid:%d clkout_regvalue: %d \n", is_enabled, clkid, clkout_regvalue[clkid]);

	if (clkid < 5) {
		reg_address = OUTPUT_REG1;
	} else {
		reg_address = OUTPUT_REG2;
	}

	ret = clkout_is_enable_disable(client, clkid, is_enabled, &value);

	if (is_enabled) {
		value = value | clkout_regvalue[clkid];		// set bit of reg by clk_id
	} else { 
		value = value & (~clkout_regvalue[clkid]); 		// set bit of reg by clk_id
	}

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

static int clkout_set_frequency(struct i2c_client *client, int clkid,int pllid, unsigned int frequency)
{
	unsigned int FIXED_POINT_PRECISION_VALUE = 1000;
	unsigned int page_num = 0;
	unsigned int reg_address = 0;
	unsigned int div4reg_value = 0;
	int j;
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

	if (( pllid == PLLB ) || ( pllid == PLLD )) {
		vco_freq_band = FVCO_HIGHBAND_FREQ_MIN;
	} else if (( pllid == PLLA ) || ( pllid == PLLC )) {
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

static int write_divn1_to_registers(struct i2c_client *client, unsigned int clkid, u16 divn1_value)
{
	int i = 0;
	int ret = 0;
	unsigned int base_reg_address[4] = {0x10, 0x20, 0x30, 0x40};
	unsigned int base_reg_address_num[4] = {0x12, 0x22, 0x32, 0x42};
	unsigned int base_reg_address_den[4] = {0x16, 0x26, 0x36, 0x46};

	u8 divn1_bytes[2];

	divn1_bytes[0] = divn1_value & 0xff;
	divn1_bytes[1] = (divn1_value >> 8) & 0xff;

	printk(KERN_INFO "DIVN1 bytes: [%02x %02x]\n", divn1_bytes[0], divn1_bytes[1]);

	// Escape to PRGCmd State
	ret = switch_to_PRGCmd_state(client, Page2_Inputsys);
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

	unsigned int reg_address = DIVN_REG_ADDRESS;
	unsigned int reg_address_num = DIVN_REG_ADDRESS_NUM;
	unsigned int reg_address_den = DIVN_REG_ADDRESS_DEN;

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
	int i = 0;
	int ret = 0;
	u8 divn2_int_bytes[5] = {0};  
	u8 divn2_fracd_bytes[4] = {0};

	u8 divn2_fracn_bytes[4] = {0};
	u64 DIVN2_INT, DIVN2_FRACN, DIVN2_FRACD;

	unsigned int divn2int_reg4_value;
	unsigned int reg_address = DIVN2_REG_ADDRESS;
	unsigned int reg_address_num = DIVN2_REG_ADDRESS_NUM;
	unsigned int reg_address_den = DIVN2_REG_ADDRESS_DEN;

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


static int set_fast_frequency(struct i2c_client *client, unsigned int input_frequency)
{
	int ret;
	int i;
	const struct freq_range *range = NULL;

	// Find the correct entry in the frequency table
	for (i = 0; i < ARRAY_SIZE(freq_table); i++) {
		if ((input_frequency >= freq_table[i].min_freq) && (input_frequency <= freq_table[i].max_freq)) {
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

static int set_normal_frequency(struct i2c_client *client, unsigned int input_frequency)
{
	int ret = 0;
	int i = 0;
	const struct freq_range *range = NULL;

	// Find the correct entry in the frequency table
	for (i = 0; i < ARRAY_SIZE(freq_table); i++) {
		if ((input_frequency >= freq_table[i].min_freq) && (input_frequency <= freq_table[i].max_freq)) {
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

static int set_accuracy(struct i2c_client *client, unsigned int accuracy)
{
	int ret = 0;
	u8 threshold_value = 0;

	threshold_value = accuracy / 100;

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
	int ret = 0, pllid = 0;
	u16 divn1_value = 0;
	u64 Fin = 0;

	struct divider_data divn;
	struct divider_data divn2;

	printk(KERN_INFO "clkin_set_frequency called for clkid: %d\n", clkid);
	printk(KERN_INFO "Input frequency: %u Hz, Output frequency: %u Hz\n", input_frequency, output_frequency);

	if (clkid == 0 || clkid == 1) {
		pllid = PLLD;
	} else if (clkid == 2 || clkid == 3) {
		pllid = PLLC; 
	} else if (clkid == 4 || clkid == 5) {
		pllid = PLLB; 
	} else if (clkid == 6 || clkid == 7) {
		pllid = PLLA; 
	}

	divn1_value = calculate_divn1(input_frequency);
	write_divn1_to_registers(client, pllid, divn1_value);
	clkout_set_frequency(client, clkid, pllid, output_frequency);

	divn = calculate_divn(g_brd->xtal_clk->freq, g_Fvco);
	write_divn_to_registers(client, pllid, &divn);

	Fin = input_frequency / divn1_value;
	divn2 = calculate_divn2(Fin, g_Fvco);
	write_divn2_to_registers(client, pllid, &divn2);

	ret = set_fast_frequency(client, input_frequency);
	if (ret < 0) {
		printk(KERN_ERR "Failed to set fast lock bandwidth\n");
		return ret;
	}

	ret = set_normal_frequency(client, input_frequency);
	if (ret < 0) {
		return ret;
	}

	ret = set_accuracy(client, g_brd->input_clk[3].accuracy);

	return SUCCESS;
}

static int clkin_set_doubler(struct i2c_client *client, unsigned int xtal_ref_freq)
{
	int ret = 0;
	// double enabled : 0 ; doubler disabled : 1
	int doubler = 0, mask = 0;
	unsigned int value = 0;
	unsigned int xo2_generic_rdvalue = 0;

	if ((xtal_ref_freq >= DOUBLER_XTAL_FREQ_MIN) && (xtal_ref_freq <= DOUBLER_XTAL_FREQ_MAX)) {
		printk(KERN_INFO "doubler enabled for %d xtal frequency as recommended\n", xtal_ref_freq);
		doubler = 0; 
		mask = 0x7f;
	} else {
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
		printk(KERN_INFO "pll fastbw  %d out of range [%lf %d] \n", \
				fastbw_val, \
				PLL_NORMAL_FAST_BW_MIN, \
				PLL_NORMAL_FAST_BW_MAX); 

		return PLLFASTBW_OUT_OF_RANGE_ERROR;
	}
	return SUCCESS;
}

static int validate_normalbw(unsigned int normalbw_val, unsigned int fin_pll)
{
	if (50 <= fin_pll / normalbw_val) {
		printk(KERN_INFO "The normal bw for pll should meet constrain of \
				50<= ( fin_pll[%d] / normal[%d]) to ensure pll stability \n", \
				fin_pll, \
				normalbw_val);

		return NORMAL_BW_CONSTRAINT_ERROR;
	}
	return SUCCESS;
}

static int validate_fastbw(unsigned int fastbw_val,unsigned int fin_pll)
{
	if (50 <= fin_pll / fastbw_val) {
		printk(KERN_INFO "The fast bw for pll should meet constrain of \
				50<= ( fin_pll[%d] / normal[%d]) to ensure pll stability \n", \
				fin_pll, \
				fastbw_val);  

		return FAST_BW_CONSTRAINT_ERROR;
	}
	return SUCCESS;
}

static int configure_clk(struct drv_si9531x *brd)
{
	int ret = SUCCESS;
	int i, j;
	int pllid;		
	u16 divn1_value;
	struct inclk_linkcfg incfg;
	struct outclk_linkcfg outcfg;

	static const unsigned int siT95317Output2siT95316RegMap[] = {0, 3, 4, 5, 7, 8, 9, 11};
	static const int clocktobitmapping[] ={3,2,1,0};
	static const int pllcd_reg28_clkbitmap[] ={7,6,5,4,3,2,1,0};
	u8 reg27_value[4]={0};
	u8 reg28_value[4]={0}; 

	unsigned int sit95316clkid = 0; 

	// Validate xtal frequency
	ret = validate_xtalfreq_range(brd->xtal_clk->freq);
	printk(KERN_INFO "xtal frequency is %d", brd->xtal_clk->freq);

	// set doubler value
	ret = clkin_set_doubler(brd->client, brd->xtal_clk->freq);

	// validate input clocks
	printk(KERN_INFO "validating input clocks...\n");
	for (i = 0; i < brd->num_inputs; ++i) {
		ret = validate_inputclkid_range(brd->input_clk[i].reg, brd->num_inputs);
		if (ret < 0) {
			printk(KERN_INFO "Input clock ID  not in range. %d", brd->input_clk[i].reg);
		}
		printk(KERN_INFO "clkid for %s is %d",brd->input_clk[i].clkName, brd->input_clk[i].reg);		

		ret = validate_inputfreq_range(brd->input_clk[i].freq);
		if (ret < 0) {
			printk(KERN_INFO "Input freq  not in range for %s is %d", \
					brd->input_clk[i].clkName, \
					brd->input_clk[i].freq);
		}
		printk(KERN_INFO "freq for %s is %d", brd->input_clk[i].clkName, brd->input_clk[i].freq);			
	}
	printk(KERN_INFO "input clock validation done\n");

	//validate output clocks
	printk(KERN_INFO "validating output clocks...\n");
	for (j = 0; j < brd->num_outputs; ++j) {
		ret = validate_outputclkid_range(brd->output_clk[j].reg, brd->num_outputs);
		if (ret < 0) {
			printk(KERN_INFO "Output clkid failed for %s is %d", \
					brd->output_clk[j].clkName, \
					brd->output_clk[j].reg);
		}

		printk(KERN_INFO "Output clkid for %s is %d", \
				brd->output_clk[j].clkName, \
				brd->output_clk[j].reg);

		ret = validate_outputfreq_range(brd->output_clk[j].freq);
		if (ret < 0) {
			printk(KERN_INFO "Output freq failed for %s is %d", \
					brd->output_clk[j].clkName, \
					brd->output_clk[j].freq);
		}

		printk(KERN_INFO "freq for %s is %d", brd->output_clk[j].clkName, brd->output_clk[j].freq);
	}
	printk(KERN_INFO "output clock validation done\n");

	// Input clock link settings & en/dis
	for (i = 0; i < brd->num_inputs; ++i) {
		if (brd->input_clk[i].status) {     
			incfg.linktype     = brd->input_clk[i].linktype;
			incfg.difflinktype = brd->input_clk[i].difflinktype;
			incfg.mode         = brd->input_clk[i].mode;		
			ret = clkin_set_linktype(brd->client, i, &incfg);
			if (ret) {
				return ret;
			} 
		}

		ret = clkin_enable_disable(brd->client, i, brd->input_clk[i].status);
		if (ret) {
			return ret;
		}
	}  
	printk(KERN_INFO "input clock link settings done\n");

	// Output clock link settings & en/dis
	for (j = 0; j < brd->num_outputs; ++j) {
		if (brd->output_clk[j].status) {
			outcfg.linktype = brd->output_clk[j].linktype;
			outcfg.difflinktype = brd->output_clk[j].difflinktype;
			outcfg.mode     = brd->output_clk[j].mode;
			outcfg.swing    = brd->output_clk[j].swingvoltage;
			outcfg.itresistor = brd->output_clk[j].terminationreg;

			ret = clkout_set_linktype(brd->client, j, &outcfg);
			if (ret) {
				return ret;
			}
		}

		ret = clkout_enable_disable(brd->client, j, brd->output_clk[j].status);
		if (ret) {
			return ret;
		}
	}
	printk(KERN_INFO "output clock link settings done\n");

	//set input dividers
	for (i = 0; i < brd->num_inputs; ++i)
	{
		if(brd->input_clk[i].status) {
			divn1_value = calculate_divn1(brd->input_clk[i].freq);

			ret = write_divn1_to_registers(brd->client, i, divn1_value);
			if (ret < 0) {
				printk(KERN_ERR "Failed to write DIVN1 for clkin %d\n", i);
			}
		}
	}

	// set output frequency
	for (j = 0; j < brd->num_outputs; ++j) {
		if(brd->output_clk[j].status) { 
			if(brd->output_clk[j].pllA == 1) {
				pllid = PLLA;
			} else if (brd->output_clk[j].pllB == 1) {
				pllid = PLLB;
			} else if (brd->output_clk[j].pllC == 1) {
				pllid = PLLC;
			} else if (brd->output_clk[j].pllD == 1) {
				pllid = PLLD;
			}
			printk(KERN_INFO "PLL for clkout %d is PLL%c\n", j, 65 + pllid);

			// Mapping is done as we are referencing sit95316 RegisterMap
			sit95316clkid = siT95317Output2siT95316RegMap[j];

			if ((pllid == PLLA) || (pllid == PLLB)) {
				if(sit95316clkid > 7) {
					reg27_value[pllid] = reg27_value[pllid] | (1 << (sit95316clkid - 8));
				} else {
					reg28_value[pllid] = reg28_value[pllid] | (1 << sit95316clkid);
				}
			} else if ((pllid == PLLC) || (pllid == PLLD)) {  
				if (sit95316clkid < 4) {
					reg27_value[pllid] = reg27_value[pllid] | (1 << (clocktobitmapping[sit95316clkid]));
				} else {
					reg28_value[pllid] = reg28_value[pllid] | (1 << (pllcd_reg28_clkbitmap[sit95316clkid - 4]));
				} 
			}

			//set output frequency
			clkout_set_frequency(brd->client, j, pllid, brd->output_clk[j].freq);
		}
	}

	for (pllid = 0; pllid < 4; pllid++) {
		ret = write_outclk_pll_bits(brd->client, pllid, reg27_value[pllid], reg28_value[pllid]);
	}

	// set accuracy
	for (i = 0; i < brd->num_inputs; ++i) {
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


static int clkin_set_linktype(struct i2c_client *client, int clkid, const struct inclk_linkcfg* clockin_linkinfo)
{
	unsigned int linktype = clockin_linkinfo->linktype;
	unsigned int difflinktype = clockin_linkinfo->difflinktype;
	unsigned int mode	  = clockin_linkinfo->mode;

	if (linktype == SINGLE) {// Single Ended
		if (difflinktype == P_TYPE) {         // Single Ended P
			linktype = 1;
		} else if (difflinktype == N_TYPE) {  // Single Ended N
			linktype = 2;
		}
	} else if(linktype == DIFFERENTIAL) { // Differential
		linktype = 0;
	}

	/*
	   printk(KERN_INFO "LinkType(0:SE 1:DIFF) : %d",linktype);
	   printk(KERN_INFO "SE LinkType(0:P 1:N) : %d",difflinktype);
	   printk(KERN_INFO "Mode(0:AC 1:DC) : %d"    ,mode);
	   */
	set_inputlinktype(client, clkid, linktype);
	set_inputmode(client, clkid, linktype, mode);

	return SUCCESS;
}

static int set_inputlinktype(struct i2c_client *client,int clkid,unsigned int se_diff_type)
{
	static const unsigned int clkin_sediff_regvalue[] = { 0x1B, 0x2B, 0x3B, 0x4B};
	unsigned int reg_address = 0;
	unsigned int linkreg_value = 0;
	int ret = 0;

	// Escape to PRGCmd State
	ret = switch_to_PRGCmd_state(client,Page2_Inputsys);
	if (ret < 0) {
		printk(KERN_ERR "Failed to Escape to PRGCmd State");
		return ret;
	}

	// LINKTYPE : SINGLE OR DIFFERENTIAL
	reg_address = clkin_sediff_regvalue[clkid];   

	linkreg_value = i2c_smbus_read_byte_data(client, reg_address);	
	if (linkreg_value < 0) {
		printk(KERN_ERR "Failed to read byte via SMBus\n");
		return -EIO;
	}

	linkreg_value = (linkreg_value & 0xFC) | se_diff_type;

	if (i2c_smbus_write_byte_data(client, reg_address, linkreg_value) < 0) {
		printk(KERN_INFO "I2C_Write Error: input clk se_diff_type set failed\n");
		return -EIO;
	}

	//update NVM Bank & Proceed to Loop Lock
	ret = do_updatenvm_lockloop(client);
	if (ret < 0) {
		printk(KERN_ERR "Failed to update NVM Bank and Lock Loop\n");
		return ret;
	}
	return SUCCESS;
}

static int set_inputmode(struct i2c_client *client,int clkid, unsigned int linktype, unsigned int acdc_mode)
{
	static const unsigned int clkin_acdcmode_regvalue[] = { 0x1C, 0x2C, 0x3C, 0x4C};
	unsigned int reg_address = 0;
	unsigned int linkreg_value = 0;
	int ret = 0;

	// Escape to PRGCmd State
	ret = switch_to_PRGCmd_state(client,Page2_Inputsys);
	if (ret < 0) {
		printk(KERN_ERR "Failed to Escape to PRGCmd State");
		return ret;
	}

	// Mode : AC OR DC
	reg_address = clkin_acdcmode_regvalue[clkid];   

	linkreg_value = i2c_smbus_read_byte_data(client, reg_address);	
	if (linkreg_value < 0) {
		printk(KERN_ERR "Failed to read byte via SMBus\n");
		return -EIO;
	}

	if (linktype == 0) {
		linkreg_value = (linkreg_value & 0xDF);
		if (acdc_mode) {
			linkreg_value = (linkreg_value & 0xDF) | (0x01 << 5);
		}
	}

	if (linktype == 1) { // SE P
		linkreg_value = (linkreg_value & 0xFE); //DC Mode
		if (acdc_mode == 0) {// AC Mode
			linkreg_value = (linkreg_value & 0xFE) | (0x01 << 0);
		}
	}

	if (linktype == 2) {//SE N
		linkreg_value = (linkreg_value & 0xFB); //DC Mode
		if (acdc_mode == 0) {// AC Mode
			linkreg_value = (linkreg_value & 0xFB) | (0x01 << 2);
		}
	}

	if (i2c_smbus_write_byte_data(client, reg_address, linkreg_value) < 0) {
		printk(KERN_INFO "I2C_Write Error: input clk acdc_mode set failed\n");
		return -EIO;
	}

	//update NVM Bank & Proceed to Loop Lock
	ret = do_updatenvm_lockloop(client);
	if (ret < 0) {
		printk(KERN_ERR "Failed to update NVM Bank and Lock Loop\n");
		return ret;
	}

	return SUCCESS;
}


static int clkout_set_linktype(struct i2c_client *client, int clkid, const struct outclk_linkcfg* clockout_linkinfo)
{
	/*  Link Info  */
	unsigned int linktype     = clockout_linkinfo->linktype;
	unsigned int difflinktype = clockout_linkinfo->difflinktype;
	unsigned int mode         = clockout_linkinfo->mode;
	unsigned int swing        = clockout_linkinfo->swing;
	unsigned int itresistor   = clockout_linkinfo->itresistor;

	/*
	   printk(KERN_INFO "LinkType                       :   %d",linktype);
	   printk(KERN_INFO "Diff LinkType                  :   %d",difflinktype);
	   printk(KERN_INFO "Mode                           :   %d",mode);
	   printk(KERN_INFO "swing                          :   %d",swing);
	   printk(KERN_INFO "internal termination resistor  :   %d",itresistor);
	   */


	if((linktype == SINGLE) && (difflinktype > CMOS_OUTP_CMOS_OUTN)) {// single-ended linktype
		difflinktype = CMOS_OUTP_NO_OUTN; //default cmos on outP,nothing on outN
	}

	set_outputlinktype(client, clkid, difflinktype);

	if (linktype == DIFFERENTIAL) {// differential linktype
		set_differential_mode(client, clkid, mode, itresistor);
		set_swing_voltage(client, clkid, swing);
	}

	return SUCCESS;
}

static int set_outputlinktype(struct i2c_client *client, int clkid, unsigned int linktype)
{
	static const unsigned int siT95317Output2siT95316RegMap[] = {0, 3, 4, 5, 7, 8, 9, 11};
	static const unsigned int clkout_odr_conf_regvalue[] = { 0x1E, 0x2E, 0x3E, 0x4E, 0x5E, 0x6E};

	unsigned int reg_address = 0;
	unsigned int linkreg_value = 0;
	int ret = 0;

	// Escape to PRGCmd State
	ret = switch_to_PRGCmd_state(client, Page3_Outsys);
	if (ret < 0) {
		printk(KERN_ERR "Failed to Escape to PRGCmd State");
		return ret;
	}

	/* Switch to page 4 if output clock id is greater than 5 */
	if (siT95317Output2siT95316RegMap[clkid] > 5) {
		if (i2c_smbus_write_byte_data(client, SI95317_PAGE_NUM, Page4_Outsys) < 0) {
			printk(KERN_INFO "I2C_Write Error: switch to Page4_Outsys failed\n");
			return -EIO;
		}
	}

	/* Register map for different output clocks */
	reg_address = clkout_odr_conf_regvalue[siT95317Output2siT95316RegMap[clkid] % 6];

	linkreg_value = i2c_smbus_read_byte_data(client, reg_address);
	if (linkreg_value < 0) {
		printk(KERN_ERR "Failed to read byte via SMBus\n");
		return -EIO;
	}

	linkreg_value = (linkreg_value & 0xF0) | linktype;

	if (i2c_smbus_write_byte_data(client, reg_address, linkreg_value) < 0) {
		printk(KERN_INFO "I2C_Write Error: output clk linktype set failed\n");
		return -EIO;
	}


	/* Set page back to MAIN OUTSYS (Page3_Outsys) */
	if (i2c_smbus_write_byte_data(client, SI95317_PAGE_NUM, Page3_Outsys) < 0) {
		printk(KERN_ERR "Failed to write byte via SMBus\n");
		return -EIO;
	}

	//update NVM Bank & Proceed to Loop Lock
	ret = do_updatenvm_lockloop(client);
	if (ret < 0) {
		printk(KERN_ERR "Failed to update NVM Bank and Lock Loop\n");
		return ret;
	}

	return SUCCESS;
}

static int set_differential_mode(struct i2c_client *client, int clkid,unsigned int mode,unsigned int itresistor)
{
	static const unsigned int siT95317Output2siT95316RegMap[] = {0, 3, 4, 5, 7, 8, 9, 11};
	static const unsigned int clkout_odr_drvmode_regvalue[] = { 0x1C, 0x2C, 0x3C, 0x4C, 0x5C, 0x6C};
	static const unsigned int clkout_odr_inttermres_regvalue[] = { 0x10, 0x20, 0x30, 0x40, 0x50, 0x60}; 

	unsigned int reg_address = 0;
	unsigned int linkreg_value = 0;
	int ret = 0;

	// Escape to PRGCmd State
	ret = switch_to_PRGCmd_state(client, Page3_Outsys);
	if (ret < 0) {
		printk(KERN_ERR "Failed to Escape to PRGCmd State");
		return ret;
	}

	/* Switch to page 4 if output clock id is greater than 5 */
	if (siT95317Output2siT95316RegMap[clkid] > 5) {
		if (i2c_smbus_write_byte_data(client, SI95317_PAGE_NUM, Page4_Outsys) < 0) {
			printk(KERN_INFO "I2C_Write Error: switch to Page4_Outsys failed\n");
			return -EIO;
		}
	}

	reg_address = clkout_odr_drvmode_regvalue[siT95317Output2siT95316RegMap[clkid] % 6];

	linkreg_value = i2c_smbus_read_byte_data(client, reg_address);
	if (linkreg_value < 0) {
		printk(KERN_ERR "Failed to read byte via SMBus\n");
		return -EIO;
	}

	linkreg_value = (linkreg_value & 0xF4) | mode;

	if (i2c_smbus_write_byte_data(client, reg_address, linkreg_value) < 0) {
		printk(KERN_INFO "I2C_Write Error: output clk mode set failed\n");
		return -EIO;
	}

	if (mode == 0) {// LVDS
		reg_address = clkout_odr_inttermres_regvalue[siT95317Output2siT95316RegMap[clkid] % 6];

		linkreg_value = i2c_smbus_read_byte_data(client, reg_address);
		if (linkreg_value < 0) {
			printk(KERN_ERR "Failed to read byte via SMBus\n");
			return -EIO;
		}

		linkreg_value = (linkreg_value & 0x7F);

		if (itresistor) {
			linkreg_value = (linkreg_value & 0x7F) | (1 << 7);
		}
		if (i2c_smbus_write_byte_data(client, reg_address, linkreg_value) < 0) {
			printk(KERN_INFO "I2C_Write Error: output clk lvds internal res termination enable failed\n");
			return -EIO;
		}
	}

	/* Set page back to MAIN OUTSYS (Page3_Outsys) */
	if (i2c_smbus_write_byte_data(client, SI95317_PAGE_NUM, Page3_Outsys) < 0) {
		printk(KERN_ERR "Failed to write byte via SMBus\n");
		return -EIO;
	}

	//update NVM Bank & Proceed to Loop Lock
	ret = do_updatenvm_lockloop(client);
	if (ret < 0) {
		printk(KERN_ERR "Failed to update NVM Bank and Lock Loop\n");
		return ret;
	}

	return SUCCESS;	
}

static int set_swing_voltage(struct i2c_client *client, int clkid, unsigned int swing)
{
	/* Nyz : Move it to header file */
	static const unsigned int siT95317Output2siT95316RegMap[] = {0, 3, 4, 5, 7, 8, 9, 11};
	static const unsigned int clkout_odr_outswing_regvalue[] = { 0x1F, 0x2F, 0x3F, 0x4F, 0x5F, 0x6F};

	unsigned int reg_address = 0;
	unsigned int linkreg_value = 0;
	int ret = 0;

	// Escape to PRGCmd State
	ret = switch_to_PRGCmd_state(client, Page3_Outsys);
	if (ret < 0) {
		printk(KERN_ERR "Failed to Escape to PRGCmd State");
		return ret;
	}

	/* Switch to page 4 if output clock id is greater than 5 */
	if (siT95317Output2siT95316RegMap[clkid] > 5) {
		if (i2c_smbus_write_byte_data(client, SI95317_PAGE_NUM, Page4_Outsys) < 0) {
			printk(KERN_INFO "I2C_Write Error: switch to Page4_Outsys failed\n");
			return -EIO;
		}
	}

	reg_address = clkout_odr_outswing_regvalue[siT95317Output2siT95316RegMap[clkid] % 6];

	linkreg_value = i2c_smbus_read_byte_data(client, reg_address);
	if (linkreg_value < 0) {
		printk(KERN_ERR "Failed to read byte via SMBus\n");
		return -EIO;
	}

	swing = (swing / 100) - 1;

	linkreg_value = (linkreg_value & 0xF4) | swing;

	if (i2c_smbus_write_byte_data(client, reg_address, linkreg_value) < 0) {
		printk(KERN_INFO "I2C_Write Error: output clk swing set failed\n");
		return -EIO;
	}


	/* Set page back to MAIN OUTSYS (Page3_Outsys) */
	if (i2c_smbus_write_byte_data(client, SI95317_PAGE_NUM, Page3_Outsys) < 0) {
		printk(KERN_ERR "Failed to write byte via SMBus\n");
		return -EIO;
	}

	//update NVM Bank & Proceed to Loop Lock
	ret = do_updatenvm_lockloop(client);
	if (ret < 0) {
		printk(KERN_ERR "Failed to update NVM Bank and Lock Loop\n");
		return ret;
	}

	return SUCCESS;	
}

static int write_outclk_pll_bits(struct i2c_client *client, unsigned int pllid, u8 reg27_value, u8 reg28_value)
{
	static const unsigned int clkout_enable_pll_reg27 = 0x27;
	static const unsigned int clkout_enable_pll_reg28 = 0x28;

	unsigned int reg_value     = 0;

	unsigned int reg_address27  = 0;
	unsigned int reg_address28  = 0;

	reg_address27 = clkout_enable_pll_reg27;
	reg_address28 = clkout_enable_pll_reg28;

	/* Set page to MAIN INPUTSYS (Page2_Inputsys) */
	if (i2c_smbus_write_byte_data(client, SI95317_PAGE_NUM, PageA_PLLA + pllid) < 0) {
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

	//Read existing register configuration
	reg_value = i2c_smbus_read_byte_data(client, reg_address27);
	if (reg_value < 0) {
		printk(KERN_ERR "Failed to read byte via SMBus\n");
		return -EIO;
	}

	reg_value = (reg_value & 0xF0) | reg27_value;
	if (i2c_smbus_write_byte_data(client, reg_address27, reg_value) < 0) {
		printk(KERN_INFO "I2C_Write Error: output clk swing set failed\n");
		return -EIO;
	}

	if (i2c_smbus_write_byte_data(client, reg_address28, reg28_value) < 0) {
		printk(KERN_INFO "I2C_Write Error: output clk swing set failed\n");
		return -EIO;
	}

	/* Update the NVM bank and change to READY state */
	if (i2c_smbus_write_byte_data(client, SI95317_PRG_DIR_GEN_0, SI95317_UPDATE_NVM) < 0) {
		printk(KERN_INFO "I2C_Write Error: output clk enable failed\n");
		return -EIO;
	}

	msleep(1);

	/* Proceed to loop lock */
	if (i2c_smbus_write_byte_data(client, SI95317_PRG_DIR_GEN_0, SI95317_LOOPLOCK) < 0) {
		printk(KERN_INFO "I2C_Write Error: output clk enable failed\n");
		return -EIO;
	}

	return SUCCESS;
}


static int write_default_configuration(u16 chip_id)
{
	int ret = 0;

	switch(chip_id)
	{
		case SIT95316 :
		case SIT95317 :
			ret = siT9531x_write_multiple(g_brd, siT95317_def, ARRAY_SIZE(siT95317_def));
			break;
		case SIT95211:
			ret = siT9531x_write_multiple(g_brd, vek385_def, ARRAY_SIZE(vek385_def));
			break;
		default:
			ret = FAILED ;
	};

	return ret;
}

static int si9531x_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{


	int ret = SUCCESS;
	int err = 0;
	struct drv_si9531x *brd = NULL;
	struct clk_init_data init;
	int i, j;


	printk(KERN_INFO "Probed device :%ld Device name chip_id detected:0x%x\n", id->driver_data, client->addr);

	memset(&init, 0, sizeof(init));

	brd = devm_kzalloc(&client->dev, sizeof(*brd), GFP_KERNEL);

	brd->client = client;
	brd->chip_id = client->addr;

	switch(brd->chip_id)
	{
		case SIT95316:
			brd->chip_name   = device_support[0].chip_name;
			brd->num_inputs  = device_support[0].num_inputs;
			brd->num_outputs = device_support[0].num_outputs;
			break;

		case SIT95317:
			brd->chip_name   = device_support[1].chip_name;
			brd->num_inputs  = device_support[1].num_inputs;
			brd->num_outputs = device_support[1].num_outputs;
			break;

		case SIT95211:
			brd->chip_name  = device_support[2].chip_name;
			brd->num_inputs = device_support[2].num_inputs;
			brd->num_outputs= device_support[2].num_outputs;
			break;

		default: printk(KERN_ERR "UnSuported Chip Identified %x\n", brd->chip_id);
			 return FAILED;
	}

	ret = si9531x_dt_parse(brd);
	if (ret) {
		return ret;		
	}

	print_clk_driver(brd);
	i2c_set_clientdata(client, brd);
	g_brd = brd;

	/* Register Input Clocks */
	for (i = 0; i < brd->num_inputs; i++) {
		init.name = si9531x_input_names[i];
		init.ops = &si9531x_input_clk_ops;
		brd->input_clk[i].hw.init = &init;
		brd->input_clk[i].data = brd;
		brd->clkin[i] = devm_clk_register(&client->dev, &brd->input_clk[i].hw);
		if (IS_ERR(brd->clkin[i])) {
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
		brd->clkout[j] = devm_clk_register(&client->dev, &brd->output_clk[j].hw);
		if (IS_ERR(brd->clkout[j])) {
			printk(KERN_ERR "Failed clk register ops for Output clk: %d\n", j);
			return PTR_ERR(brd->clkout[j]); // Proper error return
		}

		// Add the clock to the clock provider
		err = devm_of_clk_add_hw_provider(&client->dev, of_clk_hw_simple_get, &brd->output_clk[j].hw);
		if (err) {
			printk(KERN_ERR "Unable to add clk provider for Output clk: %d\n", j);
			return err;
		}
	}


	if (brd->eeprom_override == ON) {
		err = write_default_configuration(brd->chip_id);
		if (ret < 0) {
			printk(KERN_INFO "Default Registration Write reg failed\n");
		}

		err = configure_clk(brd);
		if (err) {
			printk(KERN_ERR "Failed to configure clk for Output clk: \n" );
			return err;
		}
	} else {
		//Need to add print here
	}

	ret = configure_sysfs_outputclk(brd->chip_id);
	if (ret) {
		printk(KERN_ERR "Failed to configure output clk sysfs for chipID :%x\n", brd->chip_id);
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
	struct drv_si9531x *brd = NULL;

	brd = devm_kzalloc(&client->dev, sizeof(*brd), GFP_KERNEL);

	printk(KERN_INFO "%s:%d\n", __func__, __LINE__);
	remove_sysfs_outputclk();
	class_destroy(brd->siT_cl);
	cdev_del(brd->siT_cdev);
	unregister_chrdev_region(brd->ldev_node, 1);
	return 0;
}

static const struct i2c_device_id si9531x_i2c_id[] = {
	{"siT95316", 0},					//Need to check here for i2c address.
	{"siT95317", 1},					//Need to check here for i2c address.
	{"siT95211", 2},					//Need to check here for i2c address.
	{ }
};

MODULE_DEVICE_TABLE(i2c, si9531x_i2c_id);

static const struct of_device_id si9531x_dt_id[] = {
	{ .compatible = "SiTime,siT95316" },
	{ .compatible = "siTime,siT95317" },
	{ .compatible = "sitime,sit95211" },
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
MODULE_AUTHOR("Ali Rouhi <arouhi@sitime.com>");
MODULE_DESCRIPTION("SiTime SiT9531x CCF driver");
MODULE_LICENSE("GPL");
