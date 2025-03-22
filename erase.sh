openocd -f interface/stlink.cfg -f target/stm32f3x.cfg -c "init" -c "halt" -c "stm32f3x mass_erase 0" -c "shutdown"
