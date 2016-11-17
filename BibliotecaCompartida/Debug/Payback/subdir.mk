################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Payback/aceptarConexionCliente.c \
../Payback/charToString.c \
../Payback/conectarConServer.c \
../Payback/convertirStreamAString.c \
../Payback/crearSocketCliente.c \
../Payback/crearSocketServidor.c \
../Payback/enviarCoordenada.c \
../Payback/iniciarSocketServidor.c \
../Payback/paquetes.c \
../Payback/recibirCoordenada.c \
../Payback/string.c 

OBJS += \
./Payback/aceptarConexionCliente.o \
./Payback/charToString.o \
./Payback/conectarConServer.o \
./Payback/convertirStreamAString.o \
./Payback/crearSocketCliente.o \
./Payback/crearSocketServidor.o \
./Payback/enviarCoordenada.o \
./Payback/iniciarSocketServidor.o \
./Payback/paquetes.o \
./Payback/recibirCoordenada.o \
./Payback/string.o 

C_DEPS += \
./Payback/aceptarConexionCliente.d \
./Payback/charToString.d \
./Payback/conectarConServer.d \
./Payback/convertirStreamAString.d \
./Payback/crearSocketCliente.d \
./Payback/crearSocketServidor.d \
./Payback/enviarCoordenada.d \
./Payback/iniciarSocketServidor.d \
./Payback/paquetes.d \
./Payback/recibirCoordenada.d \
./Payback/string.d 


# Each subdirectory must supply rules for building sources it contributes
Payback/%.o: ../Payback/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


