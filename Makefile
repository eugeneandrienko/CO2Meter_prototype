include variables.mk


all: ccmsis cstm32_lib cc ldall
	make -C tests/
	$(SIZE) -B $(BIN).elf

ccmsis: $(CMSISSRC)
	$(CC) $(CFLAGS) $(CMSISSRC)

cstm32_lib: $(STM32_LIBSRC)
	$(CC) $(CFLAGS) $(STM32_LIBSRC)
	$(AS) $(ASFLAGS) ./stm32_lib/device_support/gcc/startup_stm32f10x_md_vl.S -o startup_stm32f10x_md_vl.o

cc: $(SRC)
	$(CC) $(CFLAGS) $(SRC)

ldall:
	$(CC) $(OBJ) $(LDFLAGS)

.PHONY: clean load

clean:
	rm -f 	$(OBJ) \
		$(BIN).map \
		$(BIN).elf
	make clean -C ./stlink/build/

load: $(BIN).elf
	./flashing_stm32vldiscovery.sh $(BIN).elf

