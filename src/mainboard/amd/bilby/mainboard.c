/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <amdblocks/amd_pci_util.h>
#include <FspsUpd.h>
#include <gpio.h>
#include <soc/cpu.h>
#include <soc/southbridge.h>
#include <soc/pci_devs.h>
#include <soc/platform_descriptors.h>
#include <types.h>
#include <commonlib/helpers.h>
#include <soc/amd/picasso/chip.h>
#include "gpio.h"
#include "mainboard.h"

#define MAINBOARD_SHARED_DDI_PORTS 2

/* TODO: recheck IRQ tables */

/* The IRQ mapping in fch_irq_map ends up getting written to the indirect address space that is
   accessed via I/O ports 0xc00/0xc01. */
static const struct fch_irq_routing fch_irq_map[] = {
	{ PIRQ_A,	8,		16 },
	{ PIRQ_B,	10,		17 },
	{ PIRQ_C,	11,		18 },
	{ PIRQ_D,	12,		19 },
	{ PIRQ_SCI,	9,		 9 },
	{ PIRQ_SD,	PIRQ_NC,	16 },
	{ PIRQ_SDIO,	PIRQ_NC,	16 },
	{ PIRQ_SATA,	PIRQ_NC,	19 },
	{ PIRQ_EMMC,	PIRQ_NC,	17 },
	{ PIRQ_GPIO,	 7,		 7 },
	{ PIRQ_I2C2,	 6,		 6 },
	{ PIRQ_I2C3,	14,		14 },
	{ PIRQ_UART0,	 4,		 4 },
	{ PIRQ_UART1,	 3,		 3 },
	{ PIRQ_UART2,	 4,		 4 },
	{ PIRQ_UART3,	 3,		 3 },

	/* The MISC registers are not interrupt numbers */
	{ PIRQ_MISC,	0xfa,		0x00 },
	{ PIRQ_MISC0,	0x91,		0x00 },
	{ PIRQ_MISC1,	0x00,		0x00 },
	{ PIRQ_MISC2,	0x00,		0x00 },
};

const struct fch_irq_routing *mb_get_fch_irq_mapping(size_t *length)
{
	*length = ARRAY_SIZE(fch_irq_map);
	return fch_irq_map;
}

static void program_display_sel_gpios(void)
{
	int idx, port_type;
	gpio_t display_sel[MAINBOARD_SHARED_DDI_PORTS] = {GPIO_29, GPIO_31};

	for (idx = 0; idx < MAINBOARD_SHARED_DDI_PORTS; idx++) {
		port_type = get_ddi_port_conn_type(idx);

		if (port_type == HDMI)
			gpio_output(display_sel[idx], 0);
		else if (port_type == DP)
			gpio_output(display_sel[idx], 1);
	}

}

static void mainboard_init(void *chip_info)
{
	struct soc_amd_picasso_config *cfg = config_of_soc();

	if (!CONFIG(BILBY_LPC))
		cfg->emmc_config.timing = SD_EMMC_EMMC_HS400;

	mainboard_program_gpios();

	program_display_sel_gpios();

	/* Re-muxing LPCCLK0 can hang the system if LPC is in use. */
	if (CONFIG(BILBY_LPC))
		printk(BIOS_INFO, "eMMC not available due to LPC requirement\n");
	else
		mainboard_program_emmc_gpios();
}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
};
