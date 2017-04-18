//#include <linux/slab.h>
#include <linux/gpio.h>
#include <linux/module.h>

/* RTDM headers */
#include <rtdm/driver.h>
#include <rtdm/rtdm.h>

/*driver specific header*/
#include "rtdm_gpio_drv.h" 

#define RTDM_SUBCLASS_GPIO       0
#define DEVICE_NAME              "rtdm_gpio"

MODULE_DESCRIPTION("RTDM driver for Intel Joule GPIO");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nitin Kulkarni");
MODULE_VERSION("1.0");


typedef struct device_context {
	int gpio;
	int val;
	int dir;
	} gpio_dev_context;





static int gpio_open(struct rtdm_fd *fd, int oflags)
{
	gpio_dev_context *context;
	context	= (gpio_dev_context *) rtdm_fd_to_private(fd);
	
	context->gpio = -1;
	context->val = 0;
	context->dir = 0;
	

	return 0;
}








static void gpio_close(struct rtdm_fd *fd)
{
	gpio_dev_context *context = (gpio_dev_context *) rtdm_fd_to_private(fd);
	
	if (context->gpio >= 0)
		gpio_free(context->gpio);

}





static int gpio_ioctl_rt(struct rtdm_fd *fd, unsigned int request,
			 void __user *arg)
{
	gpio_dev_context *context = (gpio_dev_context *) rtdm_fd_to_private(fd);
	
	int result =0;

	switch(request) {

	case GPIO_SET_PIN:
		

		result = rtdm_safe_copy_from_user(fd, &context->gpio, arg, sizeof(int));
		

		if(result != 0)
		{
			printk("rtdm_safe_copy_from_user() failed when trying to SET PIN\n");
		}

		if (gpio_cansleep(context->gpio))
			return -EINVAL;

		result = gpio_request(context->gpio, "rtdm_gpio");

		if (result < 0) {
			
			printk("gpio_request failed to get the gpio %d\n",context->gpio);
			break;
		}
		context->dir = gpiod_get_direction(gpio_to_desc(context->gpio)) ? 1 : 0;
		context->val = gpiod_get_raw_value(gpio_to_desc(context->gpio));
		break;

	case GPIO_GET_PIN:
		if (!rtdm_rw_user_ok(fd, arg, sizeof(context->gpio)))
		{
			printk("rtdm_rw_user_ok() failed when trying to get PIN (gpio num) \n");
			return -EFAULT;
		}
		result = rtdm_copy_to_user(fd, arg, &context->gpio, sizeof(context->gpio));
		
		if(result != 0)
		{
			printk("rtdm_copy_to_user() failed when trying to get PIN\n");
		}
		
		break;

	case GPIO_SET_DIR:
		if (context->gpio == -1)
		{
			printk("context->gpio = -1 when trying to set direction\n");
			return -ENODEV;
		}

		result = rtdm_safe_copy_from_user(fd, &context->dir, arg, sizeof(int));
		

		if(result != 0)
		{
			printk("rtdm_safe_copy_from_user() failed when trying to SET DIR\n");
		}
		

		if (context->dir)
			gpio_direction_output(context->gpio, context->val);
		else
			gpio_direction_input(context->gpio);
		break;

	case GPIO_GET_DIR:
		if (context->gpio == -1)
		{
			printk("context->gpio = -1 when trying to get direction\n");
			return -ENODEV;
		}
	
		context->dir = gpiod_get_direction(gpio_to_desc(context->gpio)) ? 1 : 0;

		if (!rtdm_rw_user_ok(fd, arg, sizeof(context->dir)))
		{
			printk("rtdm_rw_user_ok() failed when trying to get direction\n");
			return -EFAULT;
		}

		result = rtdm_copy_to_user(fd, arg, &context->dir, sizeof(context->dir));

		if(result != 0)
		{
			printk("rtdm_copy_to_user() failed when trying to get direction\n");
		}

		break;

	case GPIO_SET_VAL:

		if (context->gpio == -1)
		{
			printk("context->gpio = -1 when trying to set value \n");			
			return -ENODEV;
		}

		result = rtdm_safe_copy_from_user(fd, &context->val, arg, sizeof(int));
		

		if(result != 0)
		{
			printk("rtdm_safe_copy_from_user() failed when trying to SET VAL\n");
		}		
	

		if (context->dir)
			gpio_set_value(context->gpio, context->val);
		break;

	case GPIO_GET_VAL:

		if (context->gpio == -1)
		{	
			printk("context->gpio = -1 when trying to get value \n");
			return -ENODEV;
		}
		context->val = gpiod_get_raw_value(gpio_to_desc(context->gpio));

		if (!rtdm_rw_user_ok(fd, arg, sizeof(context->val)))
		{
			printk("rtdm_rw_user_ok() failed when trying to get value\n");
			return -EFAULT;
		}
		result = rtdm_copy_to_user(fd, arg, &context->val, sizeof(context->val));
		if(result !=0)
		{
			printk("rtdm_copy_to_user() failed when trying to get value\n");
		}
		break;

	default:
		
		printk(" %s: Passed invalid argument to gpio ioctl \n",__FUNCTION__ );

		return -EINVAL;
	}

	return result;
}









static struct rtdm_driver gpio_driver = {
	.profile_info           = RTDM_PROFILE_INFO(gpio,
						    RTDM_CLASS_EXPERIMENTAL,
						    RTDM_SUBCLASS_GENERIC,
						    RTGPIO_PROFILE_VER),
	.device_flags		= RTDM_NAMED_DEVICE | RTDM_EXCLUSIVE,
	.device_count		= 1,
	.context_size		= sizeof( gpio_dev_context),
	.ops = {
		.open		= gpio_open,
		.close		= gpio_close,
		.ioctl_rt	= gpio_ioctl_rt,
	},
};	


static struct rtdm_device device = {

		.driver = &gpio_driver,
		.label = DEVICE_NAME,
};



static int __init gpio_init(void)
{
	int ret;

	printk(KERN_INFO "%s: Driver init running\n",__FUNCTION__);
	if (!realtime_core_enabled())
	{
		printk("realtime_core_enabled returned false ! \n");
		return -ENODEV;
	}
	
	ret = rtdm_dev_register(&device);
	if (ret)
		goto fail;
	
	return 0;

fail:
	rtdm_dev_unregister(&device);
	
	printk(KERN_INFO "%s: Driver init failed! \n",__FUNCTION__);
	return ret;
}


static void __exit gpio_exit(void)
{
	printk(KERN_INFO "%s: Driver init exiting...\n",__FUNCTION__);
	rtdm_dev_unregister(&device);
}

module_init(gpio_init);
module_exit(gpio_exit);

