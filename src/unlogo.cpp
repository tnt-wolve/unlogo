/*
 *  unlogo.cpp
 *  unlogo
 *
 *  Created by Jeffrey Crouse and Kashif Balil
 *  Copyright 2010 Eyebeam. All rights reserved.
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <iostream>

#include "Image.h"
#include "OpticalFlow.h"
#include "Logo.h"

using namespace unlogo;


Image input, output, prev;
int framenum=0;
int targetframe;

// Do we really need the points in all of these different formats?
Point corners[4];
Rect roi;
Point topleft;
Point botright;
Mat contour;

extern "C" int init( const char* argstr )
{
	try {
		/* print a welcome message, and the OpenCV version */
		log(LOG_LEVEL_DEBUG, "Welcome to unlogo, using OpenCV version %s (%d.%d.%d)\n",
				CV_VERSION, CV_MAJOR_VERSION, CV_MINOR_VERSION, CV_SUBMINOR_VERSION);
		
		// Parse arguments.
		vector<string> argv = split(argstr, ":");
		int argc = argv.size();
		if(argc != 9) {
			log(LOG_LEVEL_ERROR, "You must supply 9 arguments.");
			exit(-1);
		}
		
		
		// Grab and parse arguments
		// This is so un-elegant.
		targetframe = atol(argv[0].c_str());
		int i,j;
		int minx=numeric_limits<int>::max();
		int miny=numeric_limits<int>::max();
		int maxx=0;
		int maxy=0;
		contour = Mat(4, 1, CV_32FC2);
		for(i=0,j=1; i<4; i++,j+=2) {
			corners[i]=Point2f(atol(argv[j].c_str()), atol(argv[j+1].c_str()));
			minx=min(corners[i].x, minx); miny=min(corners[i].y, miny);
			maxx=max(corners[i].x, maxx); maxy=max(corners[i].y, maxy);
			
			float* ptr = contour.ptr<float>(i);
			*ptr++ = corners[i].x;
			*ptr++ = corners[i].y;
		}
		roi=Rect(minx, miny, maxx-minx, maxy-miny);
		topleft = Point(minx, miny);
		botright = Point(maxx, maxy);
		// We want the contour to be relative to the roi.
		for(int i=0; i<4; i++)
		{
			float *ptr = contour.ptr<float>(i);
			(*ptr++) -= topleft.x;
			(*ptr++) -= topleft.y;
		}
		cout << "---" << endl;
		
#ifdef DEBUG		
		namedWindow("input");		cvMoveWindow("input", 0, 0);
		namedWindow("output");		cvMoveWindow("output", 0, 510);
#endif 
		
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
	log(LOG_LEVEL_DEBUG, "=== Frame %d ===", framenum);
	input.setData( width, height, src[0], src_stride[0]);
	if(input.empty()) return 1;

#ifdef DEBUG
	input.show("input");
#endif
	
	// Before we draw onto it, keep a copy of this frame for optical flow detection next frame
	prev = Image( input );

	
	
	for(int i = 0; i < 4; i++ )
	{
		line( input.cvImage, corners[i%4], corners[(i+1)%4], Scalar(0,0,255) );
	}
	rectangle(input.cvImage, topleft, botright, Scalar(0,255,0));
	
	if(framenum==targetframe)
	{
		input(roi).drawFeatures("SURF", contour);
		input.show("target");
	}
	
	

	for(int i = 0; i < 4; i++ )
	{
		line( input.cvImage, corners[i%4], corners[(i+1)%4], Scalar(0,0,255) );
	}
	rectangle(input.cvImage, topleft, botright, Scalar(0,255,0));


	output.setData( width, height, dst[0], dst_stride[0] );		// point the 'output' image to the FFMPEG data array
	output.copyFromImage(input);								// copy input into the output memory
	output.text("unlogo", 10, height-10, .5);					// watermark, dude!
	CV_Assert(&output.cvImage.data[0]==&dst[0][0]);				// Make sure output still points to dst

	
#ifdef DEBUG	
	output.show("output");
	waitKey(1);	// needed to update windows.
#endif

	framenum++;
	return 0;
}

