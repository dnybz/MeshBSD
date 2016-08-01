/*
 * Copyright (c) 2014 Adrian Chadd <adrian@FreeBSD.org>.
 *  All rights reserved.
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
 * $FreeBSD: head/sys/mips/atheros/ar934x_nfcreg.h 263295 2014-03-18 12:18:35Z adrian $
 */
/*
 * Register definitions for the built-in NAND controller
 * of the Atheros AR934x and QCA955x SoCs.
 *
 * This file is based on the AR934x SoC driver from OpenWRT.
 *
 * Copyright (C) 2011-2013 Gabor Juhos <juhosg@openwrt.org>
 *
 * Used with permission.
 */
#ifndef	__AR934X_NFCREG_H__
#define	__AR934X_NFCREG_H__

#define	BIT(x)		(1 << (x))

#define AR934X_NFC_REG_CMD		0x00
#define AR934X_NFC_REG_CTRL		0x04
#define AR934X_NFC_REG_STATUS		0x08
#define AR934X_NFC_REG_INT_MASK		0x0c
#define AR934X_NFC_REG_INT_STATUS	0x10
#define AR934X_NFC_REG_ECC_CTRL		0x14
#define AR934X_NFC_REG_ECC_OFFSET	0x18
#define AR934X_NFC_REG_ADDR0_0		0x1c
#define AR934X_NFC_REG_ADDR0_1		0x24
#define AR934X_NFC_REG_ADDR1_0		0x20
#define AR934X_NFC_REG_ADDR1_1		0x28
#define AR934X_NFC_REG_SPARE_SIZE	0x30
#define AR934X_NFC_REG_PROTECT		0x38
#define AR934X_NFC_REG_LOOKUP_EN	0x40
#define AR934X_NFC_REG_LOOKUP(_x)	(0x44 + (_i) * 4)
#define AR934X_NFC_REG_DMA_ADDR		0x64
#define AR934X_NFC_REG_DMA_COUNT	0x68
#define AR934X_NFC_REG_DMA_CTRL		0x6c
#define AR934X_NFC_REG_MEM_CTRL		0x80
#define AR934X_NFC_REG_DATA_SIZE	0x84
#define AR934X_NFC_REG_READ_STATUS	0x88
#define AR934X_NFC_REG_TIME_SEQ		0x8c
#define AR934X_NFC_REG_TIMINGS_ASYN	0x90
#define AR934X_NFC_REG_TIMINGS_SYN	0x94
#define AR934X_NFC_REG_FIFO_DATA	0x98
#define AR934X_NFC_REG_TIME_MODE	0x9c
#define AR934X_NFC_REG_DMA_ADDR_OFFS	0xa0
#define AR934X_NFC_REG_FIFO_INIT	0xb0
#define AR934X_NFC_REG_GEN_SEQ_CTRL	0xb4

#define AR934X_NFC_CMD_CMD_SEQ_S		0
#define AR934X_NFC_CMD_CMD_SEQ_M		0x3f
#define   AR934X_NFC_CMD_SEQ_1C			0x00
#define   AR934X_NFC_CMD_SEQ_ERASE		0x0e
#define   AR934X_NFC_CMD_SEQ_12			0x0c
#define   AR934X_NFC_CMD_SEQ_1C1AXR		0x21
#define   AR934X_NFC_CMD_SEQ_S			0x24
#define   AR934X_NFC_CMD_SEQ_1C3AXR		0x27
#define   AR934X_NFC_CMD_SEQ_1C5A1CXR		0x2a
#define   AR934X_NFC_CMD_SEQ_18			0x32
#define AR934X_NFC_CMD_INPUT_SEL_SIU		0
#define AR934X_NFC_CMD_INPUT_SEL_DMA		BIT(6)
#define AR934X_NFC_CMD_ADDR_SEL_0		0
#define AR934X_NFC_CMD_ADDR_SEL_1		BIT(7)
#define AR934X_NFC_CMD_CMD0_S			8
#define AR934X_NFC_CMD_CMD0_M			0xff
#define AR934X_NFC_CMD_CMD1_S			16
#define AR934X_NFC_CMD_CMD1_M			0xff
#define AR934X_NFC_CMD_CMD2_S			24
#define AR934X_NFC_CMD_CMD2_M			0xff

