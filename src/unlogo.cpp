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
#include "Logo.h"

using namespace unlogo;


Image input, output, prev;
int framenum=0;
int targetframe;
int inititalFeatures;
double initialArea;
Mat homography;
bool targetFound;

// Do we really need the points in all of these different formats?
Mat contour;
Point corners[4];

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
		targetframe = atol(argv[0].c_str());
		contour = Mat(4, 1, CV_32FC2);  // 4row 1col 2channel matrix
		int i,j;
		for(i=0,j=1; i<4; i++,j+=2)
		{
			float* ptr = contour.ptr<float>(i);
			ptr[0] = (float)atol(argv[j+0].c_str());
			ptr[1] = (float)atol(argv[j+1].c_str());
			corners[i] = Point(ptr[0], ptr[1]);
		}
		initialArea = contourArea(contour);
		
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
	cout << "(frame " << framenum << ")  ";
	input.setData( width, height, src[0], src_stride[0]);
	if(input.empty()) return 1;

#ifdef DEBUG
	input.show("input");
#endif
	
	if(framenum == targetframe)
	{
		input.findFeatures("SURF", contour);
		prev.copyFromImage( input );
		inititalFeatures = input.features.size();
		targetFound = true;
	}
	
	if(targetFound)
	{
		input.updateFeatures( prev, homography );
		prev.copyFromImage( input );
		float pctFeaturesRemaining = input.features.size()/(float)inititalFeatures;
		
		log(LOG_LEVEL_DEBUG, "%f%% of features remaining", pctFeaturesRemaining);
		
		if(pctFeaturesRemaining<.2)
		{
			log(LOG_LEVEL_DEBUG, "Only %f%% features remaining.  Done!", pctFeaturesRemaining);
			targetFound=false;
		}
		
		if(homography.empty())
		{
			log(LOG_LEVEL_DEBUG, "Empty Homography.  Done!");
			targetFound=false;
		}
		
		Mat contourTmp;
		perspectiveTransform(contour, contourTmp, homography);
		contour = contourTmp;
		
		if(!isContourConvex(contour))
		{
			log(LOG_LEVEL_DEBUG, "Box is not convex. Done!");
			targetFound=false;
		}
		
		double area = contourArea(contour);
		if(area<50)
		{
			log(LOG_LEVEL_DEBUG, "Box is too small.  Done!");
			targetFound=false;
		}
		
		double areaDiff = area/(float)initialArea;
		if(areaDiff > 5 || areaDiff < .1)
		{
			log(LOG_LEVEL_DEBUG, "Box is %fx its original size.  Done!", areaDiff);
			targetFound=false;
		}
	}

	if(targetFound)
	{
		// Move corner points fowards the homography points
		for(int i=0; i<4; i++)
		{
			float* ptr = contour.ptr<float>(i);
			Point flow(ptr[0], ptr[1]);
			lerp(corners[i], flow, 1.8);
		}
		
		// Draw the rect.
		const Point* pts[1] = {corners};
		int npts[1] = {4};
		fillPoly(input.cvImage, pts, npts, 1, CV_RGB(0,0,0));	
	}
	
	output.setData( width, height, dst[0], dst_stride[0] );		// point the 'output' image to the FFMPEG data array
	output.copyFromImage(input);								// copy input into the output memory
	output.text("unlogo", 10, height-10, .5);					// watermark, dude!
	CV_Assert(&output.cvImage.data[0]==&dst[0][0]);				// Make sure output still points to dst

	
#ifdef DEBUG	
	output.show("output");
	waitKey(3);	// needed to update windows.
#endif

	framenum++;
	return 0;
}

