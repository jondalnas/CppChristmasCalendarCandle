SRCDIR = src
INCDIR = inc
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = ${patsubst src/%, obj/%, $(SOURCES:.cpp=.o)}
DEPEND  = ${patsubst src/%, obj/%, $(SOURCES:.cpp=.d)}

CPPFLAGS = -Wall -fsanitize=address

a.out: $(OBJECTS) $(DEPEND)
	echo "$(SOURCES)"
	$(CXX) -I $(INCDIR) $(CPPFLAGS) $(OBJECTS) -o $@ $(LIBS)

obj/%.o:$(SRCDIR)/%.cpp
	$(CXX) -c $^ -I $(INCDIR) $(CPPFLAGS) -o $@

obj/%.d: $(SRCDIR)/%.cpp
	@set -e
	rm -f $@
	$(CXX) -MM -I $(INCDIR) $(CPPFLAGS) $< > $@.tmp
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.tmp > $@
	rm -f $@.tmp

include $(DEPEND)

.PHONY: debug clean

debug: a.out
	CPPFLAGS += -g

clean:
	rm -f obj/*
	rm -f a.out
