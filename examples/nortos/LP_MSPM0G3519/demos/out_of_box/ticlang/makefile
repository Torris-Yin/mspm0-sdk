MSPM0_SDK_INSTALL_DIR ?= $(abspath ../../../../../..)

include $(MSPM0_SDK_INSTALL_DIR)/imports.mak

CC = "$(TICLANG_ARMCOMPILER)/bin/tiarmclang"
LNK = "$(TICLANG_ARMCOMPILER)/bin/tiarmclang"

OBJECTS = blink_led.obj callbacks_mpack.obj function_generator.obj HAL_MSPM0G3519.obj main.obj temperature.obj ti_msp_dl_config.obj MSP_GUI_GUI_mpack.obj MSP_GUI_GUIComm_mpack.obj MSP_GUI_mpack.obj ticlang_startup_mspm0g351x_ticlang.obj
NAME = out_of_box

CFLAGS += -I.. \
    -D__MSPM0G3519__ \
    -O2 \
    "-I$(MSPM0_SDK_INSTALL_DIR)/source/third_party/CMSIS/Core/Include" \
    "-I$(MSPM0_SDK_INSTALL_DIR)/source" \
    -gdwarf-3 \
    -mcpu=cortex-m0plus \
    -march=thumbv6m \
    -mfloat-abi=soft \
    -mthumb

LFLAGS += "-l$(MSPM0_SDK_INSTALL_DIR)/source/ti/gui_composer/lib/ticlang/m0p/gui_composer.a" \
    "-l$(MSPM0_SDK_INSTALL_DIR)/source/ti/driverlib/lib/ticlang/m0p/mspm0gx51x/driverlib.a" \
    "-L$(MSPM0_SDK_INSTALL_DIR)/source" \
    -L.. \
    ../ticlang/mspm0g3519.cmd \
    "-Wl,-m,$(NAME).map" \
    -Wl,--rom_model \
    -Wl,--warn_sections \
    "-L$(TICLANG_ARMCOMPILER)/lib" \
    -llibc.a

all: $(NAME).out

blink_led.obj: ../blink_led.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< -o $@

callbacks_mpack.obj: ../callbacks_mpack.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< -o $@

function_generator.obj: ../function_generator.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< -o $@

HAL_MSPM0G3519.obj: ../HAL_MSPM0G3519.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< -o $@

main.obj: ../main.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< -o $@

temperature.obj: ../temperature.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< -o $@

ti_msp_dl_config.obj: ../ti_msp_dl_config.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< -o $@

MSP_GUI_GUI_mpack.obj: ../MSP_GUI/GUI_mpack.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< -o $@

MSP_GUI_GUIComm_mpack.obj: ../MSP_GUI/GUIComm_mpack.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< -o $@

MSP_GUI_mpack.obj: ../MSP_GUI/mpack.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< -o $@

ticlang_startup_mspm0g351x_ticlang.obj: ../ticlang/startup_mspm0g351x_ticlang.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< -o $@

$(NAME).out: $(OBJECTS)
	@ echo linking $@
	@ $(LNK) -Wl,-u,_c_int00 $(OBJECTS)  $(LFLAGS) -o $(NAME).out

clean:
	@ echo Cleaning...
	@ $(RM) $(OBJECTS) > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).out > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).map > $(DEVNULL) 2>&1
