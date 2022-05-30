#pragma once

#include "ofMain.h"

#include "ofxCv.h"
#include "ofxOpenCv.h"
#include "ofParameter.h"

#include "ofxGui.h"

#include "ofxOsc.h"

#include "time.h"

using namespace ofxCv;
using namespace cv;
using namespace std;

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

	ofxPanel gui;

	ofxCvColorImage image;
	ofPixels pixel;

	ContourFinder contour1;
	ContourFinder contour2;

	Point2f centerContour1;
	Point2f centerContour2;

	ofColor colorPick;
	ofColor color1;
	ofColor color2;

	ofParameter<float> hue1;
	ofParameter<float> sat1;
	ofParameter<float> val1;
	ofParameter<float> hue2;
	ofParameter<float> sat2;
	ofParameter<float> val2;

	ofParameter<float> threshold1;
	ofParameter<float> threshold2;

	ofParameter<bool> toggleWebcam;
	ofParameter<bool> toggleContour;
	ofParameter<bool> toggleGui;

	ofParameter<bool> connection;
	ofParameter<bool> master;

	ofxOscReceiver oscReceiver;
	ofxOscSender oscSender;
	ofxOscMessage oscMessageTX;
	ofxOscMessage oscMessageRX;

	// Player Dimensionen
	int playerHeight;
	int playerWidth;
	int playerSpacing;
	int posP1;
	int posP2;

	// Ball Dimensionen
	int ballPosX;		// Ballposition x-Achse
	float ballPosXf;	
	int ballPosY;		// Ballposition y-Achse
	float ballPosYf;	
	int ballSize;		// Ballradius
	int ballSpeed;		// Ballgeschwindigkeit
	float ballAngleX;	// Winkel des Spielballvektors x-Achse
	float ballAngleY;	// Winkel des Spielballvektors y-Achse
	int signY;			// Vorzeichen des Spielballvektors y-Achse

	bool startGame;

	// Netzwerkverbindungen
	int portTX;
	int portRX;
	string ip;
};
