#include <linux/module.h> 	
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>	 	/* for file operations struct*/
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/platform_device.h> /* For platform devices */

/* Platform device resources and platdata registered in board init file 
 * in the arch/<arch>/mach-xxx/yyyy.c file (when kernel with board file is used)
 * 
 * struct my_gpios {
 *    int reset_gpio;
 *    int led_gpio;
 *    }
 *
 * static struct 
 */
