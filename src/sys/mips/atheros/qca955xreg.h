/*-
 * Copyright (c) 2013 Adrian Chadd <adrian@FreeBSD.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $FreeBSD: head/sys/mips/atheros/qca955xreg.h 285083 2015-07-03 07:00:24Z adrian $
 */
#ifndef	__QCA955XREG_H__
#define	__QCA955XREG_H__

#define	BIT(x)          (1 << (x))

/* Revision ID information */
#define	REV_ID_MAJOR_QCA9556		0x0130
#define	REV_ID_MAJOR_QCA9558		0x1130
#define	QCA955X_REV_ID_REVISION_MASK	0xf

/* Big enough to cover APB and SPI, and most peripherals */
/*
 * it needs to cover SPI because right now the if_ath_ahb
 * code uses rman to map in the SPI address into memory
 * to read data instead of us squirreling it away at early
 * boot-time and using the firmware interface.
 *
 * if_ath_ahb.c should use the same firmware interface
 * that if_ath_pci.c uses.
 */
#define QCA955X_APB_BASE        0x18000000
#define QCA955X_APB_SIZE        0x08000000

#define	QCA955X_PCI_MEM_BASE0		0x10000000
#define	QCA955X_PCI_MEM_BASE1		0x12000000
#define	QCA955X_PCI_MEM_SIZE		0x02000000
#define	QCA955X_PCI_CFG_BASE0		0x14000000
#define	QCA955X_PCI_CFG_BASE1		0x16000000
#define	QCA955X_PCI_CFG_SIZE		0x1000
#define	QCA955X_PCI_CRP_BASE0		(AR71XX_APB_BASE + 0x000c0000)
#define	QCA955X_PCI_CRP_BASE1		(AR71XX_APB_BASE + 0x00250000)
#define	QCA955X_PCI_CRP_SIZE		0x1000
#define	QCA955X_PCI_CTRL_BASE0		(AR71XX_APB_BASE + 0x000f0000)
#define	QCA955X_PCI_CTRL_BASE1		(AR71XX_APB_BASE + 0x00280000)
#define	QCA955X_PCI_CTRL_SIZE		0x100

#define	QCA955X_WMAC_BASE		(AR71XX_APB_BASE + 0x00100000)
#define	QCA955X_WMAC_SIZE		0x20000
#define	QCA955X_EHCI0_BASE		0x1b000000
#define	QCA955X_EHCI1_BASE		0x1b400000
#define	QCA955X_EHCI_SIZE		0x1000

/* PLL block */

#define	QCA955X_PLL_CPU_CONFIG_REG		(AR71XX_PLL_CPU_BASE + 0x00)
#define	QCA955X_PLL_DDR_CONFIG_REG		(AR71XX_PLL_CPU_BASE + 0x04)
#define	QCA955X_PLL_CLK_CTRL_REG		(AR71XX_PLL_CPU_BASE + 0x08)

#define	QCA955X_PLL_CPU_CONFIG_NFRAC_SHIFT	0
#define	QCA955X_PLL_CPU_CONFIG_NFRAC_MASK	0x3f
#define	QCA955X_PLL_CPU_CONFIG_NINT_SHIFT	6
#define	QCA955X_PLL_CPU_CONFIG_NINT_MASK	0x3f
#define	QCA955X_PLL_CPU_CONFIG_REFDIV_SHIFT	12
#define	QCA955X_PLL_CPU_CONFIG_REFDIV_MASK	0x1f
#define	QCA955X_PLL_CPU_CONFIG_OUTDIV_SHIFT	19
#define	QCA955X_PLL_CPU_CONFIG_OUTDIV_MASK	0x3

#define	QCA955X_PLL_DDR_CONFIG_NFRAC_SHIFT	0
#define	QCA955X_PLL_DDR_CONFIG_NFRAC_MASK	0x3ff
#define	QCA955X_PLL_DDR_CONFIG_NINT_SHIFT	10
#define	QCA955X_PLL_DDR_CONFIG_NINT_MASK	0x3f
#define	QCA955X_PLL_DDR_CONFIG_REFDIV_SHIFT	16
#define	QCA955X_PLL_DDR_CONFIG_REFDIV_MASK	0x1f
#define	QCA955X_PLL_DDR_CONFIG_OUTDIV_SHIFT	23
#define	QCA955X_PLL_DDR_CONFIG_OUTDIV_MASK	0x7

