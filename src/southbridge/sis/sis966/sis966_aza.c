/*
 * This file is part of the LinuxBIOS project.
 *
 * Copyright (C) 2004 Tyan Computer
 * Written by Yinghai Lu <yhlu@tyan.com> for Tyan Computer.
 * Copyright (C) 2006,2007 AMD
 * Written by Yinghai Lu <yinghai.lu@amd.com> for AMD.
 * Copyright (C) 2007 Silicon Integrated Systems Corp. (SiS)
 * Written by Morgan Tsai <my_tsai@sis.com> for SiS.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <arch/io.h>
#include "sis966.h"

uint8_t	SiS_SiS7502_init[6][3]={

{0x04, 0xFF, 0x07},					 
{0x2C, 0xFF, 0x39},					 
{0x2D, 0xFF, 0x10},					 
{0x2E, 0xFF, 0x91},					 
{0x2F, 0xFF, 0x01},
{0x04, 0xFF, 0x06},
{0x00, 0x00, 0x00}					//End of table
};

static int set_bits(uint8_t *port, uint32_t mask, uint32_t val)
{
	uint32_t dword;
	int count;

	val &= mask;
	dword = readl(port);
	dword &= ~mask;
	dword |= val;
	writel(dword, port);

	count = 50;
	do {
		dword = readl(port);
		dword &= mask;
		udelay(100);
	} while ((dword != val) && --count);

	if(!count) return -1;

	udelay(540);
	return 0;

}

 uint32_t send_verb(uint8_t *base, uint32_t verb)
{


     uint32_t dword;
	 
     dword = readl(base + 0x68);
     dword=dword|(unsigned long)0x0002;
     writel(dword,base + 0x68);
     do {	  	
	 	dword = readl(base + 0x68);	   
     }  while ((dword & 1)!=0);
     writel(verb, base + 0x60);
     udelay(500);	 
     dword = readl(base + 0x68);	  
     dword =(dword |0x1);   
     writel(dword, base + 0x68);
     do {
	 	udelay(120);
		dword = readl(base + 0x68);
     } while ((dword & 3) != 2);

     dword = readl(base + 0x64);
     return dword;
		
}


#if 1
static int codec_detect(uint8_t *base)
{
	uint32_t dword;
	int idx=0;

	/* 1 */ // controller reset
	printk_debug("controller reset\n");
      
	set_bits(base + 0x08, 1, 1);
      
      do{
	  	dword = readl(base + 0x08)&0x1;	
		if(idx++>1000) { printk_debug("controller reset fail !!! \n"); break;}
	   } while (dword !=1);
       
       dword=send_verb(base,0x000F0000); // get codec VendorId and DeviceId
       
       if(dword==0) {
	   	printk_debug("No codec!\n");
		return 0;
       }

	 printk_debug("Codec ID = %lx\n", dword);
	  
#if 0
	/* 2 */
	dword = readl(base + 0x0e);
	dword |= 7;
	writel(dword, base + 0x0e);

	/* 3 */
	set_bits(base + 0x08, 1, 0);

	/* 4 */
	set_bits(base + 0x08, 1, 1);
		
	/* 5 */
	dword = readl(base + 0xe);
	dword &= 7;
       
	/* 6 */
	if(!dword) {
		set_bits(base + 0x08, 1, 0);
		printk_debug("No codec!\n");
		return 0;
	}
#endif 
       dword=0x1;
	return dword;

}

#else

static int codec_detect(uint8_t *base)
{
	uint32_t dword;

	/* 1 */
	set_bits(base + 0x08, 1, 1);

	/* 2 */
	dword = readl(base + 0x0e);
	dword |= 7;
	writel(dword, base + 0x0e);

	/* 3 */
	set_bits(base + 0x08, 1, 0);

	/* 4 */
	set_bits(base + 0x08, 1, 1);

	/* 5 */
	dword = readl(base + 0xe);
	dword &= 7;

	/* 6 */
	if(!dword) {
		set_bits(base + 0x08, 1, 0);
		printk_debug("No codec!\n");
		return 0;
	}
	return dword;

}

#endif

#if 1

// For SiS demo board PinConfig
static uint32_t verb_data[] = {
#if 0
 00172083h,
 00172108h,
 001722ECh,
 00172310h,
#endif
//14
	0x01471c10,
	0x01471d40,
	0x01471e01,
	0x01471f01,
//15
	0x01571c12,
	0x01571d10,
	0x01571e01,
	0x01571f01,
//16
	0x01671c11,
	0x01671d60,
	0x01671e01,
	0x01671f01,
//17
	0x01771c14,
	0x01771d20,
	0x01771e01,
	0x01771f01,
//18
	0x01871c40,
	0x01871d98,
	0x01871ea1,
	0x01871f01,
//19
	0x01971c50,
	0x01971d98,
	0x01971ea1,
	0x01971f02,
//1a
	0x01a71c4f,
	0x01a71d30,
	0x01a71e81,
	0x01a71f01,
//1b
	0x01b71c20,
	0x01b71d40,
	0x01b71e01,
	0x01b71f02,
//1c
	0x01c71cf0,
	0x01c71d01,
	0x01c71e33,
	0x01c71f59,
//1d
	0x01d71c01,
	0x01d71de6,
	0x01d71e05,
	0x01d71f40,
//1e
	0x01e71c30,
	0x01e71d11,
	0x01e71e44,
	0x01e71f01,
//1f
	0x01f71c60,
	0x01f71d61,
	0x01f71ec4,
	0x01f71f01,
};

#else
// orginal codec pin configuration setting

