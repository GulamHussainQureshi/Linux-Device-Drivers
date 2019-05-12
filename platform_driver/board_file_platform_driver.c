
/* Platform device resources and platdata registered in board init file 
 * in the arch/<arch>/mach-xxx/yyyy.c file (when kernel with board file is used)
 * 
 * struct my_gpios {
 *    int reset_gpio;
 *    int led_gpio;
 *    }
 *
 *      // our platform data //
 * static struct my_gpios needed_gpios = {  
 *    .reset_gpio = 47,
 *    .led_gpio = 41,
 *    }
 *
 *  #define BASE_DEV_ADDR 0x001004F1
 *  #define BASE_DEV_ADDR2 0x001005F1
 *  #define DEV_IRQ 27
 *
 *
 *    // our resource array //
 * static struct resource needed_resources [] = {
 *    [0] = { .start = BASE_DEV_ADDR,
 *            .end = BASE_DEV_ADDR + 0x1000 - 1,
 *            .flags = IORESOURCE_MEM,
 *            .name = "mem1",
 *          }
 *
 *    [1] = { .start = BASE_DEV_ADDR2,
 *            .end = BASE_DEV_ADDR + 0x01FF - 1,
 *            .flags = IORESOURCE_MEM,
 *            .name = "mem2",
 *          }
 *
 *    [1] = { .start = DEV_IR,
 *            .end = DEV_IRQ,
 *            .flags = IORESOURCE_IRQ,
 *            .name = "irq1",
 *          }
 *    }
 *
 * static struct platform_device my_device {
 *    .name = "my-platform-device",
 *    .id = 0,
 *    .dev = {
 *            .platform_data = &needed_gpios,
 *           },
 *    .resource = needed_resources,
 *    .num_resources = ARRAY_SIZE(needed_resources),
 *  };
 *
 * platform_device_register(&my_device);
 *
 */

#include <linux/module.h> 	
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>	 	/* for file operations struct*/
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/platform_device.h> /* For platform devices */

struct my_gpios {
  int reset_gpio;
  int led_gpio;
};

struct my_gpios *gpios;
struct resource *res1, *res2;
int irqnum = 0;

void __iomem *reg1, *reg2;

static int my_driver_probe(struct platform_device *pdev)
{
  gpios = (struct my_gpios*) dev_get_platdata(&pdev->dev);
  /* dev_get_platdata is used to retrieve the platform data registered in board file
        and passed to driver through struct platform_device */
  
  int rgpio = gpios->reset_gpio;
  int lgpio = gpios->led_gpio;
  
  /* after retrieving gpio numbers from the platform data, we can request those gpios using request_gpios() */
    
  /* getting the resources */
  res1 = platform_get_resource(pdev, IORESOURCE_MEM, 0);
  if(!res1) {
    pr_err(" First resource not available!!\n");
    return -1;
  }
  
  res2= platform_get_resource(pdev, IORESOURCE_MEM, 1);
  if(!res2) {
    pr_err(" Second resource not available!!\n");
    goto fail_second_res;
  }
  
  /* request memory regions and remapping */
  reg1 = ioremap(res1->start, resource_size(res1);
  reg2 = ioremap(res2->start, resource_size(res2);
                 
  /* reg1 and reg2 can now be used by the driver to perform reading/writing operations 
     using kernel provided functions to access ioremap'ed memories like 
     ioread8(), ioread16(), iowrite8(), iowrite32() etc */

  
  /* extract the irq */
  int ret;
  
  irqnum = platform_get_irq(pdev, 0);
  if(!irqnum) {
    pr_err(" Unable to get IRQ num!!\n");
    goto fail_irq;
  }
  
  pr_info(" IRQ number of DEVICE: %d\n", irqnum);
                 
  ret = request_irq(irqnum, my_irq_handler, IRQF_ONESHOT, pdev->driver.name, pdev->dev);
  if(ret!=0) 
    goto fail_irq;
                 
  /* with irq registered we can do the required intialisation */
                 
  return 0;
               
fail_irq:
      iounmap(reg2);
    
fail_second_res:
      iounmap(reg1);
                
      return -1;
                 
}
               
static int my_driver_remove(struct platform device *pdev)
{
  /* free irq */
  free_irq(irqnum, pdev->dev);
  
  /* unmap memory regions */
  iounmap(reg1);
  iounmap(reg2);

  return 0;
}
                 
static struct platform_driver mypdrv = {
    .probe = my_driver_probe,
    .remove = my_driver_remove,
    .driver = {
      .name = "my-platform-device",
      .owner = THIS_MODULE,
    },
};
                 
module_platform_driver(mypdrv);
                 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("GULAM HUSSAIN QURESHI");                 
