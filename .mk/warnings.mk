#Warning Flags

#A more modern compiler with warnings cranked up.
#I suspect this will catch more warnings than Windriver's diab compiler.
#Update: It does.
C_AND_CPP_WARNS = -Wall -pedantic -Wextra -Wconversion \
-Wno-format-extra-args -Wno-format-zero-length \
-Wformat-nonliteral -Wformat-security -Wformat=2 \
-Wpacked -Wredundant-decls -Wcast-align -Wcast-qual\
-Wshadow -Wswitch-default -Wswitch-enum -Wuninitialized -Winit-self \
-Wunreachable-code -Wlogical-op -Wmissing-declarations \
-Wshadow -Wundef -Wabi

#specific C warning flags
C_ONLY_WARNS = -Wnested-externs -Wstrict-prototypes -Wmissing-prototypes

#specific C++ warning flags
CPP_ONLY_WARNS = -Wctor-dtor-privacy -Wnoexcept -Wold-style-cast \
-Woverloaded-virtual -Wsign-promo -Wstrict-null-sentinel
