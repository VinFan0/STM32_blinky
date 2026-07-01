# Directories
TOOLS_DIR = ${TOOLS_PATH}
ARMGCC_ROOT_DIR = $(TOOLS_DIR)/arm-none-eabi-gcc
ARMGCC_BIN_DIR = $(ARMGCC_ROOT_DIR)/bin
ARMGCC_INCLUDE_DIR = $(ARMGCC_ROOT_DIR)/include/stm32l476x
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
BIN_DIR = $(BUILD_DIR)/bin
OPENOCD_DIR = $(TOOLS_DIR)/openocd/scripts
FREERTOS_DIR = external/freeRTOS/FreeRTOS/Source
FREERTOS_PORT_DIR = $(FREERTOS_DIR)/portable/GCC/ARM_CM4F
FREERTOS_HEAP_DIR = $(FREERTOS_DIR)/portable/MemMang

LIB_DIRS = $(ARMGCC_INCLUDE_DIR)
INCLUDE_DIRS = $(ARMGCC_INCLUDE_DIR) \
	       $(FREERTOS_DIR)/include \
	       $(FREERTOS_PORT_DIR) \
	       ./src/common \
	       ./src


# Toolchain
CC = $(ARMGCC_BIN_DIR)/arm-none-eabi-gcc
RM = rm
CPPCHECK = cppcheck
FORMAT = clang-format
OOCD = $(OPENOCD_DIR)/../bin/openocd

# Files
TARGET = $(BIN_DIR)/blink
STARTUP_SRC = $(ARMGCC_INCLUDE_DIR)/startup_stm32l476rgtx.s
STARTUP_OBJ = $(OBJ_DIR)/startup_stm32l476rgtx.o
LINKER = $(ARMGCC_INCLUDE_DIR)/STM32L476RGTx.ld

OPENOCD_IF = $(OPENOCD_DIR)/interface/stlink.cfg
OPENOCD_TGT = $(OPENOCD_DIR)/target/stm32l4x.cfg

FREERTOS_SOURCES = \
		   $(FREERTOS_DIR)/tasks.c \
		   $(FREERTOS_DIR)/queue.c \
		   $(FREERTOS_DIR)/list.c \
		   $(FREERTOS_DIR)/timers.c \
		   $(FREERTOS_PORT_DIR)/port.c \
		   $(FREERTOS_HEAP_DIR)/heap_4.c

SOURCES_WITH_HEADERS = \
	src/drivers/led.c \
	src/common/utils.c \
	src/drivers/io.c \
	src/drivers/uart.c \
	src/drivers/lcd.c \
	src/drivers/spi1.c \
	src/drivers/oled.c
	
SOURCES = src/main.c \
	 $(SOURCES_WITH_HEADERS) \
	 $(FREERTOS_SOURCES)

HEADERS = \
	  $(SOURCES_WITH_HEADERS:.c=.h) 


OBJECT_NAMES = $(SOURCES:.c=.o)
OBJECTS = $(patsubst %,$(OBJ_DIR)/%,$(OBJECT_NAMES)) $(STARTUP_OBJ)


# Flags
MCU = -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard 
WFLAGS = -Wall -Wextra -Werror -Wshadow 
DEPFLAGS = -MMD -MP
CFLAGS = $(MCU) $(WFLAGS) $(addprefix -I, $(INCLUDE_DIRS)) -Og -g -DSTM32L476xx $(DEPFLAGS)
LDFLAGS = $(MCU) -T$(LINKER) -nostartfiles -lgcc -lc
CPPCHECK_FLAGS = --quiet --enable=all --error-exitcode=1 --inline-suppr \
		 -DSTM32L476xx \
		 -D__GNUC__ \
		 --suppress=missingIncludeSystem \
		 --check-config

DEPS = $(OBJECTS:.o=.d)
-include $(DEPS)	

CPPCHECK_INCLUDES = $(addprefix -I, \
		    $(ARMGCC_INCLUDE_DIR) \
		    ./src \
		    ./src/common \
		    $(FREERTOS_DIR)/include \
		    $(FREERTOS_PORT_DIR))


# Build
## Linking
$(TARGET): $(OBJECTS) $(HEADERS)
	@mkdir -p $(dir $@)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

## Compiling Object Files
$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

## Compiling Startup object
$(STARTUP_OBJ): $(STARTUP_SRC)
	$(CC) $(CFLAGS) -c -o $(STARTUP_OBJ) $(STARTUP_SRC) 

# Phonies
.PHONY: all clean flash cppcheck format

all: $(TARGET)

flash: $(TARGET)
	$(OOCD) -s "$(OPENOCD_DIR)" -f "$(OPENOCD_IF)" -f "$(OPENOCD_TGT)" \
		-c "program $(TARGET) verify reset exit"

clean:
	@$(RM) -rf $(BUILD_DIR)

cppcheck:
	@$(CPPCHECK) \
		$(CPPCHECK_FLAGS) \
		$(CPPCHECK_INCLUDES) \
		src/main.c \
		$(SOURCES_WITH_HEADERS)

format:
	@$(FORMAT) -i $(SOURCES) $(HEADERS)

