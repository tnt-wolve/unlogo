/*
 *  thumbnailer.cpp
 *  unlogo
 *
 *  Created by Jeffrey Crouse on 9/2/10.
 *  Copyright 2010 Eyebeam. All rights reserved.
 *
 */

#define DEBUG 1

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <iostream>
#include "Image.h"

using namespace unlogo;
int framenum=0;
int numsaved=0;
int skip;
string prefix;

extern "C" int init( const char* argstr )
{
	try {
		log(LOG_LEVEL_DEBUG, "Welcome to unlogo, using OpenCV version %s (%d.%d.%d)\n",
			CV_VERSION, CV_MAJOR_VERSION, CV_MINOR_VERSION, CV_SUBMINOR_VERSION);
		
		vector<string> argv = split(argstr, ":");
		int argc = argv.size();
		
		if(argc != 2)
		{
			log(LOG_LEVEL_ERROR, "Please provide the gap between frames and the prefix for the outputted frames.");
			return -1;
		}
		skip = atol(argv[0].c_str());
		prefix = argv[1];
		log(LOG_LEVEL_DEBUG, "Saving every %d frames. Saving to %s", skip, prefix.c_str());
		return 0;
	}
	catch ( ... ) {
		return -1;
	}
}

extern "C" int uninit()
{
	return 0;
}

extern "C" int process( uint8_t* dst[4], int dst_stride[4],
					   uint8_t* src[4], int src_stride[4],
					   int width, int height)
{
	Image input( width, height, src[0], src_stride[0]);
	if(input.empty()) return 1;
	if(framenum%skip==0)
	{
		char filename[255];
		sprintf(filename, "%s/image_%05d.jpg", prefix.c_str(), numsaved);
		input.convert(CV_BGR2RGB);
		input.save(filename);
		numsaved++;
	}
		
#ifdef DEBUG
	input.show("output");
	waitKey(100);
#endif
	
	Image output(width, height, dst[0], dst_stride[0]);			// point the 'output' image to the FFMPEG data array	
	output.copyFromImage(input);								// copy input into the output memory
	CV_Assert(&output.cvImage.data[0]==&dst[0][0]);				// Make sure output still points to dst

	
	framenum++;
	return 0;
}


