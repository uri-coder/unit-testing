################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../UR_code/src/main_ur.c \
../UR_code/src/server.c \
../UR_code/src/ur_task.c 

OBJS += \
./UR_code/src/main_ur.o \
./UR_code/src/server.o \
./UR_code/src/ur_task.o 

C_DEPS += \
./UR_code/src/main_ur.d \
./UR_code/src/server.d \
./UR_code/src/ur_task.d 


# Each subdirectory must supply rules for building sources it contributes
UR_code/src/%.o UR_code/src/%.su UR_code/src/%.cyclo: ../UR_code/src/%.c UR_code/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g -DDEBUG -DUSE_HAL_DRIVER -DSTM32F746xx -c -I"C:/unit testing project/unit-test/unit testing stm side/UR_code/developer_code" -I"C:/unit testing project/unit-test/unit testing stm side/UR_code/inc" -I"C:/unit testing project/unit-test/unit testing stm side/UR_code/developer_code/tests inc" -I"C:/unit testing project/unit-test/unit testing stm side/UR_code/developer_code/tests inc" -I../Core/Inc -I../LWIP/App -I../LWIP/Target -I../Middlewares/Third_Party/LwIP/src/include -I../Middlewares/Third_Party/LwIP/system -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1 -I../Drivers/BSP/Components/lan8742 -I../Middlewares/Third_Party/LwIP/src/include/netif/ppp -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Middlewares/Third_Party/LwIP/src/include/lwip -I../Middlewares/Third_Party/LwIP/src/include/lwip/apps -I../Middlewares/Third_Party/LwIP/src/include/lwip/priv -I../Middlewares/Third_Party/LwIP/src/include/lwip/prot -I../Middlewares/Third_Party/LwIP/src/include/netif -I../Middlewares/Third_Party/LwIP/src/include/compat/posix -I../Middlewares/Third_Party/LwIP/src/include/compat/posix/arpa -I../Middlewares/Third_Party/LwIP/src/include/compat/posix/net -I../Middlewares/Third_Party/LwIP/src/include/compat/posix/sys -I../Middlewares/Third_Party/LwIP/src/include/compat/stdc -I../Middlewares/Third_Party/LwIP/system/arch -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-UR_code-2f-src

clean-UR_code-2f-src:
	-$(RM) ./UR_code/src/main_ur.cyclo ./UR_code/src/main_ur.d ./UR_code/src/main_ur.o ./UR_code/src/main_ur.su ./UR_code/src/server.cyclo ./UR_code/src/server.d ./UR_code/src/server.o ./UR_code/src/server.su ./UR_code/src/ur_task.cyclo ./UR_code/src/ur_task.d ./UR_code/src/ur_task.o ./UR_code/src/ur_task.su

.PHONY: clean-UR_code-2f-src

