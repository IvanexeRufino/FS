################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../pkmnUtils/test/pkmn_battle_test.c 

OBJS += \
./pkmnUtils/test/pkmn_battle_test.o 

C_DEPS += \
./pkmnUtils/test/pkmn_battle_test.d 


# Each subdirectory must supply rules for building sources it contributes
pkmnUtils/test/%.o: ../pkmnUtils/test/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


