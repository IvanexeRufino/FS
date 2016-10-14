################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include so-commons-library/tests/unit-tests/subdir.mk
-include so-commons-library/tests/integration-tests/logger/subdir.mk
-include so-commons-library/src/commons/collections/subdir.mk
-include so-commons-library/src/commons/subdir.mk
-include pkmnUtils/test/subdir.mk
-include pkmnUtils/src/pkmn/subdir.mk
-include Shared/src/subdir.mk
-include Shared/Debug/src/subdir.mk
-include PokedexServidor/src/FileSysOSADA/subdir.mk
-include PokedexServidor/src/subdir.mk
-include PokedexCliente/src/subdir.mk
-include Mapa/src/subdir.mk
-include Mapa/Debug/src/subdir.mk
-include Entrenador/src/subdir.mk
-include Entrenador/Debug/src/subdir.mk
-include BibliotecaPayback/src/subdir.mk
-include BibliotecaPayback/Debug/src/subdir.mk
-include .metadata/.plugins/org.eclipse.cdt.managedbuilder.core/subdir.mk
-include .metadata/.plugins/org.eclipse.cdt.make.core/subdir.mk
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
all: tp-2016-2c-SO-II-The-Payback

# Tool invocations
tp-2016-2c-SO-II-The-Payback: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc  -o "tp-2016-2c-SO-II-The-Payback" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) tp-2016-2c-SO-II-The-Payback
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
