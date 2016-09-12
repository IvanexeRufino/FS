################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../PokedexServidor/src/FileSysOSADA/filesys.c 

OBJS += \
./PokedexServidor/src/FileSysOSADA/filesys.o 

C_DEPS += \
./PokedexServidor/src/FileSysOSADA/filesys.d 


# Each subdirectory must supply rules for building sources it contributes
PokedexServidor/src/FileSysOSADA/%.o: ../PokedexServidor/src/FileSysOSADA/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


