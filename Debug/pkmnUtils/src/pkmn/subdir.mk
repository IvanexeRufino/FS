################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../pkmnUtils/src/pkmn/battle.c \
../pkmnUtils/src/pkmn/factory.c 

OBJS += \
./pkmnUtils/src/pkmn/battle.o \
./pkmnUtils/src/pkmn/factory.o 

C_DEPS += \
./pkmnUtils/src/pkmn/battle.d \
./pkmnUtils/src/pkmn/factory.d 


# Each subdirectory must supply rules for building sources it contributes
pkmnUtils/src/pkmn/%.o: ../pkmnUtils/src/pkmn/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


