/*This file contains all the sysfs file support for sitime devices*/
/*
 *  This File create Output clks Freq and status support in </sys/kernel/sitimeclk/> folder
 */
#include "siT95317.h"

static struct kobject *kobj[7],*kobjparent;
extern struct drv_si9531x * g_brd;

extern int get_outclk_pll(unsigned int clkid);
extern void clkoutput_frequency(const char *buff, unsigned int clkid);
extern void clkoutput_status(const char *buff, unsigned int clkid);

/**************************clkout0*****************************************/
static ssize_t clkout0_freq_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	int clkid = 0u;
	return snprintf(buf, PAGE_SIZE, "%u\n", g_brd->output_clk[clkid].freq);
}

static ssize_t clkout0_freq_store(struct  kobject *kobj, struct kobj_attribute *attr, const char *buff, size_t count)
{
	int clkid = 0u;

	clkoutput_frequency(buff, clkid);
	return count;
}

static ssize_t clkout0_status_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	int clkid = 0u;
	return snprintf(buf, PAGE_SIZE, "%u\n", g_brd->output_clk[clkid].status);
}

static ssize_t clkout0_status_store(struct  kobject *kobj, struct kobj_attribute *attr, const char *buff, size_t count)
{
	int clkid = 0u;

	clkoutput_status(buff, clkid);
	return count;
}

static struct kobj_attribute clkout0_attr_status =
__ATTR(status, S_IWUSR | S_IRUGO, clkout0_status_show, clkout0_status_store);

static struct kobj_attribute clkout0_attr_frequency =
__ATTR(frequency, S_IWUSR | S_IRUGO, clkout0_freq_show, clkout0_freq_store);

static struct attribute *sitime_attributes0[] = {
	&clkout0_attr_status.attr,
	&clkout0_attr_frequency.attr,
	NULL,
};

static const struct attribute_group sitime_attrclk0_group = {
	.attrs = sitime_attributes0,
};

/**************************clkout1*****************************************/
static ssize_t clkout1_freq_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	int clkid = 1u;
	return snprintf(buf, PAGE_SIZE, "%u\n", g_brd->output_clk[clkid].freq);
}

static ssize_t clkout1_freq_store(struct  kobject *kobj, struct kobj_attribute *attr, const char *buff, size_t count)
{
	int clkid = 1u;

	clkoutput_frequency(buff, clkid);
	return count;
}

static ssize_t clkout1_status_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	int clkid = 1u;
	return snprintf(buf, PAGE_SIZE, "%u\n", g_brd->output_clk[clkid].status);
}

static ssize_t clkout1_status_store(struct  kobject *kobj, struct kobj_attribute *attr, const char *buff, size_t count)
{
	int clkid = 1u;

	clkoutput_status(buff, clkid);
	return count;
}

static struct kobj_attribute clkout1_attr_status =
__ATTR(status, S_IWUSR | S_IRUGO, clkout1_status_show, clkout1_status_store);

static struct kobj_attribute clkout1_attr_frequency =
__ATTR(frequency, S_IWUSR | S_IRUGO, clkout1_freq_show, clkout1_freq_store);

static struct attribute *sitime_attributes1[] = {
	&clkout1_attr_status.attr,
	&clkout1_attr_frequency.attr,
	NULL,
};

static const struct attribute_group sitime_attrclk1_group = {
	.attrs = sitime_attributes1,
};
/**************************clkout2*****************************************/
static ssize_t clkout2_freq_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	int clkid = 2u;
	return snprintf(buf, PAGE_SIZE, "%u\n", g_brd->output_clk[clkid].freq);
}

static ssize_t clkout2_freq_store(struct  kobject *kobj, struct kobj_attribute *attr, const char *buff, size_t count)
{
	int clkid = 2u;

	clkoutput_frequency(buff, clkid);
	return count;
}

static ssize_t clkout2_status_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	int clkid = 2u;
	return snprintf(buf, PAGE_SIZE, "%u\n", g_brd->output_clk[clkid].status);
}

