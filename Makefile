# Directories
ARMGCC_ROOT_DIR = C:/Users/Ryan/dev_tools/arm-none-eabi-gcc
ARMGCC_BIN_DIR = $(ARMGCC_ROOT_DIR)/bin
ARMGCC_INCLUDE_DIR = $(ARMGCC_ROOT_DIR)/include/stm32l476x
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
BIN_DIR = $(BUILD_DIR)/bin
OPENOCD_DIR = C:/Users/Ryan/dev_tools/xpack-openocd/xpack-openocd-0.12.0-7/openocd/scripts

LIB_DIRS = $(ARMGCC_INCLUDE_DIR)
INCLUDE_DIRS = $(ARMGCC_INCLUDE_DIR) \
	       ./src


# Toolchain
CC = $(ARMGCC_BIN_DIR)/arm-none-eabi-gcc.exe
RM = rm
CPPCHECK = cppcheck
FORMAT = clang-format

# Files
TARGET = $(BIN_DIR)/blink
STARTUP_SRC = $(ARMGCC_INCLUDE_DIR)/startup_stm32l476rgtx.s
STARTUP_OBJ = $(OBJ_DIR)/startup_stm32l476rgtx.o
LINKER = $(ARMGCC_INCLUDE_DIR)/STM32L476RGTX.ld

OPENOCD_IF = $(OPENOCD_DIR)/interface/stlink.cfg
OPENOCD_TGT = $(OPENOCD_DIR)/target/stm32l4x.cfg

SOURCES_WITH_HEADERS = \
	src/drivers/led.c \
	src/common/utils.c \
	
SOURCES = src/main.c \
	 $(SOURCES_WITH_HEADERS)

HEADERS = \
	  $(SOURCES_WITH_HEADERS:.c=.h) \


OBJECT_NAMES = $(SOURCES:.c=.o)
OBJECTS = $(patsubst %,$(OBJ_DIR)/%,$(OBJECT_NAMES)) $(STARTUP_OBJ)


# Flags
MCU = -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard 
WFLAGS = -Wall -Wextra -Werror -Wshadow 
CFLAGS = $(MCU) $(WFLAGS) $(addprefix -I, $(INCLUDE_DIRS)) -Og -g -DSTM32L476xx 
LDFLAGS = $(MCU) -T$(LINKER) -nostartfiles -nostdlib 
CPPCHECK_FLAGS = --quiet --enable=all --error-exitcode=1 --inline-suppr \
		 -DSTM32L476xx \
		 -D__GNUC__ \
		 --suppress=missingIncludeSystem \
		 --check-config
CPPCHECK_INCLUDES = $(addprefix -I, $(shell cygpath -u "$(ARMGCC_INCLUDE_DIR)") ./src)

# Build
## Linking
$(TARGET): $(OBJECTS) $(HEADERS)
	@mkdir -p $(dir $@)
	$(CC) $(LDFLAGS) $^ -o $@

## Compiling Object Files
$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $^ 

## Compiling Startup object
$(STARTUP_OBJ): $(STARTUP_SRC)
	$(CC) $(CFLAGS) -c -o $(STARTUP_OBJ) $(STARTUP_SRC) 

# Phonies
.PHONY: all clean flash cppcheck format

all: $(TARGET)

flash: $(TARGET)
	openocd -f "$(OPENOCD_IF)" -f "$(OPENOCD_TGT)" \
		-c "transport select swd" \
		-c "program $(TARGET) verify reset exit"

clean:
	@$(RM) -rf $(BUILD_DIR)

cppcheck:
	@$(CPPCHECK) \
		$(CPPCHECK_FLAGS) \
		$(CPPCHECK_INCLUDES) \
		$(SOURCES)

format:
	@$(FORMAT) -i $(SOURCES) $(HEADERS)
