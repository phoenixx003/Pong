#include "ofApp.h"

using namespace ofxCv;

//--------------------------------------------------------------
void ofApp::setup() {
	ofSetWindowTitle("Pong");

	// GUI Setup
	//
	gui.setup();
	gui.add(threshold1.set("Threshold1", 20, 0, 255));
	gui.add(threshold2.set("Threshold2", 20, 0, 255));

	gui.add(hue1.set("Hue 1", 0, 0, 360));
	gui.add(sat1.set("Saturation 1", 0, 0, 255));
	gui.add(val1.set("Value 1", 0, 0, 255));

	gui.add(hue2.set("Hue 2", 0, 0, 360));
	gui.add(sat2.set("Saturation 2", 0, 0, 255));
	gui.add(val2.set("Value 1", 0, 0, 255));

	gui.add(toggleWebcam.set("Toggle webcam", true));
	gui.add(toggleContour.set("Toggle contours", true));

	//gui.add(trackHueSat.set("Track Hue/Saturation", true));

	// Webcam Setup
	webcam.listDevices();
	webcam.setDeviceID(0);
	webcam.setup(640, 480);

	// Contour Setup
	contour1.setMinAreaRadius(15);
	contour1.setMaxAreaRadius(50);
	contour2.setMinAreaRadius(15);
	contour2.setMaxAreaRadius(50);

	//Startposition Spieler 1 & 2
	posP1 = ofGetHeight() / 2 - playerHeight / 2;
	posP2 = ofGetHeight() / 2 - playerHeight / 2;

	//Spielergröße und Abstand zum Rand
	playerHeight = 150;
	playerWidth = 10;
	playerSpacing = 10;

	//Startposition Ball
	ballPosXf = ofGetWidth() / 2.0 - ballSize / 2.0;
	ballPosYf = ofGetHeight() / 2.0 - ballSize / 2.0;

	//Ballgröße
	ballSize = 10;
	ballSpeed = 10;
	ballAngleX = 1.0;
	ballAngleY = 1.0;
	signY = 1;

	// Variablen zum Ein-/Ausblenden
	toggleWebcam = true;
	toggleContour = true;

	srand((unsigned)time(NULL));
}

