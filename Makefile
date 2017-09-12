CC=avr-gcc
MCU=atmega328p
F_CPU=16000000UL

TARGET=freertos.hex

FREERTOS_DIR=miniAVRfreeRTOS

FREERTOS_SRC  :=  $(FREERTOS_DIR)/queue.c       \
                  $(FREERTOS_DIR)/tasks.c       \
                  $(FREERTOS_DIR)/list.c        \
                  $(FREERTOS_DIR)/hooks.c       \
                  $(FREERTOS_DIR)/port_tc.c

SOURCES :=        $(FREERTOS_SRC)               \
                  main.c                        \
                  pwm.c                         \
                  shell.c												\
									spi_bus_master.c							\
									tm1638.c											\
									bmp180.c											\
									bmp180_support.c						  \
                  i2c_bus_master.c              \
                  baro_reader.c                 \
                  adc.c                 				\
                  adc_reader.c           				\
									spi_test.c

V = 0
ifeq ($V, 0)
	Q = @
	P = > /dev/null
else
  Q =
  P = 
endif

AVRDUDE = avrdude

AVRDUDE_PROGRAMMER = arduino
AVRDUDE_PORT = /dev/tty.wchusbserial1410
#AVRDUDE_PORT = /dev/ttyUSB0

AVRDUDE_WRITE_FLASH = -U flash:w:$(TARGET)

AVRDUDE_VERBOSE = -v -v

AVRDUDE_FLAGS = -p $(MCU) -b 57600
AVRDUDE_FLAGS += -P $(AVRDUDE_PORT)
AVRDUDE_FLAGS += -c $(AVRDUDE_PROGRAMMER)

INC_PATH=-IminiAVRfreeRTOS -I./

CFLAGS = -mmcu=$(MCU)
CFLAGS += -funsigned-char -funsigned-bitfields -DDEBUG
CFLAGS += -DF_CPU=$(F_CPU) 
CFLAGS += $(INC_PATH)
CFLAGS += -O2 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums
CFLAGS += -mrelax -Wall -Wstrict-prototypes
CFLAGS += -std=gnu11 -Wundef
CFLAGS += -MMD -MP -MF .dep/$(@F).d
CFLAGS += -DBMP180_API

LFLAGS = -mmcu=$(MCU)
LFLAGS += -Wl,-Map=$(MAPFILE),--cref,--gc-sections
LFLAGS += -lm

OBJECTS := $(addprefix  obj/,$(SOURCES:.c=.o))
LINKED := $(addprefix obj/, $(TARGET:.hex=.elf))
BINARY := $(addprefix obj/, $(TARGET:.hex=.bin))
MAPFILE := $(addprefix obj/, $(TARGET:.hex=.map))
SYMFILE := $(addprefix obj/, $(TARGET:.hex=.sym))
LSSFILE := $(addprefix obj/, $(TARGET:.hex=.lss))

all: $(TARGET)


$(TARGET): $(OBJECTS)
	@echo "[LD]        $(TARGET)"
	$Qavr-gcc $(LFLAGS) -o $(LINKED) $^
	$Qavr-objcopy -O ihex -R .eeprom $(LINKED) $@
	$Qavr-objcopy -I ihex $(TARGET) -O binary $(BINARY)
	$Qavr-size --format=berkeley $(LINKED)
	$Qavr-nm -n $(LINKED) > $(SYMFILE)
	$Qavr-objdump -h -S $(LINKED) > $(LSSFILE)

obj/%.o: %.c
	@echo "[CC]        $(notdir $<)"
	$Qmkdir -p $(dir $@)
	$Q$(CC) $(CFLAGS) -c -o $@ $<

program: $(TARGET)
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_FLASH) $(AVRDUDE_WRITE_EEPROM)

clean:
	@echo "Cleaning $(TARGET)"
	$Qrm -f $(TARGET)
	$Qrm -f $(BINARY)
	$Qrm -f $(MAPFILE)
	$Qrm -f $(SYMFILE)
	$Qrm -f $(LSSFILE)
	@echo "Cleaning Objects"
	$Qrm -fR obj
	$Qrm -fR .dep

-include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)
