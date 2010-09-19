CPP      = g++
CPPFLAGS = -fPIC -O0 -g -Wall -W -Llib
INCLUDES = -Iinclude -Iinclude/opencv 
SRCS     = src/moustachizer.cpp src/Image.cpp src/ulUtils.cpp
OBJS     = $(SRCS:.cpp=.o)
LIBS     = -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_video -lopencv_features2d -lopencv_calib3d -lopencv_objdetect -lopencv_contrib -lopencv_legacy -ljasper -ljpeg -lpng14 -ltiff -framework QTKit -lz -framework Cocoa -framework Foundation -framework CoreVideo -framework vecLib -framework Security

all: $(OBJS)
	g++ -shared -o moustachizer.so $(CPPFLAGS) $(INCLUDES) $(OBJS) $(LIBS)


clean:
	rm -f *.so src/*.o

.cpp.o:
	$(CPP) -c $(CPPFLAGS) $(INCLUDES) $< -o $@