static ssize_t clkout2_status_store(struct  kobject *kobj, struct kobj_attribute *attr, const char *buff, size_t count)
{
	int clkid = 2u;

	clkoutput_status(buff, clkid);
	return count;
}

static struct kobj_attribute clkout2_attr_status =
__ATTR(status, S_IWUSR | S_IRUGO, clkout2_status_show, clkout2_status_store);

static struct kobj_attribute clkout2_attr_frequency =
__ATTR(frequency, S_IWUSR | S_IRUGO, clkout2_freq_show, clkout2_freq_store);

static struct attribute *sitime_attributes2[] = {
	&clkout2_attr_status.attr,
	&clkout2_attr_frequency.attr,
	NULL,
};

static const struct attribute_group sitime_attrclk2_group = {
	.attrs = sitime_attributes2,
};
/**************************clkout3*****************************************/
static ssize_t clkout3_freq_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	int clkid = 3u;
	return snprintf(buf, PAGE_SIZE, "%u\n", g_brd->output_clk[clkid].freq);
}

static ssize_t clkout3_freq_store(struct  kobject *kobj, struct kobj_attribute *attr, const char *buff, size_t count)
{
	int clkid = 3u;

	clkoutput_frequency(buff, clkid);
	return count;
}

static ssize_t clkout3_status_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	int clkid = 3u;
	return snprintf(buf, PAGE_SIZE, "%u\n", g_brd->output_clk[clkid].status);
}

static ssize_t clkout3_status_store(struct  kobject *kobj, struct kobj_attribute *attr, const char *buff, size_t count)
{
	int clkid = 3u;

	clkoutput_status(buff, clkid);
	return count;
}

static struct kobj_attribute clkout3_attr_status =
__ATTR(status, S_IWUSR | S_IRUGO, clkout3_status_show, clkout3_status_store);

static struct kobj_attribute clkout3_attr_frequency =
__ATTR(frequency, S_IWUSR | S_IRUGO, clkout3_freq_show, clkout3_freq_store);

static struct attribute *sitime_attributes3[] = {
	&clkout3_attr_status.attr,
	&clkout3_attr_frequency.attr,
	NULL,
};

static const struct attribute_group sitime_attrclk3_group = {
	.attrs = sitime_attributes3,
};
/**************************clkout4*****************************************/
static ssize_t clkout4_freq_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	int clkid = 4u;
	return snprintf(buf, PAGE_SIZE, "%u\n", g_brd->output_clk[clkid].freq);
}

static ssize_t clkout4_freq_store(struct  kobject *kobj, struct kobj_attribute *attr, const char *buff, size_t count)
{
	int clkid = 4u;

	clkoutput_frequency(buff, clkid);
	return count;
}

static ssize_t clkout4_status_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	int clkid = 4u;
	return snprintf(buf, PAGE_SIZE, "%u\n", g_brd->output_clk[clkid].status);
}

static ssize_t clkout4_status_store(struct  kobject *kobj, struct kobj_attribute *attr, const char *buff, size_t count)
{
	int clkid = 4u;

	clkoutput_status(buff, clkid);
	return count;
}

static struct kobj_attribute clkout4_attr_status =
__ATTR(status, S_IWUSR | S_IRUGO, clkout4_status_show, clkout4_status_store);

static struct kobj_attribute clkout4_attr_frequency =
__ATTR(frequency, S_IWUSR | S_IRUGO, clkout4_freq_show, clkout4_freq_store);

static struct attribute *sitime_attributes4[] = {
	&clkout4_attr_status.attr,
	&clkout4_attr_frequency.attr,
	NULL,
};

static const struct attribute_group sitime_attrclk4_group = {
	.attrs = sitime_attributes4,
};
/**************************clkout5*****************************************/
static ssize_t clkout5_freq_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	int clkid = 5u;
	return snprintf(buf, PAGE_SIZE, "%u\n", g_brd->output_clk[clkid].freq);
}