#define	QCA955X_PLL_CLK_CTRL_CPU_PLL_BYPASS		BIT(2)
#define	QCA955X_PLL_CLK_CTRL_DDR_PLL_BYPASS		BIT(3)
#define	QCA955X_PLL_CLK_CTRL_AHB_PLL_BYPASS		BIT(4)
#define	QCA955X_PLL_CLK_CTRL_CPU_POST_DIV_SHIFT		5
#define	QCA955X_PLL_CLK_CTRL_CPU_POST_DIV_MASK		0x1f
#define	QCA955X_PLL_CLK_CTRL_DDR_POST_DIV_SHIFT		10
#define	QCA955X_PLL_CLK_CTRL_DDR_POST_DIV_MASK		0x1f
#define	QCA955X_PLL_CLK_CTRL_AHB_POST_DIV_SHIFT		15
#define	QCA955X_PLL_CLK_CTRL_AHB_POST_DIV_MASK		0x1f
#define	QCA955X_PLL_CLK_CTRL_CPUCLK_FROM_CPUPLL		BIT(20)
#define	QCA955X_PLL_CLK_CTRL_DDRCLK_FROM_DDRPLL		BIT(21)
#define	QCA955X_PLL_CLK_CTRL_AHBCLK_FROM_DDRPLL		BIT(24)

#define	QCA955X_PLL_ETH_XMII_CONTROL_REG		(AR71XX_PLL_CPU_BASE + 0x28)
#define	QCA955X_PLL_ETH_SGMII_CONTROL_REG		(AR71XX_PLL_CPU_BASE + 0x48)

/* Reset block */
#define	QCA955X_RESET_REG_RESET_MODULE		(AR71XX_RST_BLOCK_BASE + 0x1c)
#define	QCA955X_RESET_REG_BOOTSTRAP		(AR71XX_RST_BLOCK_BASE + 0xb0)
#define	QCA955X_RESET_REG_EXT_INT_STATUS	(AR71XX_RST_BLOCK_BASE + 0xac)

#define	QCA955X_BOOTSTRAP_REF_CLK_40		BIT(4)

#define	QCA955X_EXT_INT_WMAC_MISC		BIT(0)
#define	QCA955X_EXT_INT_WMAC_TX			BIT(1)
#define	QCA955X_EXT_INT_WMAC_RXLP		BIT(2)
#define	QCA955X_EXT_INT_WMAC_RXHP		BIT(3)
#define	QCA955X_EXT_INT_PCIE_RC1		BIT(4)
#define	QCA955X_EXT_INT_PCIE_RC1_INT0		BIT(5)
#define	QCA955X_EXT_INT_PCIE_RC1_INT1		BIT(6)
#define	QCA955X_EXT_INT_PCIE_RC1_INT2		BIT(7)
#define	QCA955X_EXT_INT_PCIE_RC1_INT3		BIT(8)
#define	QCA955X_EXT_INT_PCIE_RC2		BIT(12)
#define	QCA955X_EXT_INT_PCIE_RC2_INT0		BIT(13)
#define	QCA955X_EXT_INT_PCIE_RC2_INT1		BIT(14)
#define	QCA955X_EXT_INT_PCIE_RC2_INT2		BIT(15)
#define	QCA955X_EXT_INT_PCIE_RC2_INT3		BIT(16)
#define	QCA955X_EXT_INT_USB1			BIT(24)
#define	QCA955X_EXT_INT_USB2			BIT(28)

#define	QCA955X_EXT_INT_WMAC_ALL \
        (QCA955X_EXT_INT_WMAC_MISC | QCA955X_EXT_INT_WMAC_TX | \
         QCA955X_EXT_INT_WMAC_RXLP | QCA955X_EXT_INT_WMAC_RXHP)

#define	QCA955X_EXT_INT_PCIE_RC1_ALL \
        (QCA955X_EXT_INT_PCIE_RC1 | QCA955X_EXT_INT_PCIE_RC1_INT0 | \
         QCA955X_EXT_INT_PCIE_RC1_INT1 | QCA955X_EXT_INT_PCIE_RC1_INT2 | \
         QCA955X_EXT_INT_PCIE_RC1_INT3)

#define	QCA955X_EXT_INT_PCIE_RC2_ALL \
        (QCA955X_EXT_INT_PCIE_RC2 | QCA955X_EXT_INT_PCIE_RC2_INT0 | \
         QCA955X_EXT_INT_PCIE_RC2_INT1 | QCA955X_EXT_INT_PCIE_RC2_INT2 | \
         QCA955X_EXT_INT_PCIE_RC2_INT3)

