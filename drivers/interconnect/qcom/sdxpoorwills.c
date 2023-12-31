// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2022-2023, Qualcomm Innovation Center, Inc. All rights reserved.
 *
 */

#include <asm/div64.h>
#include <dt-bindings/interconnect/qcom,sdxpoorwills.h>
#include <linux/device.h>
#include <linux/interconnect.h>
#include <linux/interconnect-provider.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/of_device.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/sort.h>

#include "icc-rpmh.h"
#include "qnoc-qos.h"

static const struct regmap_config icc_regmap_config = {
	.reg_bits = 32,
	.reg_stride = 4,
	.val_bits = 32,
};

static struct qcom_icc_node ipa_core_master = {
	.name = "ipa_core_master",
	.id = MASTER_IPA_CORE,
	.channels = 1,
	.buswidth = 8,
	.noc_ops = &qcom_qnoc4_ops,
	.num_links = 1,
	.links = { SLAVE_IPA_CORE },
};

static struct qcom_icc_node llcc_mc = {
	.name = "llcc_mc",
	.id = MASTER_LLCC,
	.channels = 1,
	.buswidth = 4,
	.noc_ops = &qcom_qnoc4_ops,
	.num_links = 1,
	.links = { SLAVE_EBI1 },
};

static struct qcom_icc_qosbox acm_tcu_qos = {
	.regs = icc_qnoc_qos_regs[ICC_QNOC_QOSGEN_TYPE_RPMH],
	.num_ports = 1,
	.offsets = { 0x10000 },
	.config = &(struct qos_config) {
		.prio = 6,
		.urg_fwd = 0,
	},
};

static struct qcom_icc_node acm_tcu = {
	.name = "acm_tcu",
	.id = MASTER_TCU_0,
	.channels = 1,
	.buswidth = 8,
	.noc_ops = &qcom_qnoc4_ops,
	.qosbox = &acm_tcu_qos,
	.num_links = 1,
	.links = { SLAVE_LLCC },
};

static struct qcom_icc_qosbox qnm_snoc_gc_qos = {
	.regs = icc_qnoc_qos_regs[ICC_QNOC_QOSGEN_TYPE_RPMH],
	.num_ports = 1,
	.offsets = { 0x18000 },
	.config = &(struct qos_config) {
		.prio = 0,
		.urg_fwd = 1,
	},
};

static struct qcom_icc_node qnm_snoc_gc = {
	.name = "qnm_snoc_gc",
	.id = MASTER_SNOC_GC_MEM_NOC,
	.channels = 1,
	.buswidth = 8,
	.noc_ops = &qcom_qnoc4_ops,
	.qosbox = &qnm_snoc_gc_qos,
	.num_links = 1,
	.links = { SLAVE_LLCC },
};

static struct qcom_icc_qosbox xm_apps_rdwr_qos = {
	.regs = icc_qnoc_qos_regs[ICC_QNOC_QOSGEN_TYPE_RPMH],
	.num_ports = 1,
	.offsets = { 0x13000 },
	.config = &(struct qos_config) {
		.prio = 0,
		.urg_fwd = 0,
	},
};

static struct qcom_icc_node xm_apps_rdwr = {
	.name = "xm_apps_rdwr",
	.id = MASTER_APPSS_PROC,
	.channels = 1,
	.buswidth = 16,
	.noc_ops = &qcom_qnoc4_ops,
	.qosbox = &xm_apps_rdwr_qos,
	.num_links = 2,
	.links = { SLAVE_LLCC, SLAVE_MEM_NOC_SNOC },
};

static struct qcom_icc_qosbox qhm_audio_qos = {
	.regs = icc_qnoc_qos_regs[ICC_QNOC_QOSGEN_TYPE_RPMH],
	.num_ports = 1,
	.offsets = { 0x14000 },
	.config = &(struct qos_config) {
		.prio = 0,
		.urg_fwd = 0,
	},
};

static struct qcom_icc_node qhm_audio = {
	.name = "qhm_audio",
	.id = MASTER_AUDIO,
	.channels = 1,
	.buswidth = 4,
	.noc_ops = &qcom_qnoc4_ops,
	.qosbox = &qhm_audio_qos,
	.num_links = 1,
	.links = { SLAVE_ANOC_SNOC },
};

static struct qcom_icc_qosbox qhm_blsp1_qos = {
	.regs = icc_qnoc_qos_regs[ICC_QNOC_QOSGEN_TYPE_RPMH],
	.num_ports = 1,
	.offsets = { 0x15000 },
	.config = &(struct qos_config) {
		.prio = 0,
		.urg_fwd = 0,
	},
};

static struct qcom_icc_node qhm_blsp1 = {
	.name = "qhm_blsp1",
	.id = MASTER_BLSP_1,
	.channels = 1,
	.buswidth = 4,
	.noc_ops = &qcom_qnoc4_ops,
	.qosbox = &qhm_blsp1_qos,
	.num_links = 1,
	.links = { SLAVE_ANOC_SNOC },
};

