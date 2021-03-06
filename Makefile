#######################################
# user configuration:
#######################################

# git commit hash
GIT_COMMIT = $(shell git rev-parse --short HEAD)

# SOURCES: list of sources in the user application
SOURCES = $(wildcard Src/*.c)

# TARGET: name of the user application
PROJ_NAME = CANtact
TARGET = $(PROJ_NAME)-$(GIT_COMMIT)

# BUILD_DIR: directory to place output files in
BUILD_DIR = build

# LD_SCRIPT: location of the linker script
LD_SCRIPT = STM32F042C6_FLASH.ld

# USER_DEFS user defined macros
USER_DEFS = -D GIT_COMMIT=$(GIT_COMMIT)

# USB_INCLUDES: includes for the usb library
USB_INCLUDES = -IMiddlewares/ST/STM32_USB_Device_Library/Core/Inc
USB_INCLUDES += -IMiddlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc

# USER_CFLAGS: user C flags (enable warnings, enable debug info)
USER_CFLAGS = -Wall -g -ffunction-sections -fdata-sections -O3
# USER_LDFLAGS:  user LD flags
USER_LDFLAGS = -fno-exceptions -ffunction-sections -fdata-sections -Wl,--gc-sections

# TARGET_DEVICE: device to compile for
TARGET_DEVICE = STM32F042x6

#######################################
# end of user configuration
#######################################
#
#######################################
# binaries
#######################################
CC = arm-none-eabi-gcc
AR = arm-none-eabi-ar
RANLIB = arm-none-eabi-ranlib
SIZE = arm-none-eabi-size
OBJCOPY = arm-none-eabi-objcopy
MKDIR = mkdir -p
DOXYGEN = doxygen
#######################################

# Core and CPU type for Cortex M0
# ARM core type (CORE_M0, CORE_M3)
CORE = CORE_M0
CFLAGS += -mcpu=cortex-m0 -mthumb -mfloat-abi=soft

# where to build STM32Cube
CUBELIB_BUILD_DIR = $(BUILD_DIR)/STM32Cube

# various paths within the STmicro library
CMSIS_PATH = Drivers/CMSIS
CMSIS_DEVICE_PATH = $(CMSIS_PATH)/Device/ST/STM32F0xx
DRIVER_PATH = Drivers/STM32F0xx_HAL_Driver

# includes for gcc
INCLUDES = -I$(CMSIS_PATH)/Include
INCLUDES += -I$(CMSIS_DEVICE_PATH)/Include
INCLUDES += -I$(DRIVER_PATH)/Inc
INCLUDES += -IInc
INCLUDES += $(USB_INCLUDES)
INCLUDES += $(USER_INCLUDES)

# macros for gcc
DEFS = -D$(CORE) $(USER_DEFS) -D$(TARGET_DEVICE)

# Select target hardware
ifdef PLATFORM
ifneq ($(filter default DEFAULT cantact CANTACT CANtact,$(PLATFORM)),)
DEFS += -DPLATFORM_CANTACT
endif
ifneq ($(filter nucleo NUCLEO Nucleo,$(PLATFORM)),)
DEFS += -DPLATFORM_NUCLEO
endif
else
DEFS += -DPLATFORM_CANTACT
endif

# GCC compiler flags
CFLAGS += $(DEFS) $(INCLUDES)
CFLAGS += -std=gnu99
CFLAGS += $(USER_CFLAGS)
#CFLAGS += -fdiagnostic-color=auto

# default action: build the user application
all: $(BUILD_DIR)/$(TARGET).bin $(BUILD_DIR)/$(TARGET).hex

flash: all
	dfu-util -d 0483:df11 -c 1 -i 0 -a 0 -s 0x08000000:leave -D $(BUILD_DIR)/$(TARGET).bin

flash_stlink: all
	openocd -f "interface/stlink-v2-1.cfg" -f "target/stm32f0x.cfg" -c "init" -c "reset halt" -c "program $(BUILD_DIR)/$(TARGET).elf verify reset exit"

.PHONY: doc
doc:
	$(DOXYGEN)


#######################################
# build the st micro peripherial library
# (drivers and CMSIS)
#######################################

CUBELIB = $(CUBELIB_BUILD_DIR)/libstm32cube.a

# List of stm32 driver objects
CUBELIB_DRIVER_OBJS = $(addprefix $(CUBELIB_BUILD_DIR)/, $(patsubst %.c, %.o, $(notdir $(wildcard $(DRIVER_PATH)/Src/*.c))))

# shortcut for building core library (make cubelib)
cubelib: $(CUBELIB)

$(CUBELIB): $(CUBELIB_DRIVER_OBJS)
	$(AR) rv $@ $(CUBELIB_DRIVER_OBJS)
	$(RANLIB) $@

$(CUBELIB_BUILD_DIR)/%.o: $(DRIVER_PATH)/Src/%.c | $(CUBELIB_BUILD_DIR)
	$(CC) -c $(CFLAGS) -o $@ $^

$(CUBELIB_BUILD_DIR):
	$(MKDIR) $@

#######################################
# build the USB library
#######################################
USB_MIDDLEWARE_PATH = ./Middlewares/ST/STM32_USB_Device_Library/
USB_BUILD_DIR = $(BUILD_DIR)/usb
USB_SOURCES += usbd_ctlreq.c usbd_ioreq.c usbd_core.c usbd_cdc.c
# list of usb library objects
USB_OBJECTS += $(addprefix $(USB_BUILD_DIR)/,$(notdir $(USB_SOURCES:.c=.o)))

usb: $(USB_OBJECTS)

$(USB_BUILD_DIR)/%.o: $(USB_MIDDLEWARE_PATH)/Core/Src/%.c | $(USB_BUILD_DIR)
	$(CC) -Os $(CFLAGS) -c -o $@ $^

$(USB_BUILD_DIR)/%.o: $(USB_MIDDLEWARE_PATH)/Class/CDC/Src/%.c | $(USB_BUILD_DIR)
	$(CC) -Os $(CFLAGS) -c -o $@ $^

$(USB_BUILD_DIR):
	@echo $(USB_BUILD_DIR)
	$(MKDIR) $@

#######################################
# build the user application
#######################################

# list of user program objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(SOURCES:.c=.o)))
# add an object for the startup code
OBJECTS += $(BUILD_DIR)/startup_stm32f042x6.o

# use the periphlib core library, plus generic ones (libc, libm, libnosys)
LIBS = -lstm32cube -lc -lm -lnosys
LDFLAGS = -T $(LD_SCRIPT) -L $(CUBELIB_BUILD_DIR) -static $(LIBS) $(USER_LDFLAGS)

$(BUILD_DIR)/$(TARGET).hex: $(BUILD_DIR)/$(TARGET).elf
	$(OBJCOPY) -O ihex $(BUILD_DIR)/$(TARGET).elf $@

$(BUILD_DIR)/$(TARGET).bin: $(BUILD_DIR)/$(TARGET).elf
	$(OBJCOPY) -O binary $(BUILD_DIR)/$(TARGET).elf $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) $(USB_OBJECTS) $(CUBELIB)
	$(CC) -o $@ $(CFLAGS) $(OBJECTS) $(USB_OBJECTS) \
		$(LDFLAGS) -Xlinker \
		-Map=$(BUILD_DIR)/$(TARGET).map
	$(SIZE) $@
	cp $@ $(BUILD_DIR)/$(PROJ_NAME).elf

$(BUILD_DIR)/%.o: Src/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -Os -c -o $@ $^

$(BUILD_DIR)/%.o: Src/%.s | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $@ $^

$(BUILD_DIR):
	$(MKDIR) $@

# delete all user application files, keep the libraries
semiclean:
		rm -f $(BUILD_DIR)/*.o
		rm -f $(BUILD_DIR)/*.elf
		rm -f $(BUILD_DIR)/*.hex
		rm -f $(BUILD_DIR)/*.map
		rm -f $(BUILD_DIR)/*.bin

clean: semiclean
		rm -f $(USB_BUILD_DIR)/*.o
		rm -f $(CUBELIB_BUILD_DIR)/*.o

.PHONY: clean semiclean all
