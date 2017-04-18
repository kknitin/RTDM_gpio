#ifndef GPIO_RTDM_H
#define GPIO_RTDM_H

#include <linux/ioctl.h>


#define RTGPIO_PROFILE_VER	1

#define GPIO_IOC_MAGIC		'r'

#define GPIO_SET_PIN	_IOW(GPIO_IOC_MAGIC, 1, int)
#define GPIO_GET_PIN	_IOR(GPIO_IOC_MAGIC, 2, int)
#define GPIO_SET_DIR	_IOW(GPIO_IOC_MAGIC, 3, int)
#define GPIO_GET_DIR	_IOR(GPIO_IOC_MAGIC, 4, int)
#define GPIO_SET_VAL	_IOW(GPIO_IOC_MAGIC, 5, int)
#define GPIO_GET_VAL	_IOR(GPIO_IOC_MAGIC, 6, int)


#endif
