# make -j6 HARDWARE_ID=101

make -j6 HARDWARE_ID=102

# openocd -f interface/stlink.cfg -f target/stm32f3x.cfg -c "init" -c "halt" -c "program build/RM2024-SuperCap-F3-V1R.bin 0x08000000 verify" -c "reset run" -c "exit"