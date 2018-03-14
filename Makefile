include variables.mk


all: ccmsis cstm32_lib $(SOURCES) $(OBJECTS) ldall static_analysis
	make -C tests/
	$(SIZE) -B $(BIN).elf

ccmsis: $(CMSIS_SOURCES)
	$(CC) $(CFLAGS) $(CMSIS_SOURCES)

cstm32_lib: $(STM32_LIB_SOURCES)
	$(CC) $(CFLAGS) $(STM32_LIB_SOURCES)
	$(AS) $(ASFLAGS) ./stm32_lib/device_support/gcc/startup_stm32f10x_md_vl.S -o startup_stm32f10x_md_vl.o

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@
	# Prerequisites for using PVS-Studio
	$(CC) $(CFLAGS) $< -E -o $@.PVS-Studio.i
	pvs-studio --cfg $(PVS_CFG) --source-file $< \
		--i-file $@.PVS-Studio.i \
		--output-file $@.PVS-Studio.log

ldall:
	$(CC) $(CMSIS_OBJECTS) \
		$(STM32_LIB_OBJECTS) \
		$(OBJECTS) $(LDFLAGS)

static_analysis:
	plog-converter -a '' -t errorfile $(PVS_LOG_OBJECTS) -o $(PVS_LOG)
	cat $(PVS_LOG)


.PHONY: clean load

clean:
	rm -f 	$(OBJECTS) \
		$(CMSIS_OBJECTS) \
		$(STM32_LIB_OBJECTS) \
		$(PVS_I_OBJECTS) \
		$(PVS_LOG_OBJECTS) \
		$(BIN).map \
		$(BIN).elf
	make clean -C ./stlink/build/

load: $(BIN).elf
	./flashing_stm32vldiscovery.sh $(BIN).elf

