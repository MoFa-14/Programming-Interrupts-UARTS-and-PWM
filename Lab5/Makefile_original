# name of executable - change this to change the executables name

ELF=demo.elf

# object files

OBJS= $(STARTUP)  $(HAL_OBJS)  main.o

HAL_OBJS= stm32l4xx_hal.o stm32l4xx_hal_rcc.o stm32l4xx_hal_rcc_ex.o stm32l4xx_hal_cortex.o stm32l4xx_hal_pwr.o stm32l4xx_hal_pwr_ex.o stm32l4xx_hal_gpio.o \
stm32l4xx_hal_dma.o stm32l4xx_hal_uart.o stm32l4xx_hal_uart_ex.o stm32l4xx_hal_exti.o


# Tool path

TOOLROOT		=/opt/gcc-arm-none-eabi/bin

# Library path

HAL			= ../Drivers/STM32L4xx_HAL_Driver
CMSIS			= ../Drivers/CMSIS/Core
DEVICE			= ../Drivers/CMSIS/Device/ST/STM32L4xx/Include
Common_BSP		= ../Drivers/BSP/Components/Common
APP			= .
NEWLIB 			= ../../newlib-nano/arm-none-eabi/newlib/
NEWLIBINCLUDE 		= ../../newlib-cygwin/newlib/libc/include/


# Tools

CC=$(TOOLROOT)/arm-none-eabi-gcc
LD=$(TOOLROOT)/arm-none-eabi-gcc
AR=$(TOOLROOT)/arm-none-eabi-ar
AS=$(TOOLROOT)/arm-none-eabi-as
OBJCOPY=$(TOOLROOT)/arm-none-eabi-objcopy

# Code paths
APP_CODE=$(APP)/Src

# Search path for perpheral library

VPATH=$(HAL)/Src:$(APP_CODE)

vpath %.c $(APP_CODE)
vpath %.s $(APP_CODE)
vpath %.c $(HAL)/Src


#  Processor specific

PTYPE = STM32L475xx
LDSCRIPT = STM32L475VGTx_FLASH.ld
STARTUP=  startup_stm32l475xx.o system_stm32l4xx.o stm32l4xx_hal_msp.o stm32l4xx_it.o syscalls.o 
BOARD=USE_HAL_DRIVER
BOARDTYPE=USE_STM32L475E_IOT01

# compilation flags for gdb

CFLAGS  = -O0 -g

# Compilation Flags

FULLASSERT = -DUSE_FULL_ASSERT 

LDFLAGS+= -T$(LDSCRIPT) -mthumb -mcpu=cortex-m4 -nostdlib  -L$(NEWLIB)
CFLAGS+= -mcpu=cortex-m4 -mthumb -D$(BOARD) -D$(BOARDTYPE) -D$(PTYPE)
CFLAGS+= -I$(APP)/Inc -I$(HAL)/Inc -I$(CMSIS)/Include -I$(Common_BSP)  -I$(DEVICE) -I$(NEWLIBINCLUDE)
CFLAGS+=  -DUSE_USB_OTG_FS  -D$(BOARD) -DUSE_STDPERIPH_DRIVER $(FULLASSERT)

# Build executable 

$(ELF) : $(OBJS)
	$(LD) $(LDFLAGS) -o $@ -lc -lm $(OBJS) -lc -lm -lgcc

# compile and generate dependency info

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

%.o: %.s
	$(CC) -c $(CFLAGS) $< -o $@

%.hex: %.elf
	$(OBJCOPY) -Oihex $*.elf $*.hex

%.flash: %.hex
	@echo FLASH $<
	@openocd -f board/stm32l4discovery.cfg \
		-c "init" \
		-c "reset init" \
		-c "stm32l4x mass_erase 0" \
		-c "flash write_image $(*).hex" \
		-c "reset" \
		-c "shutdown" $(NULL)
	@touch $@
clean:
	rm -f $(OBJS) $(OBJS:.o=.d) $(ELF) $(ELF).hx $(ELF).flash 

debug: $(ELF)
	multiarch-gdb $(ELF)


# pull in dependencies

-include $(OBJS:.o=.d)




