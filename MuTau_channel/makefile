CXX = g++
CXXFLAGS = `root-config --cflags`
LIBS = `root-config --libs`

analysis: sinal.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(LIBS)

plot-all: plot_m.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(LIBS)