static struct qcom_icc_qosbox qhm_qdss_bam_qos = {
	.regs = icc_qnoc_qos_regs[ICC_QNOC_QOSGEN_TYPE_RPMH],
	.num_ports = 1,
	.offsets = { 0x16000 },
	.config = &(struct qos_config) {
		.prio = 0,
		.urg_fwd = 0,
	},
};

static struct qcom_icc_node qhm_qdss_bam = {
	.name = "qhm_qdss_bam",
	.id = MASTER_QDSS_BAM,
	.channels = 1,
	.buswidth = 4,
	.noc_ops = &qcom_qnoc4_ops,
	.qosbox = &qhm_qdss_bam_qos,
	.num_links = 21,
	.links = { SLAVE_AOP, SLAVE_AOSS,
		   SLAVE_AUDIO, SLAVE_BLSP_1,
		   SLAVE_CLK_CTL, SLAVE_CRYPTO_0_CFG,
		   SLAVE_IPA_CFG, SLAVE_PCIE_PARF,
		   SLAVE_PDM, SLAVE_PRNG,
		   SLAVE_QPIC, SLAVE_SDCC_1,
		   SLAVE_SNOC_CFG, SLAVE_SPMI_FETCHER,
		   SLAVE_TCSR, SLAVE_TLMM,
		   SLAVE_USB3, SLAVE_USB3_PHY_CFG,
		   SLAVE_SNOC_MEM_NOC_GC, SLAVE_IMEM,
		   SLAVE_TCU },
};

static struct qcom_icc_node qhm_qpic = {
	.name = "qhm_qpic",
	.id = MASTER_QPIC,
	.channels = 1,
	.buswidth = 4,
	.noc_ops = &qcom_qnoc4_ops,
	.num_links = 1,
	.links = { SLAVE_ANOC_SNOC },
};

static struct qcom_icc_node qhm_snoc_cfg = {
	.name = "qhm_snoc_cfg",
	.id = MASTER_SNOC_CFG,
	.channels = 1,
	.buswidth = 4,
	.noc_ops = &qcom_qnoc4_ops,
	.num_links = 1,
	.links = { SLAVE_SERVICE_SNOC },
};

static struct qcom_icc_node qhm_spmi_fetcher1 = {
	.name = "qhm_spmi_fetcher1",
	.id = MASTER_SPMI_FETCHER,
	.channels = 1,
	.buswidth = 4,
	.noc_ops = &qcom_qnoc4_ops,
	.num_links = 2,
	.links = { SLAVE_AOP, SLAVE_ANOC_SNOC },
};

static struct qcom_icc_node qnm_aggre_noc = {
	.name = "qnm_aggre_noc",
	.id = MASTER_ANOC_SNOC,
	.channels = 1,
	.buswidth = 8,
	.noc_ops = &qcom_qnoc4_ops,
	.num_links = 23,
	.links = { SLAVE_AOP, SLAVE_AOSS,
		   SLAVE_AUDIO, SLAVE_BLSP_1,
		   SLAVE_CLK_CTL, SLAVE_CRYPTO_0_CFG,
		   SLAVE_IPA_CFG, SLAVE_PCIE_PARF,
		   SLAVE_PDM, SLAVE_PRNG,
		   SLAVE_QPIC, SLAVE_SDCC_1,
		   SLAVE_SNOC_CFG, SLAVE_SPMI_FETCHER,
		   SLAVE_TCSR, SLAVE_TLMM,
		   SLAVE_USB3, SLAVE_USB3_PHY_CFG,
		   SLAVE_SNOC_MEM_NOC_GC, SLAVE_IMEM,
		   SLAVE_PCIE_0, SLAVE_QDSS_STM,
		   SLAVE_TCU },
};

static struct qcom_icc_node qnm_aggre_noc_ipa = {
	.name = "qnm_aggre_noc_ipa",
	.id = MASTER_ANOC_IPA,
	.channels = 1,
	.buswidth = 8,
	.noc_ops = &qcom_qnoc4_ops,
	.num_links = 22,
	.links = { SLAVE_AOP, SLAVE_AOSS,
		   SLAVE_AUDIO, SLAVE_BLSP_1,
		   SLAVE_CLK_CTL, SLAVE_CRYPTO_0_CFG,
		   SLAVE_IPA_CFG, SLAVE_PCIE_PARF,
		   SLAVE_PDM, SLAVE_PRNG,
		   SLAVE_QPIC, SLAVE_SDCC_1,
		   SLAVE_SNOC_CFG, SLAVE_SPMI_FETCHER,
		   SLAVE_TCSR, SLAVE_TLMM,
		   SLAVE_USB3, SLAVE_USB3_PHY_CFG,
		   SLAVE_SNOC_MEM_NOC_GC, SLAVE_IMEM,
		   SLAVE_QDSS_STM, SLAVE_TCU },
};

