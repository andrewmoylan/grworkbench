binary_name = lax
library_name = lib/liblax.a

compiler_flags = -O3 -I..

linker_flags = -O3
libraries = 

# Don't change anythying below here.

sources = $(wildcard *.cpp)
objects = $(sources:.cpp=.o)
dependencies = $(sources:.cpp=.d)

$(binary_name) : $(objects)
	g++ $(linker_flags) -o $(binary_name) $(objects) $(libraries)

include $(dependencies)

%.o: %.cpp
	g++ -c $(compiler_flags) -o $@ $<

%.d: %.cpp
	@set -e; rm -f $@; \
        g++ $(compiler_flags) -MM $< > $@.$$$$; \
        sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
        rm -f $@.$$$$

clean:
	rm -f *.o *.d $(binary_name) $(library_name)

library : $(objects)
	ar cru $(library_name) $(objects)
	ranlib $(library_name)
