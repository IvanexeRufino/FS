################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/paquetes.c 

OBJS += \
./src/paquetes.o 

C_DEPS += \
./src/paquetes.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/workspace/tp-2016-2c-SO-II-The-Payback/BibliotecaPayback/src" -I"/home/utnso/workspace/tp-2016-2c-SO-II-The-Payback/Entrenador/src" -I"/home/utnso/workspace/tp-2016-2c-SO-II-The-Payback/Mapa/src" -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