static struct qcom_icc_node qnm_memnoc = {
	.name = "qnm_memnoc",
	.id = MASTER_MEM_NOC_SNOC,
	.channels = 1,
	.buswidth = 8,
	.noc_ops = &qcom_qnoc4_ops,
	.num_links = 21,
	.links = { SLAVE_AOP, SLAVE_AOSS,
		   SLAVE_AUDIO, SLAVE_BLSP_1,
		   SLAVE_CLK_CTL, SLAVE_CRYPTO_0_CFG,
		   SLAVE_IPA_CFG, SLAVE_PCIE_PARF,
		   SLAVE_PDM, SLAVE_PRNG,
		   SLAVE_QPIC, SLAVE_SDCC_1,
		   SLAVE_SNOC_CFG, SLAVE_SPMI_FETCHER,
		   SLAVE_TCSR, SLAVE_TLMM,
		   SLAVE_USB3, SLAVE_USB3_PHY_CFG,
		   SLAVE_IMEM, SLAVE_QDSS_STM,
		   SLAVE_TCU },
};

static struct qcom_icc_qosbox qxm_crypto_qos = {
	.regs = icc_qnoc_qos_regs[ICC_QNOC_QOSGEN_TYPE_RPMH],
	.num_ports = 1,
	.offsets = { 0xc000 },
	.config = &(struct qos_config) {
		.prio = 0,
		.urg_fwd = 0,
	},
};

static struct qcom_icc_node qxm_crypto = {
	.name = "qxm_crypto",
	.id = MASTER_CRYPTO,
	.channels = 1,
	.buswidth = 8,
	.noc_ops = &qcom_qnoc4_ops,
	.qosbox = &qxm_crypto_qos,
	.num_links = 2,
	.links = { SLAVE_AOSS, SLAVE_ANOC_SNOC },
};

static struct qcom_icc_qosbox qxm_ipa_qos = {
	.regs = icc_qnoc_qos_regs[ICC_QNOC_QOSGEN_TYPE_RPMH],
	.num_ports = 1,
	.offsets = { 0x10000 },
	.config = &(struct qos_config) {
		.prio = 0,
		.urg_fwd = 1,
	},
};

static struct qcom_icc_node qxm_ipa = {
	.name = "qxm_ipa",
	.id = MASTER_IPA,
	.channels = 1,
	.buswidth = 8,
	.noc_ops = &qcom_qnoc4_ops,
	.qosbox = &qxm_ipa_qos,
	.num_links = 1,
	.links = { SLAVE_ANOC_IPA },
};

static struct qcom_icc_qosbox qxm_ipa2pcie_slv_qos = {
	.regs = icc_qnoc_qos_regs[ICC_QNOC_QOSGEN_TYPE_RPMH],
	.num_ports = 1,
	.offsets = { 0x11000 },
	.config = &(struct qos_config) {
		.prio = 0,
		.urg_fwd = 0,
	},
};

static struct qcom_icc_node qxm_ipa2pcie_slv = {
	.name = "qxm_ipa2pcie_slv",
	.id = MASTER_IPA_PCIE,
	.channels = 1,
	.buswidth = 8,
	.noc_ops = &qcom_qnoc4_ops,
	.qosbox = &qxm_ipa2pcie_slv_qos,
	.num_links = 1,
	.links = { SLAVE_PCIE_0 },
};

static struct qcom_icc_qosbox xm_emac_qos = {
	.regs = icc_qnoc_qos_regs[ICC_QNOC_QOSGEN_TYPE_RPMH],
	.num_ports = 1,
	.offsets = { 0x12000 },
	.config = &(struct qos_config) {
		.prio = 0,
		.urg_fwd = 0,
	},
};

static struct qcom_icc_node xm_emac = {
	.name = "xm_emac",
	.id = MASTER_EMAC,
	.channels = 1,
	.buswidth = 8,
	.noc_ops = &qcom_qnoc4_ops,
	.qosbox = &xm_emac_qos,
	.num_links = 1,
	.links = { SLAVE_ANOC_SNOC },
};

static struct qcom_icc_qosbox xm_pcie_qos = {
	.regs = icc_qnoc_qos_regs[ICC_QNOC_QOSGEN_TYPE_RPMH],
	.num_ports = 1,
	.offsets = { 0xd000 },
	.config = &(struct qos_config) {
		.prio = 0,
		.urg_fwd = 1,
	},
};

static struct qcom_icc_node xm_pcie = {
	.name = "xm_pcie",
	.id = MASTER_PCIE_0,
	.channels = 1,
	.buswidth = 8,
	.noc_ops = &qcom_qnoc4_ops,
	.qosbox = &xm_pcie_qos,
	.num_links = 1,
	.links = { SLAVE_ANOC_SNOC },
};