#define AR934X_NFC_CTRL_ADDR_CYCLE0_M		0x7
#define AR934X_NFC_CTRL_ADDR_CYCLE0_S		0
#define AR934X_NFC_CTRL_SPARE_EN		BIT(3)
#define AR934X_NFC_CTRL_INT_EN			BIT(4)
#define AR934X_NFC_CTRL_ECC_EN			BIT(5)
#define AR934X_NFC_CTRL_BLOCK_SIZE_S		6
#define AR934X_NFC_CTRL_BLOCK_SIZE_M		0x3
#define   AR934X_NFC_CTRL_BLOCK_SIZE_32		0
#define   AR934X_NFC_CTRL_BLOCK_SIZE_64		1
#define   AR934X_NFC_CTRL_BLOCK_SIZE_128	2
#define   AR934X_NFC_CTRL_BLOCK_SIZE_256	3
#define AR934X_NFC_CTRL_PAGE_SIZE_S		8
#define AR934X_NFC_CTRL_PAGE_SIZE_M		0x7
#define   AR934X_NFC_CTRL_PAGE_SIZE_256		0
#define   AR934X_NFC_CTRL_PAGE_SIZE_512		1
#define   AR934X_NFC_CTRL_PAGE_SIZE_1024	2
#define   AR934X_NFC_CTRL_PAGE_SIZE_2048	3
#define   AR934X_NFC_CTRL_PAGE_SIZE_4096	4
#define   AR934X_NFC_CTRL_PAGE_SIZE_8192	5
#define   AR934X_NFC_CTRL_PAGE_SIZE_16384	6
#define AR934X_NFC_CTRL_CUSTOM_SIZE_EN		BIT(11)
#define AR934X_NFC_CTRL_IO_WIDTH_8BITS		0
#define AR934X_NFC_CTRL_IO_WIDTH_16BITS		BIT(12)
#define AR934X_NFC_CTRL_LOOKUP_EN		BIT(13)
#define AR934X_NFC_CTRL_PROT_EN			BIT(14)
#define AR934X_NFC_CTRL_WORK_MODE_ASYNC		0
#define AR934X_NFC_CTRL_WORK_MODE_SYNC		BIT(15)
#define AR934X_NFC_CTRL_ADDR0_AUTO_INC		BIT(16)
#define AR934X_NFC_CTRL_ADDR1_AUTO_INC		BIT(17)
#define AR934X_NFC_CTRL_ADDR_CYCLE1_M		0x7
#define AR934X_NFC_CTRL_ADDR_CYCLE1_S		18
#define AR934X_NFC_CTRL_SMALL_PAGE		BIT(21)

#define AR934X_NFC_DMA_CTRL_DMA_START		BIT(7)
#define AR934X_NFC_DMA_CTRL_DMA_DIR_WRITE	0
#define AR934X_NFC_DMA_CTRL_DMA_DIR_READ	BIT(6)
#define AR934X_NFC_DMA_CTRL_DMA_MODE_SG		BIT(5)
#define AR934X_NFC_DMA_CTRL_DMA_BURST_S		2
#define AR934X_NFC_DMA_CTRL_DMA_BURST_0		0
#define AR934X_NFC_DMA_CTRL_DMA_BURST_1		1
#define AR934X_NFC_DMA_CTRL_DMA_BURST_2		2
#define AR934X_NFC_DMA_CTRL_DMA_BURST_3		3
#define AR934X_NFC_DMA_CTRL_DMA_BURST_4		4
#define AR934X_NFC_DMA_CTRL_DMA_BURST_5		5
#define AR934X_NFC_DMA_CTRL_ERR_FLAG		BIT(1)
#define AR934X_NFC_DMA_CTRL_DMA_READY		BIT(0)

#define AR934X_NFC_INT_DEV_RDY(_x)		BIT(4 + (_x))
#define AR934X_NFC_INT_CMD_END			BIT(1)

#define AR934X_NFC_ECC_CTRL_ERR_THRES_S		8
#define AR934X_NFC_ECC_CTRL_ERR_THRES_M		0x1f
#define AR934X_NFC_ECC_CTRL_ECC_CAP_S		5
#define AR934X_NFC_ECC_CTRL_ECC_CAP_M		0x7
#define AR934X_NFC_ECC_CTRL_ECC_CAP_2		0
#define AR934X_NFC_ECC_CTRL_ECC_CAP_4		1
#define AR934X_NFC_ECC_CTRL_ECC_CAP_6		2
#define AR934X_NFC_ECC_CTRL_ECC_CAP_8		3
#define AR934X_NFC_ECC_CTRL_ECC_CAP_10		4
#define AR934X_NFC_ECC_CTRL_ECC_CAP_12		5
#define AR934X_NFC_ECC_CTRL_ECC_CAP_14		6
#define AR934X_NFC_ECC_CTRL_ECC_CAP_16		7
#define AR934X_NFC_ECC_CTRL_ERR_OVER		BIT(2)
#define AR934X_NFC_ECC_CTRL_ERR_UNCORRECT	BIT(1)
#define AR934X_NFC_ECC_CTRL_ERR_CORRECT		BIT(0)

#define AR934X_NFC_ECC_OFFS_OFSET_M		0xffff

/* default timing values */
#define AR934X_NFC_TIME_SEQ_DEFAULT	0x7fff
#define AR934X_NFC_TIMINGS_ASYN_DEFAULT	0x22
#define AR934X_NFC_TIMINGS_SYN_DEFAULT	0xf

#define AR934X_NFC_ID_BUF_SIZE		8
#define AR934X_NFC_DEV_READY_TIMEOUT	25 /* msecs */
#define AR934X_NFC_DMA_READY_TIMEOUT	25 /* msecs */
#define AR934X_NFC_DONE_TIMEOUT		1000
#define AR934X_NFC_DMA_RETRIES		20

#define AR934X_NFC_IRQ_MASK		AR934X_NFC_INT_DEV_RDY(0)

#define  AR934X_NFC_GENSEQ_SMALL_PAGE_READ	0x30043

#endif /* __AR934X_NFCREG_H__ */
