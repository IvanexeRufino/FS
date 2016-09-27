################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../PokedexServidor/src/FileSysOSADA/osada.c 

OBJS += \
./PokedexServidor/src/FileSysOSADA/osada.o 

C_DEPS += \
./PokedexServidor/src/FileSysOSADA/osada.d 


# Each subdirectory must supply rules for building sources it contributes
PokedexServidor/src/FileSysOSADA/%.o: ../PokedexServidor/src/FileSysOSADA/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/workspace/tp-2016-2c-SO-II-The-Payback" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