#define	QCA955X_RESET_HOST		BIT(31)
#define	QCA955X_RESET_SLIC		BIT(30)
#define	QCA955X_RESET_HDMA		BIT(29)
#define	QCA955X_RESET_EXTERNAL		BIT(28)
#define	QCA955X_RESET_RTC		BIT(27)
#define	QCA955X_RESET_PCIE_EP_INT	BIT(26)
#define	QCA955X_RESET_CHKSUM_ACC	BIT(25)
#define	QCA955X_RESET_FULL_CHIP		BIT(24)
#define	QCA955X_RESET_GE1_MDIO		BIT(23)
#define	QCA955X_RESET_GE0_MDIO		BIT(22)
#define	QCA955X_RESET_CPU_NMI		BIT(21)
#define	QCA955X_RESET_CPU_COLD		BIT(20)
#define	QCA955X_RESET_HOST_RESET_INT	BIT(19)
#define	QCA955X_RESET_PCIE_EP		BIT(18)
#define	QCA955X_RESET_UART1		BIT(17)
#define	QCA955X_RESET_DDR		BIT(16)
#define	QCA955X_RESET_USB_PHY_PLL_PWD_EXT	BIT(15)
#define	QCA955X_RESET_NANDF		BIT(14)
#define	QCA955X_RESET_GE1_MAC		BIT(13)
#define	QCA955X_RESET_SGMII_ANALOG	BIT(12)
#define	QCA955X_RESET_USB_PHY_ANALOG	BIT(11)
#define	QCA955X_RESET_HOST_DMA_INT	BIT(10)
#define	QCA955X_RESET_GE0_MAC		BIT(9)
#define	QCA955X_RESET_SGMII		BIT(8)
#define	QCA955X_RESET_PCIE_PHY		BIT(7)
#define	QCA955X_RESET_PCIE		BIT(6)
#define	QCA955X_RESET_USB_HOST		BIT(5)
#define	QCA955X_RESET_USB_PHY		BIT(4)
#define	QCA955X_RESET_USBSUS_OVERRIDE	BIT(3)
#define	QCA955X_RESET_LUT		BIT(2)
#define	QCA955X_RESET_MBOX		BIT(1)
#define	QCA955X_RESET_I2S		BIT(0)

/* GPIO block */
#define	QCA955X_GPIO_REG_OUT_FUNC0	0x2c
#define	QCA955X_GPIO_REG_OUT_FUNC1	0x30
#define	QCA955X_GPIO_REG_OUT_FUNC2	0x34
#define	QCA955X_GPIO_REG_OUT_FUNC3	0x38
#define	QCA955X_GPIO_REG_OUT_FUNC4	0x3c
#define	QCA955X_GPIO_REG_OUT_FUNC5	0x40
#define	QCA955X_GPIO_REG_FUNC		0x6c
#define	QCA955X_GPIO_COUNT		24

#define	QCA955X_GMAC_BASE	(AR71XX_APB_BASE + 0x00070000)
#define	QCA955X_GMAC_SIZE	0x40
#define	QCA955X_NFC_BASE	0x1b800200
#define	QCA955X_NFC_SIZE	0xb8


/* GMAC Interface */
#define	QCA955X_GMAC_REG_ETH_CFG	(QCA955X_GMAC_BASE + 0x00)

#define	QCA955X_ETH_CFG_RGMII_EN	BIT(0)
#define	QCA955X_ETH_CFG_GE0_SGMII	BIT(6)

/* XXX Same as AR934x values */
#define	QCA955X_PLL_VAL_1000	0x16000000
#define	QCA955X_PLL_VAL_100	0x00000101
#define	QCA955X_PLL_VAL_10	0x00001616

/* DDR block */
#define	QCA955X_DDR_REG_FLUSH_GE0	(AR71XX_APB_BASE + 0x9c)
#define	QCA955X_DDR_REG_FLUSH_GE1	(AR71XX_APB_BASE + 0xa0)
#define	QCA955X_DDR_REG_FLUSH_USB	(AR71XX_APB_BASE + 0xa4)
#define	QCA955X_DDR_REG_FLUSH_PCIE	(AR71XX_APB_BASE + 0xa8)
#define	QCA955X_DDR_REG_FLUSH_WMAC	(AR71XX_APB_BASE + 0xac)
/* PCIe EP */
#define	QCA955X_DDR_REG_FLUSH_SRC1	(AR71XX_APB_BASE + 0xb0)
/* checksum engine */
#define	QCA955X_DDR_REG_FLUSH_SRC2	(AR71XX_APB_BASE + 0xb2)

/* PCIe control block - relative to PCI_CTRL_BASE0/PCI_CTRL_BASE1 */

#define	QCA955X_PCI_APP                  0x0
#define	QCA955X_PCI_APP_LTSSM_ENABLE     (1 << 0)
#define	QCA955X_PCI_RESET                0x18
#define	QCA955X_PCI_RESET_LINK_UP        (1 << 0)
#define	QCA955X_PCI_INTR_STATUS          0x4c
#define	QCA955X_PCI_INTR_MASK            0x50
#define	QCA955X_PCI_INTR_DEV0            (1 << 14)

#endif	/* __QCA955XREG_H__ */
