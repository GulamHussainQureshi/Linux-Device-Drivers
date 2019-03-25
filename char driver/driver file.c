#include <linux/module.h> 	
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>	 	/* for file operations struct*/
#include <linux/cdev.h>
#include <linux/device.h>

dev_t dev = 0;
