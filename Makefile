ifndef PS5_PAYLOAD_SDK
$(error Set PS5_PAYLOAD_SDK to the extracted SDK directory)
endif
include $(PS5_PAYLOAD_SDK)/toolchain/prospero.mk

.PHONY: all
all: baidu-launcher.elf

build/assets.h: assets/param.json assets/icon0.png tools/embed.py
	python3 tools/embed.py

baidu-launcher.elf: src/main.c build/assets.h
	$(CC) -Os -Wall -Wextra -Werror -Ibuild -ffunction-sections -fdata-sections -Wl,--gc-sections -o $@ src/main.c -lSceAppInstUtil
	$(PS5_PAYLOAD_SDK)/bin/prospero-strip $@
