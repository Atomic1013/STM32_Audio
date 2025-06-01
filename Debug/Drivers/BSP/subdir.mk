################################################################################
# 自动生成的文件。不要编辑！
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# 将这些工具调用的输入和输出添加到构建变量 
C_SRCS += \
../Drivers/BSP/my_audio.c \
../Drivers/BSP/wm8978.c 

OBJS += \
./Drivers/BSP/my_audio.o \
./Drivers/BSP/wm8978.o 

C_DEPS += \
./Drivers/BSP/my_audio.d \
./Drivers/BSP/wm8978.d 


# 每个子目录必须为构建它所贡献的源提供规则
Drivers/BSP/%.o Drivers/BSP/%.su Drivers/BSP/%.cyclo: ../Drivers/BSP/%.c Drivers/BSP/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_PWR_LDO_SUPPLY -DUSE_HAL_DRIVER -DSTM32H743xx -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I"D:/GitHub/STM32_Audio/Drivers/BSP" -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-BSP

clean-Drivers-2f-BSP:
	-$(RM) ./Drivers/BSP/my_audio.cyclo ./Drivers/BSP/my_audio.d ./Drivers/BSP/my_audio.o ./Drivers/BSP/my_audio.su ./Drivers/BSP/wm8978.cyclo ./Drivers/BSP/wm8978.d ./Drivers/BSP/wm8978.o ./Drivers/BSP/wm8978.su

.PHONY: clean-Drivers-2f-BSP

