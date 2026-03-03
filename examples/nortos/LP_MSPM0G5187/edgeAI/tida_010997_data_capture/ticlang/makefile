MSPM0_SDK_INSTALL_DIR ?= $(abspath ../../../../../..)

include $(MSPM0_SDK_INSTALL_DIR)/imports.mak

CC = "$(TICLANG_ARMCOMPILER)/bin/tiarmclang"
LNK = "$(TICLANG_ARMCOMPILER)/bin/tiarmclang"

SYSCONFIG_GUI_TOOL = $(dir $(SYSCONFIG_TOOL))sysconfig_gui$(suffix $(SYSCONFIG_TOOL))
SYSCFG_CMD_STUB = $(SYSCONFIG_TOOL) --compiler ticlang --product $(MSPM0_SDK_INSTALL_DIR)/.metadata/product.json
SYSCFG_GUI_CMD_STUB = $(SYSCONFIG_GUI_TOOL) --compiler ticlang --product $(MSPM0_SDK_INSTALL_DIR)/.metadata/product.json
SYSCFG_FILES := $(shell $(SYSCFG_CMD_STUB) --listGeneratedFiles --listReferencedFiles --output . ../tida_010997_data_capture.syscfg)

SYSCFG_C_FILES = $(filter %.c,$(SYSCFG_FILES))
SYSCFG_H_FILES = $(filter %.h,$(SYSCFG_FILES))
SYSCFG_OPT_FILES = $(filter %.opt,$(SYSCFG_FILES))

# Enable verbose output by setting VERBOSE=1
V := @
ifeq ($(VERBOSE), 1)
  V :=
endif

OBJECTS = tida_010997_data_capture.obj device_agent_protocol_dap_interface.obj comm_i2c_comm.obj comm_spi_comm.obj sensors_sensor.obj sensors_ambient_light_sensor_OPT4001.obj sensors_hall_effect_sensor_TMAG5170.obj sensors_humidity_temperature_sensor_HDC3020.obj sensors_imu_sensor_BMI270.obj sensors_pressure_sensor_BMP384.obj ti_edgeAI_device_agent_protocol_core_dap_core.obj ti_edgeAI_device_agent_protocol_uart_comm_uart_comm_target.obj $(patsubst %.c,%.obj,$(notdir $(SYSCFG_C_FILES)))

NAME = tida_010997_data_capture


CFLAGS += -I.. \
    -I. \
    $(addprefix @,$(SYSCFG_OPT_FILES)) \
    -DDEVICE_NAME=MSPM0G5187 \
    @device.opt \
    "-I$(MSPM0_SDK_INSTALL_DIR)/source/third_party/CMSIS/Core/Include" \
    -Iexamples/edgeAI/tida_010997_data_capture/device_agent_protocol \
    -Iexamples/edgeAI/tida_010997_data_capture/comm \
    -Iexamples/edgeAI/tida_010997_data_capture/sensors \
    -Iexamples/edgeAI/tida_010997_data_capture/sensors/ambient_light \
    -Iexamples/edgeAI/tida_010997_data_capture/sensors/digital_mic \
    -Iexamples/edgeAI/tida_010997_data_capture/sensors/hall_effect \
    -Iexamples/edgeAI/tida_010997_data_capture/sensors/humidity_temperature \
    -Iexamples/edgeAI/tida_010997_data_capture/sensors/imu \
    -Iexamples/edgeAI/tida_010997_data_capture/sensors/pressure \
    -I../comm \
    -I../sensors \
    -I../sensors/hall_effect \
    -I../sensors/humidity_temperature \
    -I../sensors/imu \
    -I../sensors/pressure \
    -I../sensors/ambient_light \
    "-I$(MSPM0_SDK_INSTALL_DIR)/source/ti/edgeAI/device_agent_protocol/core" \
    "-I$(MSPM0_SDK_INSTALL_DIR)/source/ti/edgeAI/device_agent_protocol/uart_comm" \
    "-I$(MSPM0_SDK_INSTALL_DIR)/source" \
    -gdwarf-3 \
    -mcpu=cortex-m0plus \
    -march=thumbv6m \
    -mfloat-abi=soft \
    -mthumb \
    -Wall