static struct qcom_icc_qosbox xm_qdss_etr_qos = {
	.regs = icc_qnoc_qos_regs[ICC_QNOC_QOSGEN_TYPE_RPMH],
	.num_ports = 1,
	.offsets = { 0xe000 },
	.config = &(struct qos_config) {
		.prio = 0,
		.urg_fwd = 0,
	},
};

static struct qcom_icc_node xm_qdss_etr = {
	.name = "xm_qdss_etr",
	.id = MASTER_QDSS_ETR,
	.channels = 1,
	.buswidth = 8,
	.noc_ops = &qcom_qnoc4_ops,
	.qosbox = &xm_qdss_etr_qos,
	.num_links = 21,
	.links = { SLAVE_AOP, SLAVE_AOSS,
		   SLAVE_AUDIO, SLAVE_BLSP_1,
		   SLAVE_CLK_CTL, SLAVE_CRYPTO_0_CFG,
		   SLAVE_IPA_CFG, SLAVE_PCIE_PARF,
		   SLAVE_PDM, SLAVE_PRNG,
		   SLAVE_QPIC, SLAVE_SDCC_1,
		   SLAVE_SNOC_CFG, SLAVE_SPMI_FETCHER,
		   SLAVE_TCSR, SLAVE_TLMM,
		   SLAVE_USB3, SLAVE_USB3_PHY_CFG,
		   SLAVE_SNOC_MEM_NOC_GC, SLAVE_IMEM,
		   SLAVE_TCU },
};

static struct qcom_icc_qosbox xm_sdc1_qos = {
	.regs = icc_qnoc_qos_regs[ICC_QNOC_QOSGEN_TYPE_RPMH],
	.num_ports = 1,
	.offsets = { 0x13000 },
	.config = &(struct qos_config) {
		.prio = 0,
		.urg_fwd = 0,
	},
};

static struct qcom_icc_node xm_sdc1 = {
	.name = "xm_sdc1",
	.id = MASTER_SDCC_1,
	.channels = 1,
	.buswidth = 8,
	.noc_ops = &qcom_qnoc4_ops,
	.qosbox = &xm_sdc1_qos,
	.num_links = 2,
	.links = { SLAVE_AOSS, SLAVE_ANOC_SNOC },
};

static struct qcom_icc_node xm_usb3 = {
	.name = "xm_usb3",
	.id = MASTER_USB3,
	.channels = 1,
	.buswidth = 8,
	.noc_ops = &qcom_qnoc4_ops,
	.num_links = 1,
	.links = { SLAVE_ANOC_SNOC },
};

static struct qcom_icc_node ipa_core_slave = {
	.name = "ipa_core_slave",
	.id = SLAVE_IPA_CORE,
	.channels = 1,
	.buswidth = 8,
	.noc_ops = &qcom_qnoc4_ops,
	.num_links = 0,
};

static struct qcom_icc_node ebi = {
	.name = "ebi",
	.id = SLAVE_EBI1,
	.channels = 1,
	.buswidth = 4,
	.noc_ops = &qcom_qnoc4_ops,
	.num_links = 0,
};

static struct qcom_icc_node qns_llcc = {
	.name = "qns_llcc",
	.id = SLAVE_LLCC,
	.channels = 1,
	.buswidth = 16,
	.noc_ops = &qcom_qnoc4_ops,
	.num_links = 1,
	.links = { MASTER_LLCC },
};

static struct qcom_icc_node qns_memnoc_snoc = {
	.name = "qns_memnoc_snoc",
	.id = SLAVE_MEM_NOC_SNOC,
	.channels = 1,
	.buswidth = 8,
	.noc_ops = &qcom_qnoc4_ops,
	.num_links = 1,
	.links = { MASTER_MEM_NOC_SNOC },
};

static struct qcom_icc_node qhs_aop = {
	.name = "qhs_aop",
	.id = SLAVE_AOP,
	.channels = 1,
	.buswidth = 4,
	.noc_ops = &qcom_qnoc4_ops,
	.num_links = 0,
};

static struct qcom_icc_node qhs_aoss = {
	.name = "qhs_aoss",
	.id = SLAVE_AOSS,
	.channels = 1,
	.buswidth = 4,
	.noc_ops = &qcom_qnoc4_ops,
	.num_links = 0,
};

static struct qcom_icc_node qhs_audio = {
	.name = "qhs_audio",
	.id = SLAVE_AUDIO,
	.channels = 1,
	.buswidth = 4,
	.noc_ops = &qcom_qnoc4_ops,
	.num_links = 0,
};

static struct qcom_icc_node qhs_blsp1 = {
	.name = "qhs_blsp1",
	.id = SLAVE_BLSP_1,
	.channels = 1,
	.buswidth = 4,
	.noc_ops = &qcom_qnoc4_ops,
	.num_links = 0,
};

