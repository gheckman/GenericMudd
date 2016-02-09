#MAIN PROGRAM
PROG_NAME = main

#RULE TO BUILD THE PROGRAM
all: $(PROG_NAME)

#--------------------#
# TRANSCOMPILE RULES |
#--------------------#
#hcp files
%.cpp %.hpp: %.hcp
	@echo "[0;36m$^[0m";
	@hcp-compiler --no-utility-functions $^;

#-----------#
# C++ RULES |
#-----------#
#object
%.o: %.cpp %.h
	@echo "[0;34m$<[0m";
	@$(CPP_COMPILER) $(LOOK_PATHS) $(CPP_FLAGS) -c $< -o $@
	
#main
%.o: %.cpp
	@echo "[0;34m$<[0m";
	@$(CPP_COMPILER) $(LOOK_PATHS) $(CPP_FLAGS) -c $< -o $@

#-----------#
#  C RULES  |
#-----------#
#object
%.o: %.c %.h
	@echo "[0;34m$<[0m";
	@$(C_COMPILER) $(LOOK_PATHS) $(C_FLAGS) -c $< -o $@
	
#main
%.o: %.c
	@echo "[0;34m$<[0m";
	@$(C_COMPILER) $(LOOK_PATHS) $(C_FLAGS) -c $< -o $@

#-----------------#
#  Linking C/C++  |
#-----------------#
$(PROG_NAME): $(CPP_SOURCE_PATHS:%.cpp=%.o) $(C_SOURCE_PATHS:%.c=%.o) $(HCP_SOURCE_PATHS:%.hcp=%.o)
	@$(CPP_COMPILER) $(LOOK_PATHS) $(CPP_FLAGS) -o $@ $^ -Wl,--as-needed -Wl,--start-group $(LIBS)
	@echo "[0;35msuccessful link[0m"

#-------------------#
#  recursive clean  |
#-------------------#
clean:
	@rm -rf *~ *.o *.d *.~ $(PROG_NAME) `ls-cpp --only-generated --also-sympulls`;
	@rm -rf $(call rwildcard,,*~) $(call rwildcard,,*.o) $(call rwildcard,,*.d)
	@echo "cleaned";
