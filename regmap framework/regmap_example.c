#include <linux/kernel.h>




static int spi_regmap_probe(struct spi_device *spi)
{
  struct regmap_config config;
  struct custom_drv_data *priv;
  unsigned char data;
  
  /* setup the regmap configuration */
  memset(&config, 0, sizeof(config)); /* zeroing the memory area of struct regmap_config */
  config.reg_bits = 8;
  config.val_bits = 8;
  config.write_flag_mask = 0x80;
  config.max_register = 0x80;
  config.fast_io = true;
  config.writeable_reg = spi_writeable_reg;
  config.readable_reg = spi_readable_reg;
 
  /* can use read & write tables instead of writeable_reg() & readable_reg() */
  
  priv = kzalloc(sizeof(struct custom_drv_data), GFP_KERNEL);
  if(!priv) {
        pr_err("No memory.");
        return -ENOMEM;
  }
  
  priv->map = regmap_init_spi(spi, &config);
  
  
  
