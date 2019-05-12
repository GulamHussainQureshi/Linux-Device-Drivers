#include <linux/module.h> 	
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>	 	/* for file operations struct*/
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/platform_device.h> /* For platform devices */

enum abx80x_chip {
  AB0801,
  AB0802,
  AB0803,
  AB0804,
  AB1801,
  AB1802,
  AB1803,
  ABX80X,
}  

struct abx80x_cap {
  u16 pn;
  bool has_tc;
};

static struct abx80x_cap abx80x_caps[] = {
  [AB0801] = {.pn = 0x02A1},
  [AB0802] = {.pn = 0x0023},
  [AB0803] = {.pn = 0x0342, .has_tc = true},
  [AB0804] = {.pn = 0x03A1, .has_tc = true},
  [AB1801] = {.pn = 0x0341},
  [AB1802] = {.pn = 0x0A01},
  [AB1803] = {.pn = 0x0310, .has_tc = true},
  [ABX80X] = {.pn = 0}
}

static const struct platform_device_id abx80x_id[] = {
  { "abx80x", ABX80X },
  { "ab0801", AB0801 },
  { "ab0801", AB0802 },
  { "ab0803", AB0803 },
  { "ab0804", AB0804 },
  { "ab1801", AB1801 },
  { "ab1802", AB1802 },
  { "ab1803", AB1803 },
  { "rv1803", AB0803 },
}

static int abx80x_probe(struct platform_device *dev)
{
  