static struct qcom_icc_node qhs_clk_ctl = {
	.name = "qhs_clk_ctl",
	.id = SLAVE_CLK_CTL,
	.channels = 1,
	.buswidth = 4,
	.noc_ops = &qcom_qnoc4_ops,
	.num_links = 0,
};

static struct qcom_icc_node qhs_crypto_cfg = {
	.name = "qhs_crypto_cfg",
	.id = SLAVE_CRYPTO_0_CFG,
	.channels = 1,
	.buswidth = 4,
	.noc_ops = &qcom_qnoc4_ops,
	.num_links = 0,
};

static struct qcom_icc_node qhs_ipa = {
	.name = "qhs_ipa",
	.id = SLAVE_IPA_CFG,
	.channels = 1,
	.buswidth = 4,
	.noc_ops = &qcom_qnoc4_ops,
	.num_links = 0,
};

static struct qcom_icc_node qhs_pcie_parf = {
	.name = "qhs_pcie_parf",
	.id = SLAVE_PCIE_PARF,
	.channels = 1,
	.buswidth = 4,
	.noc_ops = &qcom_qnoc4_ops,
	.num_links = 0,
};

static struct qcom_icc_node qhs_pdm = {
	.name = "qhs_pdm",
	.id = SLAVE_PDM,
	.channels = 1,
	.buswidth = 4,
	.noc_ops = &qcom_qnoc4_ops,
	.num_links = 0,
};

static struct qcom_icc_node qhs_prng = {
	.name = "qhs_prng",
	.id = SLAVE_PRNG,
	.channels = 1,
	.buswidth = 4,
	.noc_ops = &qcom_qnoc4_ops,
	.num_links = 0,
};

static struct qcom_icc_node qhs_qpic = {
	.name = "qhs_qpic",
	.id = SLAVE_QPIC,
	.channels = 1,
	.buswidth = 4,
	.noc_ops = &qcom_qnoc4_ops,
	.num_links = 0,
};

static struct qcom_icc_node qhs_sdc1 = {
	.name = "qhs_sdc1",
	.id = SLAVE_SDCC_1,
	.channels = 1,
	.buswidth = 4,
	.noc_ops = &qcom_qnoc4_ops,
	.num_links = 0,
};

static struct qcom_icc_node qhs_snoc_cfg = {
	.name = "qhs_snoc_cfg",
	.id = SLAVE_SNOC_CFG,
	.channels = 1,
	.buswidth = 4,
	.noc_ops = &qcom_qnoc4_ops,
	.num_links = 1,
	.links = { MASTER_SNOC_CFG },
};

static struct qcom_icc_node qhs_spmi_fetcher = {
	.name = "qhs_spmi_fetcher",
	.id = SLAVE_SPMI_FETCHER,
	.channels = 1,
	.buswidth = 4,
	.noc_ops = &qcom_qnoc4_ops,
	.num_links = 0,
};

static struct qcom_icc_node qhs_tcsr = {
	.name = "qhs_tcsr",
	.id = SLAVE_TCSR,
	.channels = 1,
	.buswidth = 4,
	.noc_ops = &qcom_qnoc4_ops,
	.num_links = 0,
};

static struct qcom_icc_node qhs_tlmm = {
	.name = "qhs_tlmm",
	.id = SLAVE_TLMM,
	.channels = 1,
	.buswidth = 4,
	.noc_ops = &qcom_qnoc4_ops,
	.num_links = 0,
};

static struct qcom_icc_node qhs_usb3 = {
	.name = "qhs_usb3",
	.id = SLAVE_USB3,
	.channels = 1,
	.buswidth = 4,
	.noc_ops = &qcom_qnoc4_ops,
	.num_links = 0,
};

static struct qcom_icc_node qhs_usb3_phy = {
	.name = "qhs_usb3_phy",
	.id = SLAVE_USB3_PHY_CFG,
	.channels = 1,
	.buswidth = 4,
	.noc_ops = &qcom_qnoc4_ops,
	.num_links = 0,
};

static struct qcom_icc_node qns_aggre_noc = {
	.name = "qns_aggre_noc",
	.id = SLAVE_ANOC_SNOC,
	.channels = 1,
	.buswidth = 8,
	.noc_ops = &qcom_qnoc4_ops,
	.num_links = 1,
	.links = { MASTER_ANOC_SNOC },
};

static struct qcom_icc_node qns_aggre_noc_ipa = {
	.name = "qns_aggre_noc_ipa",
	.id = SLAVE_ANOC_IPA,
	.channels = 1,
	.buswidth = 8,
	.noc_ops = &qcom_qnoc4_ops,
	.num_links = 1,
	.links = { MASTER_ANOC_IPA },
};

static struct qcom_icc_node qns_snoc_memnoc = {
	.name = "qns_snoc_memnoc",
	.id = SLAVE_SNOC_MEM_NOC_GC,
	.channels = 1,
	.buswidth = 8,
	.noc_ops = &qcom_qnoc4_ops,
	.num_links = 1,
	.links = { MASTER_SNOC_GC_MEM_NOC },
};