static uint32_t verb_data[] = {
#if 0
	0x00172001,
	0x001721e6,
	0x00172200,
	0x00172300,
#endif

	0x01471c10,
	0x01471d44,
	0x01471e01,
	0x01471f01,
//1
	0x01571c12,
	0x01571d14,
	0x01571e01,
	0x01571f01,
//2
	0x01671c11,
	0x01671d60,
	0x01671e01,
	0x01671f01,
//3
	0x01771c14,
	0x01771d20,
	0x01771e01,
	0x01771f01,
//4
	0x01871c30,
	0x01871d9c,
	0x01871ea1,
	0x01871f01,
//5
	0x01971c40,
	0x01971d9c,
	0x01971ea1,
	0x01971f02,
//6
	0x01a71c31,
	0x01a71d34,
	0x01a71e81,
	0x01a71f01,
//7
	0x01b71c1f,
	0x01b71d44,
	0x01b71e21,
	0x01b71f02,
//8
	0x01c71cf0,
	0x01c71d11,
	0x01c71e11,
	0x01c71f41,
//9
	0x01d71c3e,
	0x01d71d01,
	0x01d71e83,
	0x01d71f99,
//10
	0x01e71c20,
	0x01e71d41,
	0x01e71e45,
	0x01e71f01,
//11
	0x01f71c50,
	0x01f71d91,
	0x01f71ec5,
	0x01f71f01,
};

#endif 
static unsigned find_verb(uint32_t viddid, uint32_t **verb)
{
        if((viddid == 0x10ec0883) || (viddid == 0x10ec0882) || (viddid == 0x10ec0880)) return 0;
	*verb =  (uint32_t *)verb_data;
	return sizeof(verb_data)/sizeof(uint32_t);
}


static void codec_init(uint8_t *base, int addr)
{
	uint32_t dword;
	uint32_t *verb;
	unsigned verb_size;
	int i;

	/* 1 */
	do {
		dword = readl(base + 0x68);
	} while (dword & 1);

	dword = (addr<<28) | 0x000f0000;
	writel(dword, base + 0x60);

	do {
		dword = readl(base + 0x68);
	} while ((dword & 3)!=2);

	dword = readl(base + 0x64);

	/* 2 */
	printk_debug("codec viddid: %08x\n", dword);
	verb_size = find_verb(dword, &verb);

	if(!verb_size) {
		printk_debug("No verb!\n");
		return;
	}

	printk_debug("verb_size: %d\n", verb_size);
	/* 3 */
	for(i=0; i<verb_size; i++) {
		send_verb(base,verb[i]);
#if 0
		do {
			dword = readl(base + 0x68);
		} while (dword & 1);

		writel(verb[i], base + 0x60);

		do {
			dword = readl(base + 0x68);
		} while ((dword & 3) != 2);
#endif 
	}
	printk_debug("verb loaded!\n");
}

static void codecs_init(uint8_t *base, uint32_t codec_mask)
{
	int i;
	codec_init(base, 0);
	return;
	for(i=2; i>=0; i--) {
		if( codec_mask & (1<<i) )
			codec_init(base, i);
	}
}

static void aza_init(struct device *dev)
{
	uint8_t *base;
	struct resource *res;
	uint32_t codec_mask;


//-------------- enable AZA (SiS7502) -------------------------
{
       uint8_t  temp8;
       int i=0;
	while(SiS_SiS7502_init[i][0] != 0)
	{				temp8 = pci_read_config8(dev, SiS_SiS7502_init[i][0]);
					temp8 &= SiS_SiS7502_init[i][1];
					temp8 |= SiS_SiS7502_init[i][2];									
					pci_write_config8(dev, SiS_SiS7502_init[i][0], temp8);
					i++;
	};
}
//-----------------------------------------------------------


// put audio to D0 state
pci_write_config8(dev, 0x54,0x00);

#if 0
{
    int i;
    printk_debug("Azalia PCI config \n");
    for(i=0;i<0xFF;i+=4)
    {
        if((i%16)==0)
        {
            print_debug("\r\n");print_debug_hex8(i);print_debug("  ");
        }
        print_debug_hex32(pci_read_config32(dev,i));
        print_debug("  ");    
    }
    print_debug("\r\n");
}
#endif 

	res = find_resource(dev, 0x10);
	if(!res)
		return;

	base =(uint8_t *) res->base;
	printk_debug("base = %08x\n", base);

	codec_mask = codec_detect(base);

	if(codec_mask) {
		printk_debug("codec_mask = %02x\n", codec_mask);
		codecs_init(base, codec_mask);
	}

#if 0
{
    int i;
    printk_debug("Azalia PCI config \n");
    for(i=0;i<0xFF;i+=4)
    {
        if((i%16)==0)
        {
            print_debug("\r\n");print_debug_hex8(i);print_debug("  ");
        }
        outl(0x80000800+i,0xcf8);     
        print_debug_hex32(inl(0xcfc));
        print_debug("  ");    
    }
    print_debug("\r\n");
}
#endif 

}

static void lpci_set_subsystem(device_t dev, unsigned vendor, unsigned device)
{
	pci_write_config32(dev, 0x40,
		((device & 0xffff) << 16) | (vendor & 0xffff));
}

static struct pci_operations lops_pci = {
	.set_subsystem	= lpci_set_subsystem,
};

static struct device_operations aza_audio_ops  = {
	.read_resources	= pci_dev_read_resources,
	.set_resources	= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
//	.enable		= sis966_enable,
	.init		= aza_init,
	.scan_bus	= 0,
	.ops_pci	= &lops_pci,
};

static struct pci_driver azaaudio_driver __pci_driver = {
	.ops	= &aza_audio_ops,
	.vendor	= PCI_VENDOR_ID_SIS,
	.device	= PCI_DEVICE_ID_SIS_SIS966_AZA,
};

