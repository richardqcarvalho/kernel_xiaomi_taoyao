#
# Makefile for the Synaptics TCM touchscreen driver.
#

# Each configuration option enables a list of files.
obj-$(CONFIG_TOUCHSCREEN_SYNAPTICS_TCM_SPI)	+= synaptics_tcm_spi.o
obj-$(CONFIG_TOUCHSCREEN_SYNAPTICS_TCM_CORE)	+= synaptics_tcm_core_module.o
synaptics_tcm_core_module-y := synaptics_tcm_core.o synaptics_tcm_touch.o
obj-$(CONFIG_TOUCHSCREEN_SYNAPTICS_TCM_REFLASH)	+= synaptics_tcm_reflash.o
obj-$(CONFIG_TOUCHSCREEN_SYNAPTICS_TCM_DEVICE)	+= synaptics_tcm_device.o
obj-$(CONFIG_TOUCHSCREEN_SYNAPTICS_TCM_TESTING) += synaptics_tcm_testing.o
obj-$(CONFIG_TOUCHSCREEN_SYNAPTICS_TCM_DIAGNOSTIC) += synaptics_tcm_diagnostics.o
