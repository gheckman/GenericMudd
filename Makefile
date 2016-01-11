#A small custom makefile to build a single executable for any number of source and header files.
#Every source and header file will be grabbed recursively.  

#various build settings
include .mk/warnings.mk
include .mk/language-standards.mk
include .mk/compilers.mk
include .mk/libraries.mk
include .mk/utility.mk

#Complete set of flags to give compiler
#MD and MP help deduce dependencies automatically
C_FLAGS = -MD -MP $(C_STANDARD) $(C_ONLY_WARNS) $(C_AND_CPP_WARNS)
CPP_FLAGS = -MD -MP $(CPP_STANDARD) $(CPP_ONLY_WARNS) $(C_AND_CPP_WARNS)

# recursively get all the source files from dependencies pulled in.
C_SOURCE_PATHS = $(call rwildcard,Mudd,*.c)
CPP_SOURCE_PATHS = $(call rwildcard,Mudd,*.cpp)
HCP_SOURCE_PATHS = $(call rwildcard,Mudd,*.hcp)
INCLUDE_PATHS = $(shell find Mudd -type d)

#current directory of makefile
C_SOURCE_PATHS += $(wildcard *.c)
CPP_SOURCE_PATHS += $(wildcard *.cpp)
HCP_SOURCE_PATHS += $(wildcard *.hcp)
INCLUDE_PATHS += $(shell pwd)

# any source file can include any header found in the directory tree
# as such, we need to build up -I flags and feed them to the compiler
LOOK_PATHS = $(addprefix -I,$(INCLUDE_PATHS))

#assume that none of our rules create files that are used in rules to create other files.
#although we may have intermediate files, we don't want make to automatically delete them.  
#for example, a transcompiled source file, creates source files used to create objects.  
#we don't want the source files deleted after an object is built.
.SECONDARY:

#our custom rules to transcompile code, build objects, and link.
#there really shouldn't be any reason that you need to modify these rules/
include .mk/rules.mk

#rules to trigger builds based on derived dependencies
include .mk/generated_dependency_rules.mk
