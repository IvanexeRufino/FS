################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Entrenador/src/Entrenador.c 

OBJS += \
./Entrenador/src/Entrenador.o 

C_DEPS += \
./Entrenador/src/Entrenador.d 


# Each subdirectory must supply rules for building sources it contributes
Entrenador/src/%.o: ../Entrenador/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