static struct qcom_icc_node qxs_imem = {
	.name = "qxs_imem",
	.id = SLAVE_IMEM,
	.channels = 1,
	.buswidth = 8,
	.noc_ops = &qcom_qnoc4_ops,
	.num_links = 0,
};

static struct qcom_icc_node qxs_pcie = {
	.name = "qxs_pcie",
	.id = SLAVE_PCIE_0,
	.channels = 1,
	.buswidth = 8,
	.noc_ops = &qcom_qnoc4_ops,
	.num_links = 0,
};

static struct qcom_icc_node srvc_snoc = {
	.name = "srvc_snoc",
	.id = SLAVE_SERVICE_SNOC,
	.channels = 1,
	.buswidth = 4,
	.noc_ops = &qcom_qnoc4_ops,
	.num_links = 0,
};

static struct qcom_icc_node xs_qdss_stm = {
	.name = "xs_qdss_stm",
	.id = SLAVE_QDSS_STM,
	.channels = 1,
	.buswidth = 4,
	.noc_ops = &qcom_qnoc4_ops,
	.num_links = 0,
};

static struct qcom_icc_node xs_sys_tcu_cfg = {
	.name = "xs_sys_tcu_cfg",
	.id = SLAVE_TCU,
	.channels = 1,
	.buswidth = 8,
	.noc_ops = &qcom_qnoc4_ops,
	.num_links = 0,
};

static struct qcom_icc_bcm bcm_acv = {
	.name = "ACV",
	.voter_idx = 0,
	.num_nodes = 1,
	.nodes = { &ebi },
};

static struct qcom_icc_bcm bcm_ce = {
	.name = "CE",
	.voter_idx = 0,
	.num_nodes = 1,
	.nodes = { &qxm_crypto },
};

static struct qcom_icc_bcm bcm_ip0 = {
	.name = "IP0",
	.voter_idx = 0,
	.num_nodes = 1,
	.nodes = { &ipa_core_slave },
};

static struct qcom_icc_bcm bcm_mc0 = {
	.name = "MC0",
	.voter_idx = 0,
	.keepalive = true,
	.num_nodes = 1,
	.nodes = { &ebi },
};

static struct qcom_icc_bcm bcm_pn0 = {
	.name = "PN0",
	.voter_idx = 0,
	.keepalive = true,
	.num_nodes = 18,
	.nodes = { &qhs_aop, &qhs_aoss,
		   &qhs_audio, &qhs_blsp1,
		   &qhs_clk_ctl, &qhs_crypto_cfg,
		   &qhs_ipa, &qhs_pcie_parf,
		   &qhs_pdm, &qhs_prng,
		   &qhs_qpic, &qhs_sdc1,
		   &qhs_snoc_cfg, &qhs_spmi_fetcher,
		   &qhs_tcsr, &qhs_tlmm,
		   &qhs_usb3, &qhs_usb3_phy },
};

static struct qcom_icc_bcm bcm_pn1 = {
	.name = "PN1",
	.voter_idx = 0,
	.num_nodes = 1,
	.nodes = { &xm_sdc1 },
};

static struct qcom_icc_bcm bcm_pn2 = {
	.name = "PN2",
	.voter_idx = 0,
	.num_nodes = 2,
	.nodes = { &qhm_audio, &qhm_spmi_fetcher1 },
};

static struct qcom_icc_bcm bcm_pn3 = {
	.name = "PN3",
	.voter_idx = 0,
	.num_nodes = 2,
	.nodes = { &qhm_blsp1, &qhm_qpic },
};

static struct qcom_icc_bcm bcm_pn5 = {
	.name = "PN5",
	.voter_idx = 0,
	.num_nodes = 1,
	.nodes = { &qxm_crypto },
};

static struct qcom_icc_bcm bcm_sh0 = {
	.name = "SH0",
	.voter_idx = 0,
	.keepalive = true,
	.num_nodes = 1,
	.nodes = { &qns_llcc },
};

static struct qcom_icc_bcm bcm_sh1 = {
	.name = "SH1",
	.voter_idx = 0,
	.num_nodes = 1,
	.nodes = { &acm_tcu },
};

static struct qcom_icc_bcm bcm_sh3 = {
	.name = "SH3",
	.voter_idx = 0,
	.num_nodes = 1,
	.nodes = { &xm_apps_rdwr },
};

static struct qcom_icc_bcm bcm_sh4 = {
	.name = "SH4",
	.voter_idx = 0,
	.num_nodes = 1,
	.nodes = { &qns_memnoc_snoc },
};

static struct qcom_icc_bcm bcm_sn0 = {
	.name = "SN0",
	.voter_idx = 0,
	.keepalive = true,
	.num_nodes = 1,
	.nodes = { &qns_snoc_memnoc },
};

