################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: Mapa

dependents:
	-cd /home/utnso/Escritorio/tp/tp-2016-2c-SO-II-The-Payback/BibliotecaCompartida/Debug && $(MAKE) all

# Tool invocations
Mapa: $(OBJS) $(USER_OBJS) /home/utnso/Escritorio/tp/tp-2016-2c-SO-II-The-Payback/BibliotecaCompartida/Debug/libBibliotecaCompartida.so
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc -L"/home/utnso/Escritorio/tp/tp-2016-2c-SO-II-The-Payback/BibliotecaCompartida/Debug" -L"/home/utnso/Escritorio/tp/tp-2016-2c-SO-II-The-Payback/BibliotecaCompartida" -o "Mapa" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) Mapa
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:
/home/utnso/Escritorio/tp/tp-2016-2c-SO-II-The-Payback/BibliotecaCompartida/Debug/libBibliotecaCompartida.so:

-include ../makefile.targets
