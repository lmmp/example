deps_config := \
	/home/xxxx/ESP8266_RTOS_SDK/components/app_update/Kconfig \
	/home/xxxx/ESP8266_RTOS_SDK/components/aws_iot/Kconfig \
	/home/xxxx/ESP8266_RTOS_SDK/components/esp8266/Kconfig \
	/home/xxxx/ESP8266_RTOS_SDK/components/freertos/Kconfig \
	/home/xxxx/ESP8266_RTOS_SDK/components/libsodium/Kconfig \
	/home/xxxx/ESP8266_RTOS_SDK/components/log/Kconfig \
	/home/xxxx/ESP8266_RTOS_SDK/components/lwip/Kconfig \
	/home/xxxx/ESP8266_RTOS_SDK/components/mdns/Kconfig \
	/home/xxxx/ESP8266_RTOS_SDK/components/mqtt/Kconfig \
	/home/xxxx/ESP8266_RTOS_SDK/components/newlib/Kconfig \
	/home/xxxx/ESP8266_RTOS_SDK/components/pthread/Kconfig \
	/home/xxxx/ESP8266_RTOS_SDK/components/ssl/Kconfig \
	/home/xxxx/ESP8266_RTOS_SDK/components/tcpip_adapter/Kconfig \
	/home/xxxx/ESP8266_RTOS_SDK/components/wpa_supplicant/Kconfig \
	/home/xxxx/ESP8266_RTOS_SDK/components/bootloader/Kconfig.projbuild \
	/home/xxxx/ESP8266_RTOS_SDK/components/esptool_py/Kconfig.projbuild \
	/home/xxxx/ESP8266_RTOS_SDK/examples/wifi/simple_wifi/main/Kconfig.projbuild \
	/home/xxxx/ESP8266_RTOS_SDK/components/partition_table/Kconfig.projbuild \
	/home/xxxx/ESP8266_RTOS_SDK/Kconfig

include/config/auto.conf: \
	$(deps_config)


$(deps_config): ;
