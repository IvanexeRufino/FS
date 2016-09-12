################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../PokedexServidor/src/PokedexServidor.c 

OBJS += \
./PokedexServidor/src/PokedexServidor.o 

C_DEPS += \
./PokedexServidor/src/PokedexServidor.d 


# Each subdirectory must supply rules for building sources it contributes
PokedexServidor/src/%.o: ../PokedexServidor/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


