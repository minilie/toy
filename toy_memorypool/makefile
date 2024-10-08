CXX = g++

CXXFLAGS = -std=c++11

SRCS = mtest.cpp
OBJS = $(SRCS:.cpp=.o)
DEPS = $(SRCS:.cpp=.d)

TARGET = test

all : $(TARGET)

$(TARGET) : $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
	$(CXX) $(CXXFLAGS) -MMD -MF $*.d -c $<

-include $(DEPS)

clean :
	rm -f $(OBJS) $(DEPS) $(TARGET)

run : 
	@echo "Running $(TARGET):"
	@./$(TARGET)
