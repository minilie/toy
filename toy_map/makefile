CXX = g++
CXXFLAGS = -g

TEST_SRC = test.cpp
COMP_SRC = comp.cpp

T_CASE = 10000000
TEST_TARGETS = Btest AVLtest BRtest
COMP_TARGETS = comp

all: $(TEST_TARGETS) $(COMP_TARGETS)

Btest: $(TEST_SRC)
	$(CXX) $(CXXFLAGS) -D T_treetype=BTree -D T_case=$(T_CASE)  -o $@ $(TEST_SRC)

AVLtest: $(TEST_SRC)
	$(CXX) $(CXXFLAGS) -D T_treetype=AVLTree -D T_case=$(T_CASE) -o $@ $(TEST_SRC)

BRtest: $(TEST_SRC)
	$(CXX) $(CXXFLAGS) -D T_treetype=BRTree -D T_case=$(T_CASE) -o $@ $(TEST_SRC)

comp : $(COMP_SRC)
	$(CXX) $(CXXFLAGS) -D T_case=$(T_CASE) -o $@ $(COMP_SRC)

clean:
	rm -f $(TEST_TARGETS) $(COMP_TARGETS)

run: $(TEST_TARGETS) $(COMP_TARGETS)
ifdef target
	@echo "Running $(target):"
	./$(target)
else
	@for target in $(TEST_TARGETS) $(COMP_TARGETS); do \
		echo "Running $$target:"; \
		./$$target | grep -E "ran"; \
	done
endif
