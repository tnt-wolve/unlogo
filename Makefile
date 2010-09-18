CPP      = g++
CPPFLAGS = -fPIC -O0 -g -Wall -W -Llib
INCLUDES = -Iinclude -Iinclude/opencv 
SRCS     = src/moustachizer.cpp src/Image.cpp src/ulUtils.cpp
OBJS     = $(SRCS:.cpp=.o)
LIBS     = -lopencv_core.2.1.1 -lopencv_imgproc.2.1.1 -lopencv_highgui.2.1.1 -lopencv_ml.2.1.1 -lopencv_video.2.1.1 -lopencv_features2d.2.1.1 -lopencv_calib3d.2.1.1 -lopencv_objdetect.2.1.1 -lopencv_contrib.2.1.1 -lopencv_legacy.2.1.1


all: $(OBJS)
	g++ -shared -o moustachizer.so $(CPPFLAGS) $(INCLUDES) $(OBJS) $(LIBS)


clean:
	rm -f *.so src/*.o

.cpp.o:
	$(CPP) -c $(CPPFLAGS) $(INCLUDES) $< -o $@