LFLAGS += -ldevice.cmd.genlibs \
    "-L$(MSPM0_SDK_INSTALL_DIR)/source" \
    -L.. \
    ../ticlang/device_linker.cmd \
    "-Wl,-m,$(NAME).map" \
    -Wl,--rom_model \
    -Wl,--warn_sections \
    "-L$(TICLANG_ARMCOMPILER)/lib" \
    -llibc.a

all: $(NAME).out

.INTERMEDIATE: syscfg
$(SYSCFG_FILES): syscfg
	@ echo generation complete

syscfg: ../tida_010997_data_capture.syscfg
	@ echo Generating configuration files...
	$(V) $(SYSCFG_CMD_STUB) --output $(@D) $<


# Helpful hint that the user needs to use a standalone SysConfig installation
$(SYSCONFIG_GUI_TOOL):
	$(error $(dir $(SYSCONFIG_TOOL)) does not contain the GUI framework \
        necessary to launch the SysConfig GUI.  Please set SYSCONFIG_TOOL \
        (in your SDK's imports.mak) to a standalone SysConfig installation \
        rather than one inside CCS)

syscfg-gui: ../tida_010997_data_capture.syscfg $(SYSCONFIG_GUI_TOOL)
	@ echo Opening SysConfig GUI
	$(V) $(SYSCFG_GUI_CMD_STUB) $<


define C_RULE
$(basename $(notdir $(1))).obj: $(1) $(SYSCFG_H_FILES)
	@ echo Building $$@
	$(V) $(CC) $(CFLAGS) -c $$< -o $$@
endef

$(foreach c_file,$(SYSCFG_C_FILES),$(eval $(call C_RULE,$(c_file))))

tida_010997_data_capture.obj: ../tida_010997_data_capture.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

device_agent_protocol_dap_interface.obj: ../device_agent_protocol/dap_interface.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

comm_i2c_comm.obj: ../comm/i2c_comm.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

comm_spi_comm.obj: ../comm/spi_comm.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

sensors_sensor.obj: ../sensors/sensor.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

sensors_ambient_light_sensor_OPT4001.obj: ../sensors/ambient_light/sensor_OPT4001.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

sensors_hall_effect_sensor_TMAG5170.obj: ../sensors/hall_effect/sensor_TMAG5170.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

sensors_humidity_temperature_sensor_HDC3020.obj: ../sensors/humidity_temperature/sensor_HDC3020.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

sensors_imu_sensor_BMI270.obj: ../sensors/imu/sensor_BMI270.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

sensors_pressure_sensor_BMP384.obj: ../sensors/pressure/sensor_BMP384.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

ti_edgeAI_device_agent_protocol_core_dap_core.obj: /home/a0226989/.cache/bazel/_bazel_a0226989/4d845513c594c2a2405290acdd717718/sandbox/linux-sandbox/1845/execroot/__main__/source/ti/edgeAI/device_agent_protocol/core/dap_core.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

ti_edgeAI_device_agent_protocol_uart_comm_uart_comm_target.obj: /home/a0226989/.cache/bazel/_bazel_a0226989/4d845513c594c2a2405290acdd717718/sandbox/linux-sandbox/1845/execroot/__main__/source/ti/edgeAI/device_agent_protocol/uart_comm/uart_comm_target.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

$(NAME).out: $(OBJECTS)
	@ echo linking $@
	$(V) $(LNK) -Wl,-u,_c_int00 $(OBJECTS)  $(LFLAGS) -o $(NAME).out

clean:
	@ echo Cleaning...
	$(V) $(RM) $(OBJECTS) > $(DEVNULL) 2>&1
	$(V) $(RM) $(NAME).out > $(DEVNULL) 2>&1
	$(V) $(RM) $(NAME).map > $(DEVNULL) 2>&1
	$(V) $(RM) $(call SLASH_FIXUP,$(SYSCFG_FILES)) > $(DEVNULL) 2>&1
