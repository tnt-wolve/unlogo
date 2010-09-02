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


Image prev;						// The last frame -- for optical flow.
int framenum=0;					// Current frame number
int targetframe;				// The frame number on which the target logo appears
int inititalFeatures;			// The number of feaatures in the target frame/bounds
double initialArea;				// The initial area of the black box
bool targetFound;				// Have we gotten to the target and is it still visible?
bool coordsNormalized=false;	// Did the coords come in normalized?
Mat contour;					// Black box contour


extern "C" int init( const char* argstr )
{
	try {
		log(LOG_LEVEL_DEBUG, "Welcome to unlogo, using OpenCV version %s (%d.%d.%d)\n",
				CV_VERSION, CV_MAJOR_VERSION, CV_MINOR_VERSION, CV_SUBMINOR_VERSION);
		
		// Parse arguments.
		vector<string> argv = split(argstr, ":");
		int argc = argv.size();
		if(argc < 9)
		{
			log(LOG_LEVEL_ERROR, "You must supply at least 9 arguments.");
			exit(-1);
		}
		
		// Grab and parse arguments
		targetframe = atol(argv[0].c_str());
		contour = Mat(4, 1, CV_32FC2);  // 4row 1col 2channel matrix
		int i,j;
		float x, y;
		float total;
		for(i=0,j=1; i<4; i++,j+=2)
		{
			x = atof(argv[j+0].c_str());
			y = atof(argv[j+1].c_str());
			float* ptr = contour.ptr<float>(i);
			ptr[0] = x;
			ptr[1] = y;
			total += x;	
		}

		// Ghetto way of determining whether coordinates are normalized to
		// maintain backwards compatability.
		if(total < 4.)
		{
			coordsNormalized=true;
		}
		
		// Get the hull of the points provided so we don't fail right away if the points are convex
		vector<Point2f> hull;
		convexHull(contour, hull, true); 
		contour = points2mat(hull);
		initialArea = contourArea(contour);
		
#ifdef DEBUG		
		namedWindow("input");		cvMoveWindow("input", 0, 0);
		namedWindow("output");		cvMoveWindow("output", 650, 0);
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
	Image input( width, height, src[0], src_stride[0]);
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
		Mat homography;
		input.updateFeatures( prev, homography );
		prev.copyFromImage( input );
		float pctFeaturesRemaining = input.features.size()/(float)inititalFeatures;
		
		if(pctFeaturesRemaining<.2)
		{
			char failMessage[255];
			sprintf(failMessage, "Only %f%% features remaining.  Done!", pctFeaturesRemaining);
			input.text(failMessage, 10, height-10, .5);
			log(LOG_LEVEL_DEBUG, failMessage);
			targetFound=false;
		}
		
		if(homography.empty())
		{
			const char* failMessage =  "Empty Homography.  Done!";
			log(LOG_LEVEL_DEBUG, failMessage);
			input.text(failMessage, 10, height-10, .5);
			targetFound=false;
		}
		
		Mat contourTmp;
		perspectiveTransform(contour, contourTmp, homography);
		contour = contourTmp;
		
		if(!isContourConvex(contour))
		{
			const char* failMessage = "Box is not convex. Done!";
			log(LOG_LEVEL_DEBUG, failMessage);
			input.text(failMessage, 10, height-10, .5);
			targetFound=false;
		}
		
		double area = contourArea(contour);
		if(area<50)
		{
			const char* failMessage = "Box is too small.  Done!";
			log(LOG_LEVEL_DEBUG, failMessage);
			input.text(failMessage, 10, height-10, .5);
			targetFound=false;
		}
		
		float areaDiff = area/(float)initialArea;
		if(areaDiff > 5 || areaDiff < .1)
		{
			char failMessage[255];
			sprintf(failMessage,"Box is %fx its original size.  Done!", areaDiff);
			input.text(failMessage, 10, height-10, .5);
			log(LOG_LEVEL_DEBUG, failMessage);
			targetFound=false;
		}
	}

	if(targetFound)
	{
		Point corners[4];
		
		// Move corner points fowards the homography points
		for(int i=0; i<4; i++)
		{
			float* ptr = contour.ptr<float>(i);
			corners[i] = Point(ptr[0], ptr[1]);
			if(coordsNormalized)
			{
				corners[i].x += width;
				corners[i].y *= height;
			}
			//lerp(corners[i], flow, 1.8);
		}
		
		// Draw the rect.
		fillConvexPoly(input.cvImage, corners, 4, CV_RGB(0,0,0));
	}

	Image output(width, height, dst[0], dst_stride[0]);			// point the 'output' image to the FFMPEG data array	
	output.copyFromImage(input);								// copy input into the output memory

	CV_Assert(&output.cvImage.data[0]==&dst[0][0]);				// Make sure output still points to dst

	
#ifdef DEBUG	
	output.show("output");
	waitKey(3);	// needed to update windows.
#endif

	framenum++;
	return 0;
}

