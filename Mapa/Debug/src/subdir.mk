################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Mapa.c 

OBJS += \
./src/Mapa.o 

C_DEPS += \
./src/Mapa.d 


# Each subdirectory must supply rules for building sources it contributes
src/Mapa.o: ../src/Mapa.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/tp-2016-2c-SO-II-The-Payback/BibliotecaCompartida" -I/home/utnso/workspace/tp-2016-2c-SO-II-The-Payback/so-pkmn-utils -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"src/Mapa.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


