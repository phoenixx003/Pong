#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxOpenCv.h"
#include "ofxGui.h"
#include "ofxOsc.h"

#include "time.h"

class ofApp : public ofBaseApp {

public:
	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	ofVideoGrabber webcam;

	ofxCvColorImage image;
	ofPixels pixel;

	ofxPanel gui;

	ofxCv::ContourFinder contour1;
	cv::Point2f centerContour1;
	ofxCv::ContourFinder contour2;
	cv::Point2f centerContour2;

	ofColor colorPick;
	ofColor color1;
	ofColor color2;

	ofParameter<float> hue1;
	ofParameter<float> sat1;
	ofParameter<float> val1;

	ofParameter<float> hue2;
	ofParameter<float> sat2;
	ofParameter<float> val2;

	int playerHeight;
	int playerWidth;
	int playerSpacing;

	ofParameter<bool> toggleWebcam;
	ofParameter<bool> toggleContour;
	ofParameter<bool> toggleGui;

	ofParameter<float> threshold1;
	ofParameter<float> threshold2;
	ofParameter<bool> trackHueSat;

	int ballPosX;
	float ballPosXf;
	int ballPosY;
	float ballPosYf;
	int ballSize;
	int ballSpeed;
	float ballAngleX;
	float ballAngleY;	// Winkel des Spielballvektors in Y-Richtung
	int signY;			// Vorzeichen des Spielballvektors

	int posP1;
	int posP2;
};
