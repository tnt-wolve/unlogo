CPP      = g++
CPPFLAGS = -fPIC -O0 -g -Wall -W -L/usr/local/lib
INCLUDES = -I/usr/local/include -I/usr/local/include/opencv  -I../src
SRCS     = thumbnailer.cpp ../src/Image.cpp ../src/ulUtils.cpp
OBJS     = $(SRCS:.cpp=.o)
LIBS     = -lopencv_highgui -lopencv_calib3d -lopencv_core -lopencv_features2d -lopencv_imgproc -lopencv_video


all: $(OBJS)
	g++ -shared -o thumbnailer.so $(OBJS) $(LIBS)


clean:
	rm -f *.so *.o

.cpp.o:
	$(CPP) -c $(CPPFLAGS) $(INCLUDES) $< -o $@