static ssize_t clkout5_freq_store(struct  kobject *kobj, struct kobj_attribute *attr, const char *buff, size_t count)
{
	int clkid = 5u;

	clkoutput_frequency(buff, clkid);
	return count;
}

static ssize_t clkout5_status_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	int clkid = 5u;
	return snprintf(buf, PAGE_SIZE, "%u\n", g_brd->output_clk[clkid].status);
}

static ssize_t clkout5_status_store(struct  kobject *kobj, struct kobj_attribute *attr, const char *buff, size_t count)
{
	int clkid = 5u;

	clkoutput_status(buff, clkid);
	return count;
}

static struct kobj_attribute clkout5_attr_status =
__ATTR(status, S_IWUSR | S_IRUGO, clkout5_status_show, clkout5_status_store);

static struct kobj_attribute clkout5_attr_frequency =
__ATTR(frequency, S_IWUSR | S_IRUGO, clkout5_freq_show, clkout5_freq_store);

static struct attribute *sitime_attributes5[] = {
	&clkout5_attr_status.attr,
	&clkout5_attr_frequency.attr,
	NULL,
};

static const struct attribute_group sitime_attrclk5_group = {
	.attrs = sitime_attributes5,
};
/**************************clkout6*****************************************/
static ssize_t clkout6_freq_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	int clkid = 6u;
	return snprintf(buf, PAGE_SIZE, "%u\n", g_brd->output_clk[clkid].freq);
}

static ssize_t clkout6_freq_store(struct  kobject *kobj, struct kobj_attribute *attr, const char *buff, size_t count)
{
	int clkid = 6u;

	clkoutput_frequency(buff, clkid);
	return count;
}

static ssize_t clkout6_status_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	int clkid = 6u;
	return snprintf(buf, PAGE_SIZE, "%u\n", g_brd->output_clk[clkid].status);
}

static ssize_t clkout6_status_store(struct  kobject *kobj, struct kobj_attribute *attr, const char *buff, size_t count)
{
	int clkid = 6u;

	clkoutput_status(buff, clkid);
	return count;
}

static struct kobj_attribute clkout6_attr_status =
__ATTR(status, S_IWUSR | S_IRUGO, clkout6_status_show, clkout6_status_store);

static struct kobj_attribute clkout6_attr_frequency =
__ATTR(frequency, S_IWUSR | S_IRUGO, clkout6_freq_show, clkout6_freq_store);

static struct attribute *sitime_attributes6[] = {
	&clkout6_attr_status.attr,
	&clkout6_attr_frequency.attr,
	NULL,
};

static const struct attribute_group sitime_attrclk6_group = {
	.attrs = sitime_attributes6,
};
/**************************clkout7*****************************************/
static ssize_t clkout7_freq_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	int clkid = 7u;
	return snprintf(buf, PAGE_SIZE, "%u\n", g_brd->output_clk[clkid].freq);
}

static ssize_t clkout7_freq_store(struct  kobject *kobj, struct kobj_attribute *attr, const char *buff, size_t count)
{
	int clkid = 7u;

	clkoutput_frequency(buff, clkid);
	return count;
}

static ssize_t clkout7_status_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	int clkid = 7u;
	return snprintf(buf, PAGE_SIZE, "%u\n", g_brd->output_clk[clkid].status);
}

static ssize_t clkout7_status_store(struct  kobject *kobj, struct kobj_attribute *attr, const char *buff, size_t count)
{
	int clkid = 7u;

	clkoutput_status(buff, clkid);
	return count;
}

static struct kobj_attribute clkout7_attr_status =
__ATTR(status, S_IWUSR | S_IRUGO, clkout7_status_show, clkout7_status_store);

static struct kobj_attribute clkout7_attr_frequency =
__ATTR(frequency, S_IWUSR | S_IRUGO, clkout7_freq_show, clkout7_freq_store);

