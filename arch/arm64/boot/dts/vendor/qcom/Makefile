ifneq ($(CONFIG_ARCH_QTI_VM), y)
	ifeq ($(CONFIG_BUILD_ARM64_DT_OVERLAY),y)
		dtbo-$(CONFIG_ARCH_YUPIK) += \
			taoyao-sm7325-overlay.dtbo \
			yupik-idp-overlay.dtbo \
			yupik-idp-pm7250b-overlay.dtbo \
			yupikp-idp-overlay.dtbo \
			yupikp-atp-overlay.dtbo \
			yupikp-qrd-overlay.dtbo \
			yupikp-idps-amoled-overlay.dtbo \

		taoyao-sm7325-overlay.dtbo-base := yupik.dtb
		yupik-idp-overlay.dtbo-base := yupik.dtb
		yupik-idp-pm7250b-overlay.dtbo-base := yupik.dtb
		yupikp-idp-overlay.dtbo-base := yupikp.dtb
		yupikp-atp-overlay.dtbo-base := yupikp.dtb
		yupikp-qrd-overlay.dtbo-base := yupikp.dtb
		yupikp-idps-amoled-overlay.dtbo-base := yupikp.dtb
	else
		dtb-$(CONFIG_ARCH_YUPIK) += \
			yupik-rumi.dtb \
			yupik-idp.dtb \
			yupik-atp.dtb \
			yupik-qrd.dtb \
			yupik-idps-amoled.dtb \
			yupik-idp-pm7250b.dtb \
			yupikp-idp.dtb \
			yupikp-atp.dtb \
			yupikp-qrd.dtb \
			yupikp-idps-amoled.dtb \
			yupikp-idp-hsp.dtb \
			yupik-iot-idp.dtb \
			yupik-iot-qrd.dtb \
			yupik-iot-hsp.dtb \
			yupik-iot-idps-amoled.dtb \
			yupikp-iot-idp.dtb \
			yupikp-iot-qrd.dtb \
			yupikp-iot-hsp.dtb \
			yupikp-iot-idps-amoled.dtb
	endif
endif

always		:= $(dtb-y)
subdir-y	:= $(dts-dirs)
clean-files	:= *.dtb *.dtbo