//--------------------------------------------------------------
void ofApp::update() {
	// Webcam Update
	webcam.update();
	webcam.getPixels().mirrorTo(pixel, false, true);

	image = pixel;

	//image = webcam.getPixels()

	// Farberkennung und Positionierung von P1
	color1.setHueAngle(hue1);
	color1.setSaturation(sat1);
	color1.setBrightness(val1);

	contour1.setTargetColor(color1);
	contour1.setThreshold(threshold1);
	contour1.findContours(image);
	for (int i = 0; i < contour1.size(); i++)
	{
		centerContour1 = contour1.getCenter(i);
	}
	posP1 = (centerContour1.y - playerHeight / 2.0) * 2.8;

	if (posP1 <= 0)
	{
		posP1 = 0;
	}

	if (posP1 >= ofGetWindowHeight() - playerHeight)
	{
		posP1 = ofGetWindowHeight() - playerHeight;
	}

	// Farberkennung und Positionierung von P2
	color2.setHueAngle(hue2);
	color2.setSaturation(sat2);
	color2.setBrightness(val2);

	contour2.setTargetColor(color2);
	contour2.setThreshold(threshold2);
	contour2.findContours(image);
	for (int i = 0; i < contour2.size(); i++)
	{
		centerContour2 = contour2.getCenter(i);
	}
	posP2 = (centerContour2.y - playerHeight / 2.0) * 2.8;

	if (posP2 <= 0)
	{
		posP2 = 0;
	}

	if (posP2 >= ofGetWindowHeight() - playerHeight)
	{
		posP2 = ofGetWindowHeight() - playerHeight;
	}

	// Ballphysik
	//
	// Ball trifft auf einen Spieler
	//
	if ((ballPosX == (playerSpacing + playerWidth)) && (ballPosY >= posP1) && (ballPosY <= (posP1 + playerHeight)))
	{
		signY = -signY;
		ballAngleY = signY * ((float)rand() / RAND_MAX);
		ballAngleX = -ballAngleX;

		ballSpeed = ballSpeed + 2;
	}
	//
	if ((ballPosX == (ofGetWidth() - playerSpacing - playerWidth - ballSize)) && (ballPosY >= posP2) && (ballPosY <= (posP2 + playerHeight)))
	{
		signY = -signY;
		ballAngleY = signY * ((float)rand() / RAND_MAX);
		ballAngleX = -ballAngleX;

		ballSpeed = ballSpeed + 2;
	}
	//
	// Ball geht ins aus
	//
	// Spieler 1 hat verloren
	if ((ballPosX == (playerSpacing + playerWidth)) && !((ballPosY >= posP1) && (ballPosY <= (posP1 + playerHeight))))
	{
		// GameOver
		ballPosXf = ofGetWidth() / 2.0 - ballSize / 2.0;
		ballPosYf = ofGetHeight() / 2.0 - ballSize / 2.0;

		ballSpeed = 10;
	}
	// Spieler 2 hat verloren
	if ((ballPosX == (ofGetWidth() - playerSpacing - playerWidth - ballSize)) && !((ballPosY >= posP2) && (ballPosY <= (posP2 + playerHeight))))
	{
		// Gameover
		ballPosXf = ofGetWidth() / 2.0 - ballSize / 2.0;
		ballPosYf = ofGetHeight() / 2.0 - ballSize / 2.0;

		ballSpeed = 10;
	}
	//
	// Ball trifft auf oberen/unteren Rand
	if (ballPosY >= ofGetHeight() - ballSize)
	{
		ballAngleY = -ballAngleY;
	}

	if (ballPosY <= 0)
	{
		ballAngleY = -ballAngleY;
	}
	//
	// Ballposition zuweisen
	ballPosXf = ballPosXf + ballSpeed * ballAngleX;
	ballPosYf = ballPosYf + ballSpeed * ballAngleY;
	//
	ballPosX = ballPosXf;
	ballPosY = ballPosYf;
	//
	// X-Korrektur
	if (ballPosX < playerSpacing + playerWidth)
	{
		ballPosX = playerSpacing + playerWidth;
	}

	if (ballPosX > ofGetWidth() - playerSpacing - playerWidth - ballSize)
	{
		ballPosX = ofGetWidth() - playerSpacing - playerWidth - ballSize;
	}
	//
	// Y-Korrektur
	if (ballPosY < 0)
	{
		ballPosY = 0;
	}

	if (ballPosY > ofGetHeight() - ballSize)
	{
		ballPosY = ofGetHeight() - ballSize;
	}
	//__
}

//--------------------------------------------------------------
void ofApp::draw() {
	if (toggleWebcam)
	{
		image.draw(0, 0);
	}

	if (toggleContour)
	{
		contour1.draw();
		contour2.draw();
	}
	if (toggleGui)
	{
		gui.draw();
	}

	ofSetColor(color1);
	ofDrawRectangle(playerSpacing, posP1, playerWidth, playerHeight);

	ofSetColor(color2);
	ofDrawRectangle(ofGetWidth() - playerWidth - playerSpacing, posP2, playerWidth, playerHeight);

	ofSetColor(ofColor::white);
	ofDrawRectangle(ballPosX, ballPosY, ballSize, ballSize);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
	if (key == '1')
	{
		colorPick = image.getPixels().getColor(ofGetMouseX(), ofGetMouseY());
		hue1 = colorPick.getHueAngle(),
			sat1 = colorPick.getSaturation();
		val1 = colorPick.getBrightness();
	}
	if (key == '2')
	{
		colorPick = image.getPixels().getColor(ofGetMouseX(), ofGetMouseY());
		hue2 = colorPick.getHueAngle(),
			sat2 = colorPick.getSaturation();
		val2 = colorPick.getBrightness();
	}
	if (key == 'w')
	{
		toggleWebcam = !toggleWebcam;
	}
	if (key == 'c')
	{
		toggleContour = !toggleContour;
	}
	if (key == 'g')
	{
		toggleGui = !toggleGui;
	}
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}
