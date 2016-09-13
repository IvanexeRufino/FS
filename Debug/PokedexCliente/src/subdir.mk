################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../PokedexCliente/src/PokedexCliente.c 

OBJS += \
./PokedexCliente/src/PokedexCliente.o 

C_DEPS += \
./PokedexCliente/src/PokedexCliente.d 


# Each subdirectory must supply rules for building sources it contributes
PokedexCliente/src/%.o: ../PokedexCliente/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


