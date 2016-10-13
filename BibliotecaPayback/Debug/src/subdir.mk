################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/paquetes.c \
../src/recibirCoordenada.c \
../src/string.c 

OBJS += \
./src/paquetes.o \
./src/recibirCoordenada.o \
./src/string.o 

C_DEPS += \
./src/paquetes.d \
./src/recibirCoordenada.d \
./src/string.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