static struct attribute *sitime_attributes7[] = {
	&clkout7_attr_status.attr,
	&clkout7_attr_frequency.attr,
	NULL,
};

static const struct attribute_group sitime_attrclk7_group = {
	.attrs = sitime_attributes7,
};

/**************************clkout8*****************************************/
static ssize_t clkout8_freq_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	int clkid = 8u;
	return snprintf(buf, PAGE_SIZE, "%u\n", g_brd->output_clk[clkid].freq);
}

static ssize_t clkout8_freq_store(struct  kobject *kobj, struct kobj_attribute *attr, const char *buff, size_t count)
{
	int clkid = 8u;

	clkoutput_frequency(buff, clkid);
	return count;
}

static ssize_t clkout8_status_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	int clkid = 8u;
	return snprintf(buf, PAGE_SIZE, "%u\n", g_brd->output_clk[clkid].status);
}

static ssize_t clkout8_status_store(struct  kobject *kobj, struct kobj_attribute *attr, const char *buff, size_t count)
{
	int clkid = 8u;

	clkoutput_status(buff, clkid);
	return count;
}

static struct kobj_attribute clkout8_attr_status =
__ATTR(status, S_IWUSR | S_IRUGO, clkout8_status_show, clkout8_status_store);

static struct kobj_attribute clkout8_attr_frequency =
__ATTR(frequency, S_IWUSR | S_IRUGO, clkout8_freq_show, clkout8_freq_store);

static struct attribute *sitime_attributes8[] = {
	&clkout8_attr_status.attr,
	&clkout8_attr_frequency.attr,
	NULL,
};

static const struct attribute_group sitime_attrclk8_group = {
	.attrs = sitime_attributes8,
};

/**************************clkout9*****************************************/
static ssize_t clkout9_freq_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	int clkid = 9u;
	return snprintf(buf, PAGE_SIZE, "%u\n", g_brd->output_clk[clkid].freq);
}

static ssize_t clkout9_freq_store(struct  kobject *kobj, struct kobj_attribute *attr, const char *buff, size_t count)
{
	int clkid = 9u;

	clkoutput_frequency(buff, clkid);
	return count;
}

static ssize_t clkout9_status_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	int clkid = 9u;
	return snprintf(buf, PAGE_SIZE, "%u\n", g_brd->output_clk[clkid].status);
}

static ssize_t clkout9_status_store(struct  kobject *kobj, struct kobj_attribute *attr, const char *buff, size_t count)
{
	int clkid = 9u;

	clkoutput_status(buff, clkid);
	return count;
}

static struct kobj_attribute clkout9_attr_status =
__ATTR(status, S_IWUSR | S_IRUGO, clkout9_status_show, clkout9_status_store);

static struct kobj_attribute clkout9_attr_frequency =
__ATTR(frequency, S_IWUSR | S_IRUGO, clkout9_freq_show, clkout9_freq_store);

static struct attribute *sitime_attributes9[] = {
	&clkout9_attr_status.attr,
	&clkout9_attr_frequency.attr,
	NULL,
};

static const struct attribute_group sitime_attrclk9_group = {
	.attrs = sitime_attributes9,
};

/**************************clkout10*****************************************/
static ssize_t clkout10_freq_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	int clkid = 10u;
	return snprintf(buf, PAGE_SIZE, "%u\n", g_brd->output_clk[clkid].freq);
}

static ssize_t clkout10_freq_store(struct  kobject *kobj, struct kobj_attribute *attr, const char *buff, size_t count)
{
	int clkid = 10u;

	clkoutput_frequency(buff, clkid);
	return count;
}

static ssize_t clkout10_status_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	int clkid = 10u;
	return snprintf(buf, PAGE_SIZE, "%u\n", g_brd->output_clk[clkid].status);
}

static ssize_t clkout10_status_store(struct  kobject *kobj, struct kobj_attribute *attr, const char *buff, size_t count)
{
	int clkid = 10u;

	clkoutput_status(buff, clkid);
	return count;
}

