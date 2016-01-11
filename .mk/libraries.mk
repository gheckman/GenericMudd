#Libraries
BOOST_LIBS = -lboost_program_options -lboost_filesystem -lboost_system -lboost_iostreams -lboost_serialization -lboost_thread
GMP_LIBS = -lgmpxx -lgmp
NCURSES_LIBS = -lncurses -lmenu -lpanel -lform -lncurses++
OTHER_LIBS = -lpthread
OPTIMIZE = -Ofast
LIBS = $(BOOST_LIBS) $(GMP_LIBS) $(NCURSES_LIBS) $(OTHER_LIBS)