static struct qcom_icc_bcm bcm_sn1 = {
	.name = "SN1",
	.voter_idx = 0,
	.num_nodes = 1,
	.nodes = { &qxs_imem },
};

static struct qcom_icc_bcm bcm_sn3 = {
	.name = "SN3",
	.voter_idx = 0,
	.num_nodes = 1,
	.nodes = { &xs_qdss_stm },
};

static struct qcom_icc_bcm bcm_sn6 = {
	.name = "SN6",
	.voter_idx = 0,
	.num_nodes = 1,
	.nodes = { &qxs_pcie },
};

static struct qcom_icc_bcm bcm_sn7 = {
	.name = "SN7",
	.voter_idx = 0,
	.num_nodes = 1,
	.nodes = { &qnm_aggre_noc },
};

static struct qcom_icc_bcm bcm_sn8 = {
	.name = "SN8",
	.voter_idx = 0,
	.num_nodes = 2,
	.nodes = { &qhm_qdss_bam, &xm_qdss_etr },
};

static struct qcom_icc_bcm bcm_sn9 = {
	.name = "SN9",
	.voter_idx = 0,
	.num_nodes = 1,
	.nodes = { &qnm_memnoc },
};

static struct qcom_icc_bcm bcm_sn11 = {
	.name = "SN11",
	.voter_idx = 0,
	.num_nodes = 1,
	.nodes = { &qxm_ipa },
};

static struct qcom_icc_bcm *ipa_virt_bcms[] = {
	&bcm_ip0,
};

static struct qcom_icc_node *ipa_virt_nodes[] = {
	[MASTER_IPA_CORE] = &ipa_core_master,
	[SLAVE_IPA_CORE] = &ipa_core_slave,
};

static char *ipa_virt_voters[] = {
	"hlos",
};

static struct qcom_icc_desc sdxpoorwills_ipa_virt = {
	.config = &icc_regmap_config,
	.nodes = ipa_virt_nodes,
	.num_nodes = ARRAY_SIZE(ipa_virt_nodes),
	.bcms = ipa_virt_bcms,
	.num_bcms = ARRAY_SIZE(ipa_virt_bcms),
	.voters = ipa_virt_voters,
	.num_voters = ARRAY_SIZE(ipa_virt_voters),
};

static struct qcom_icc_bcm *mc_virt_bcms[] = {
	&bcm_acv,
	&bcm_mc0,
};

static struct qcom_icc_node *mc_virt_nodes[] = {
	[MASTER_LLCC] = &llcc_mc,
	[SLAVE_EBI1] = &ebi,
};

static char *mc_virt_voters[] = {
	"hlos",
};

static struct qcom_icc_desc sdxpoorwills_mc_virt = {
	.config = &icc_regmap_config,
	.nodes = mc_virt_nodes,
	.num_nodes = ARRAY_SIZE(mc_virt_nodes),
	.bcms = mc_virt_bcms,
	.num_bcms = ARRAY_SIZE(mc_virt_bcms),
	.voters = mc_virt_voters,
	.num_voters = ARRAY_SIZE(mc_virt_voters),
};

static struct qcom_icc_bcm *mem_noc_bcms[] = {
	&bcm_sh0,
	&bcm_sh1,
	&bcm_sh3,
	&bcm_sh4,
};

static struct qcom_icc_node *mem_noc_nodes[] = {
	[MASTER_TCU_0] = &acm_tcu,
	[MASTER_SNOC_GC_MEM_NOC] = &qnm_snoc_gc,
	[MASTER_APPSS_PROC] = &xm_apps_rdwr,
	[SLAVE_LLCC] = &qns_llcc,
	[SLAVE_MEM_NOC_SNOC] = &qns_memnoc_snoc,
};

static char *mem_noc_voters[] = {
	"hlos",
};

static struct qcom_icc_desc sdxpoorwills_mem_noc = {
	.config = &icc_regmap_config,
	.nodes = mem_noc_nodes,
	.num_nodes = ARRAY_SIZE(mem_noc_nodes),
	.bcms = mem_noc_bcms,
	.num_bcms = ARRAY_SIZE(mem_noc_bcms),
	.voters = mem_noc_voters,
	.num_voters = ARRAY_SIZE(mem_noc_voters),
};

static struct qcom_icc_bcm *system_noc_bcms[] = {
	&bcm_ce,
	&bcm_pn0,
	&bcm_pn1,
	&bcm_pn2,
	&bcm_pn3,
	&bcm_pn5,
	&bcm_sn0,
	&bcm_sn1,
	&bcm_sn3,
	&bcm_sn6,
	&bcm_sn7,
	&bcm_sn8,
	&bcm_sn9,
	&bcm_sn11,
};