static struct kobj_attribute clkout10_attr_status =
__ATTR(status, S_IWUSR | S_IRUGO, clkout10_status_show, clkout10_status_store);

static struct kobj_attribute clkout10_attr_frequency =
__ATTR(frequency, S_IWUSR | S_IRUGO, clkout10_freq_show, clkout10_freq_store);

static struct attribute *sitime_attributes10[] = {
	&clkout10_attr_status.attr,
	&clkout10_attr_frequency.attr,
	NULL,
};

static const struct attribute_group sitime_attrclk10_group = {
	.attrs = sitime_attributes10,
};

/**************************clkout11*****************************************/
static ssize_t clkout11_freq_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	int clkid = 11u;
	return snprintf(buf, PAGE_SIZE, "%u\n", g_brd->output_clk[clkid].freq);
}

static ssize_t clkout11_freq_store(struct  kobject *kobj, struct kobj_attribute *attr, const char *buff, size_t count)
{
	int clkid = 11u;

	clkoutput_frequency(buff, clkid);
	return count;
}

static ssize_t clkout11_status_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	int clkid = 11u;
	return snprintf(buf, PAGE_SIZE, "%u\n", g_brd->output_clk[clkid].status);
}

static ssize_t clkout11_status_store(struct  kobject *kobj, struct kobj_attribute *attr, const char *buff, size_t count)
{
	int clkid = 11u;

	clkoutput_status(buff, clkid);
	return count;
}

static struct kobj_attribute clkout11_attr_status =
__ATTR(status, S_IWUSR | S_IRUGO, clkout11_status_show, clkout11_status_store);

static struct kobj_attribute clkout11_attr_frequency =
__ATTR(frequency, S_IWUSR | S_IRUGO, clkout11_freq_show, clkout11_freq_store);

static struct attribute *sitime_attributes11[] = {
	&clkout11_attr_status.attr,
	&clkout11_attr_frequency.attr,
	NULL,
};

static const struct attribute_group sitime_attrclk11_group = {
	.attrs = sitime_attributes11,
};
/*****************************************************************************************/

static const struct attribute_group sitime_attr_group[] = {
	sitime_attrclk0_group,
	sitime_attrclk1_group,
	sitime_attrclk2_group,
	sitime_attrclk3_group,
	sitime_attrclk4_group,
	sitime_attrclk5_group,
	sitime_attrclk6_group,
	sitime_attrclk7_group,
	sitime_attrclk8_group,
	sitime_attrclk9_group,
	sitime_attrclk10_group,
	sitime_attrclk11_group
};

int configure_sysfs_outputclk(const char * chip_name)
{
	int ret   = SUCCESS;
	int index = 0;
	char ptr[20];
	static int count;
	
	snprintf(ptr, 20, "sitimeclk_%s_%d", chip_name,count++);

	kobjparent = kobject_create_and_add(ptr, kernel_kobj);
	if (!kobjparent) {
		return -ENOMEM;
	}

	for (index = 0; index < g_brd->chip_info->num_outputs; index++) {

		kobj[index] = kobject_create_and_add(g_brd->output_clk[index].clkName, kobjparent);
		if (!kobj[index]) {
			printk(KERN_INFO "Unable to create kobj for clk :%d \n", index);
			return -ENOMEM;
		}

		ret = sysfs_create_group(kobj[index], &sitime_attr_group[index]);	
		if (ret) {
			printk(KERN_INFO "Unable to create sysfs files\n");
			return ret;
		}
	}
	return ret;
}

void remove_sysfs_outputclk(void)
{
	int index = 0;
	kobject_put(kobjparent);

	for (index = 0; index < g_brd->chip_info->num_outputs; index++) {

		kobject_put(kobj[index]);
		sysfs_remove_group(kernel_kobj, &sitime_attr_group[index]);
	}
}
