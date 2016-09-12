################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Mapa/src/Mapa.c 

OBJS += \
./Mapa/src/Mapa.o 

C_DEPS += \
./Mapa/src/Mapa.d 


# Each subdirectory must supply rules for building sources it contributes
Mapa/src/%.o: ../Mapa/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


