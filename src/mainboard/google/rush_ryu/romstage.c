/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <soc/addressmap.h>
#include <soc/clock.h>
#include <soc/funitcfg.h>
#include <soc/padconfig.h>
#include <soc/nvidia/tegra/i2c.h>
#include <soc/romstage.h>

static const struct pad_config padcfgs[] = {
	/* AP_SYS_RESET_L */
	PAD_CFG_GPIO_OUT1(GPIO_PI5, PINMUX_PULL_UP),
	/* WP_L */
	PAD_CFG_GPIO_INPUT(KB_ROW1, PINMUX_PULL_NONE),
};

static const struct pad_config tpm_pads[] = {
	PAD_CFG_SFIO(CAM_I2C_SCL, PINMUX_INPUT_ENABLE, I2C3),
	PAD_CFG_SFIO(CAM_I2C_SDA, PINMUX_INPUT_ENABLE, I2C3),
};

static const struct pad_config ec_i2c_pads[] = {
	PAD_CFG_SFIO(GEN2_I2C_SCL, PINMUX_OPEN_DRAIN|PINMUX_INPUT_ENABLE, I2C2),
	PAD_CFG_SFIO(GEN2_I2C_SDA, PINMUX_OPEN_DRAIN|PINMUX_INPUT_ENABLE, I2C2),
};

static const struct funit_cfg funits[] = {
	/* TPM on I2C3  @ 400kHz */
	FUNIT_CFG(I2C3, PLLP, 400, tpm_pads, ARRAY_SIZE(tpm_pads)),
	/* EC on I2C2 - pulled to 3.3V @ 100kHz */
	FUNIT_CFG(I2C2, PLLP, 100, ec_i2c_pads, ARRAY_SIZE(ec_i2c_pads)),
	/* I2C6 for audio, temp sensor, etc. */
	FUNIT_CFG(I2C6, PLLP, 400, NULL, 0),
};

void romstage_mainboard_init(void)
{
	/* Bring up controller interfaces for ramstage loading. */
	soc_configure_funits(funits, ARRAY_SIZE(funits));
	soc_configure_pads(padcfgs, ARRAY_SIZE(padcfgs));
	soc_configure_i2c6pad();

	/* TPM */
	i2c_init(I2C3_BUS);
	/* EC */
	i2c_init(I2C2_BUS);
	/* I2C6 bus (audio, etc.) */
	i2c_init(I2C6_BUS);
}

void mainboard_configure_pmc(void)
{
}

void mainboard_enable_vdd_cpu(void)
{
	/* VDD_CPU is already enabled in bootblock. */
}