static struct qcom_icc_node *system_noc_nodes[] = {
	[MASTER_AUDIO] = &qhm_audio,
	[MASTER_BLSP_1] = &qhm_blsp1,
	[MASTER_QDSS_BAM] = &qhm_qdss_bam,
	[MASTER_QPIC] = &qhm_qpic,
	[MASTER_SNOC_CFG] = &qhm_snoc_cfg,
	[MASTER_SPMI_FETCHER] = &qhm_spmi_fetcher1,
	[MASTER_ANOC_SNOC] = &qnm_aggre_noc,
	[MASTER_ANOC_IPA] = &qnm_aggre_noc_ipa,
	[MASTER_MEM_NOC_SNOC] = &qnm_memnoc,
	[MASTER_CRYPTO] = &qxm_crypto,
	[MASTER_IPA] = &qxm_ipa,
	[MASTER_IPA_PCIE] = &qxm_ipa2pcie_slv,
	[MASTER_EMAC] = &xm_emac,
	[MASTER_PCIE_0] = &xm_pcie,
	[MASTER_QDSS_ETR] = &xm_qdss_etr,
	[MASTER_SDCC_1] = &xm_sdc1,
	[MASTER_USB3] = &xm_usb3,
	[SLAVE_AOP] = &qhs_aop,
	[SLAVE_AOSS] = &qhs_aoss,
	[SLAVE_AUDIO] = &qhs_audio,
	[SLAVE_BLSP_1] = &qhs_blsp1,
	[SLAVE_CLK_CTL] = &qhs_clk_ctl,
	[SLAVE_CRYPTO_0_CFG] = &qhs_crypto_cfg,
	[SLAVE_IPA_CFG] = &qhs_ipa,
	[SLAVE_PCIE_PARF] = &qhs_pcie_parf,
	[SLAVE_PDM] = &qhs_pdm,
	[SLAVE_PRNG] = &qhs_prng,
	[SLAVE_QPIC] = &qhs_qpic,
	[SLAVE_SDCC_1] = &qhs_sdc1,
	[SLAVE_SNOC_CFG] = &qhs_snoc_cfg,
	[SLAVE_SPMI_FETCHER] = &qhs_spmi_fetcher,
	[SLAVE_TCSR] = &qhs_tcsr,
	[SLAVE_TLMM] = &qhs_tlmm,
	[SLAVE_USB3] = &qhs_usb3,
	[SLAVE_USB3_PHY_CFG] = &qhs_usb3_phy,
	[SLAVE_ANOC_SNOC] = &qns_aggre_noc,
	[SLAVE_ANOC_IPA] = &qns_aggre_noc_ipa,
	[SLAVE_SNOC_MEM_NOC_GC] = &qns_snoc_memnoc,
	[SLAVE_IMEM] = &qxs_imem,
	[SLAVE_PCIE_0] = &qxs_pcie,
	[SLAVE_SERVICE_SNOC] = &srvc_snoc,
	[SLAVE_QDSS_STM] = &xs_qdss_stm,
	[SLAVE_TCU] = &xs_sys_tcu_cfg,
};

static char *system_noc_voters[] = {
	"hlos",
};

static struct qcom_icc_desc sdxpoorwills_system_noc = {
	.config = &icc_regmap_config,
	.nodes = system_noc_nodes,
	.num_nodes = ARRAY_SIZE(system_noc_nodes),
	.bcms = system_noc_bcms,
	.num_bcms = ARRAY_SIZE(system_noc_bcms),
	.voters = system_noc_voters,
	.num_voters = ARRAY_SIZE(system_noc_voters),
};

static int qnoc_probe(struct platform_device *pdev)
{
	int ret;

	ret = qcom_icc_rpmh_probe(pdev);
	if (ret)
		dev_err(&pdev->dev, "failed to register ICC provider\n");

	return ret;
}

static const struct of_device_id qnoc_of_match[] = {
	{ .compatible = "qcom,sdxpoorwills-ipa_virt",
	  .data = &sdxpoorwills_ipa_virt},
	{ .compatible = "qcom,sdxpoorwills-mc_virt",
	  .data = &sdxpoorwills_mc_virt},
	{ .compatible = "qcom,sdxpoorwills-mem_noc",
	  .data = &sdxpoorwills_mem_noc},
	{ .compatible = "qcom,sdxpoorwills-system_noc",
	  .data = &sdxpoorwills_system_noc},
	{ }
};
MODULE_DEVICE_TABLE(of, qnoc_of_match);

static struct platform_driver qnoc_driver = {
	.probe = qnoc_probe,
	.remove = qcom_icc_rpmh_remove,
	.driver = {
		.name = "qnoc-sdxpoorwills",
		.of_match_table = qnoc_of_match,
		.sync_state = qcom_icc_rpmh_sync_state,
	},
};

static int __init qnoc_driver_init(void)
{
	return platform_driver_register(&qnoc_driver);
}
core_initcall(qnoc_driver_init);

MODULE_DESCRIPTION("SDXPOORWILLS NoC driver");
MODULE_LICENSE("GPL v2");
