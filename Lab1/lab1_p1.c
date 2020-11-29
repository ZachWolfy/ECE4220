#ifndef MODULE
#define MODULE
#endif
#ifndef __KERNEL__
#define __KERNEL__
#endif

#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/io.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/delay.h>

MODULE_LICENSE("GPL");

//define addresses and offset
#define ADD_BASE 0x3F200000 //address base for GPIO & GPFSEL0
#define GPSET0_OFFSET 0x07 //GPSET0 offset from GPIO base address
#define GPCLR0_OFFSET 0x0A //GPCLR0 offset from GPIO base address



int my_init(void)
{
	unsigned long *GPFSEL0 = (unsigned long*)ioremap(ADD_BASE, 4096);//set GPFSEL0 address and size
	unsigned long *GPSET0 = GPFSEL0 + GPSET0_OFFSET;//set GPSET0 address by copying GPFSEL0 and offsetting the address
	unsigned long *GPCLR0 = GPFSEL0 + GPCLR0_OFFSET;//set GPCLR0 address by copying GPFSEL0 and offsetting the address
	
	
	//checking to make sure using proper address
	printk("GPFSEL0 add: %X",GPFSEL0);
	printk("GPSET0 add: %X",GPSET0);
	printk("GPCLR0 add: %X",GPCLR0);
	printk("gpfsel____last add: %X",GPFSEL0);
	
	//turning on led
	iowrite32(1 << 3, GPFSEL0); //shift 0001 to the left 3 times
	iowrite32(1 << 3, GPSET0); //shift 0001 to the left 3 times

	return 0;
}

// Cleanup - undo whatever init_module did
void my_clean(void)
{
	printk("Goodbye!");
	printk("Goodnight!");
	unsigned long *GPFSEL0 = (unsigned long*)ioremap(ADD_BASE, 4096);//set GPFSEL0 address and size
	unsigned long *GPCLR0 = GPFSEL0 + GPCLR0_OFFSET; //set GPCLR0 address by copying GPFSEL0 and offsetting the address 4096)
	
	iowrite32(1 << 6, GPFSEL0);//shift 0001 to the left 6 times
	iowrite32(1 << 3, GPCLR0);//shift 0001 to the left 3 times
}

module_init(my_init);
module_exit(my_clean);

