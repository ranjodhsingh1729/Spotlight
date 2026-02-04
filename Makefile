CXX := g++
CXXFLAGS := -Wall
IFLAGS := -I./src -I./3rdparty/tensorflow
LDFLAGS := -L./3rdparty -ltensorflowlite -lyuv -lturbojpeg -lspng -lv4l2 -Wl,-rpath,'$$ORIGIN/3rdparty'
SRC := src/spotlight.cpp

spotlight: $(SRC)
	$(CXX) -O3 -mavx2 $(CXXFLAGS) $(IFLAGS) $^ $(LDFLAGS) -o $@

spotlight_debug: $(SRC)
	$(CXX) -g -fsanitize=address -fno-omit-frame-pointer $(CXXFLAGS) $(IFLAGS) $^ $(LDFLAGS) -o $@
