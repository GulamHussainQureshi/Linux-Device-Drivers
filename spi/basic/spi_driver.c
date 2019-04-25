#include <linux/kernel.h>



#define MAX_SPEED = 20000000;
#define BITS_PER_WORD = 16;
#define DRIVER_NAME = "simple_spi_driver"


struct device_data {
  struct mutex lock;
  struct spi_device *spi;
  char name;
}

static int spi_probe (struct spi_device *spi)
{
  int err;
  
  struct device_data *data;
  
  data = devm_kzalloc(&spi->dev, sizeof(struct device_data), GFP_KERNEL);
    if(!data) {
     turn -ENOMEM;
    }
  
  mutex_init(data->lock);
  
  data->spi = spi;
  data->name = DRIVER_NAME;
  
  spi->mode = SPI_MODE_0;
  spi->max_speed_hz = MAX_SPEED;
  spi->bits_per_word = 16;
  
  err = spi_setup(spi);
      if(err < 0)
          return err;
  
  
  spi_set_drvdata(spi,data);
  
  
  

} 


static struct spi_device_id spi_id_table[] = {
  { "xdevice", 0},
  { },
};

MODULE_DEVICE_TABLE(spi, spi_id_table);

struct spi_driver spidriver {
  .driver = {
    .owner = THIS_MODULE,
    .name = DRIVER_NAME,
    .of_match_table = of_match_ptr(spi_of_ids),
  },
  .probe = spi_probe,
  .remove = spi_remove,
  .id_table = spi_id_table,
}
