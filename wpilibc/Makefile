# force powerpc compilers
CXX=powerpc-wrs-vxworks-g++
AR=powerpc-wrs-vxworks-ar

CPPFILES := $(shell find -name '*.cpp' | sort | sed 's/\.\///g')
OBJS = $(CPPFILES:.cpp=.o)

WPILib.a: $(OBJS)
	@echo " [AR]	WPILib.a"
	@$(AR) crus WPILib.a $(OBJS)

%.o: %.cpp
	@echo " [G++]	"$?
	@mkdir -p `dirname Debug/$@`
	@$(CXX) -o $@ -c $? -g -I. -mcpu=603 -mstrict-align -mlongcall -Wall -DTOOL=GNU -D'SVN_REV="3623+1"' -std=c++11

clean-objects:
	@$(foreach obf, $(shell find -name '*.o' | sort | sed 's/\.\///g'), echo " [RM]	"$(obf);rm -rf $(obf);)
	
clean-wpilib:
	@echo " [RM]	WPILib.a"
	@rm -rf WPILib.a
	
clean: clean-objects clean-wpilib
	
install:
	mkdir -p $(DESTDIR)$(PREFIX)/lib
	cp WPILib.a $(DESTDIR)$(PREFIX)/lib/libWPILib.a
	$(foreach hdir,$(shell find -name '*.h' | sed 's/\.\///g' | xargs -L 1 dirname | sort | uniq),mkdir -p $(DESTDIR)$(PREFIX)/include/WPILib/$(hdir);)
	$(foreach header, $(shell find -name '*.h' | sed 's/\.\///g'), cp $(header) $(DESTDIR)$(PREFIX)/include/WPILib/$(header);)

rebuild: clean all

all: WPILib.a
