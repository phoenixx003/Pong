#include "ofApp.h"

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

	gui.add(connection.set("Connection", false));

	// Webcam Setup
	webcam.listDevices();
	webcam.setDeviceID(0);
	webcam.setup(640, 480);  // Auflösung der Webcam

	// Contour Setup
	contour1.setMinAreaRadius(15);
	contour1.setMaxAreaRadius(50);
	contour2.setMinAreaRadius(15);
	contour2.setMaxAreaRadius(50);

	// OSC Setup
	// 
	// Ziel IP- und Port-Adresse
	ip = "172.20.10.2";
	portTX = 12345;
	// Eigener Port zum Empfangen 
	portRX = 12346;
	//
	oscSender.setup(ip, portTX);
	oscReceiver.setup(portRX);
	connection = false;

	// Startposition Spieler 1 & 2
	posP1 = ofGetHeight() / 2 - playerHeight / 2;
	posP2 = ofGetHeight() / 2 - playerHeight / 2;

	// Spielergröße und Abstand zum Rand
	playerHeight = 150;
	playerWidth = 10;
	playerSpacing = 10;

	// Startposition Ball
	ballPosXf = ofGetWidth() / 2.0 - ballSize / 2.0;
	ballPosYf = ofGetHeight() / 2.0 - ballSize / 2.0;

	// Ballgröße
	ballSize = 10;
	ballSpeed = 10;

	// Ballwinkel
	ballAngleX = 1.0;
	ballAngleY = 1.0;

	// Vorzeichen Ballvektor
	signY = 1;

	// Variable zum Bewegen des Balls
	startGame = false;

	// Variablen zum Ein-/Ausblenden
	toggleWebcam = true;
	toggleContour = true;
	toggleGui = true;

	srand((unsigned)time(NULL));
}

//--------------------------------------------------------------
void ofApp::update() {
	// Webcam Update
	//
	webcam.update();					
	webcam.getPixels().mirrorTo(pixel, false, true);
	image = pixel;

	// Farberkennung und Positionierung von P1
	//
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
	//
	if (!connection) //Dieser Abschnitt wird nur dann benutzt, wenn keine Verbindung zu einem anderen Spieler besteht
	{
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
	}

	// Ballphysik
	//
	// Ball trifft auf einen Spieler
	//
	// Ball trifft auf Spieler 1
	if ((ballPosX == (playerSpacing + playerWidth)) && (ballPosY >= posP1) && (ballPosY <= (posP1 + playerHeight)))
	{
		signY = -signY;
		ballAngleY = signY * ((float)rand() / RAND_MAX);
		ballAngleX = -ballAngleX;

		ballSpeed = ballSpeed ++;
	}
	// Ball trifft auf Spieler 2
	if ((ballPosX == (ofGetWidth() - playerSpacing - playerWidth - ballSize)) && (ballPosY >= posP2) && (ballPosY <= (posP2 + playerHeight)))
	{
		signY = -signY;
		ballAngleY = signY * ((float)rand() / RAND_MAX);
		ballAngleX = -ballAngleX;

		ballSpeed = ballSpeed ++;
	}
	//
	// Ball geht ins aus
	//
	// Spieler 1 GameOver
	if ((ballPosX == (playerSpacing + playerWidth)) && !((ballPosY >= posP1) && (ballPosY <= (posP1 + playerHeight))))
	{
		// GameOver
		ballPosXf = ofGetWidth() / 2.0 - ballSize / 2.0;
		ballPosYf = ofGetHeight() / 2.0 - ballSize / 2.0;

		ballSpeed = 10;
	}
	// Spieler 2 GameOver
	if ((ballPosX == (ofGetWidth() - playerSpacing - playerWidth - ballSize)) && !((ballPosY >= posP2) && (ballPosY <= (posP2 + playerHeight))))
	{
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
	if (startGame)
	{
		ballPosXf = ballPosXf + ballSpeed * ballAngleX;
		ballPosYf = ballPosYf + ballSpeed * ballAngleY;
	}
	else
	{
		ballPosXf = ofGetWidth() / 2.0 - ballSize / 2.0;
		ballPosYf = ofGetHeight() / 2.0 - ballSize / 2.0;
	}
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
	// Osc Nachricht Senden/Empfangen
	// Senden
	oscMessageTX.clear();

	oscMessageTX.setAddress("/playerMusa");
	// Sende eigene Position
	oscMessageTX.addIntArg(posP1);
	// Sende Position vom Ball
	oscMessageTX.addIntArg(ballPosX);
	oscMessageTX.addIntArg(ballPosY);

	oscSender.sendMessage(oscMessageTX, false);
	// Empfangen
	while (oscReceiver.hasWaitingMessages())
	{
		oscReceiver.getNextMessage(oscMessageRX);
		if (oscMessageRX.getAddress() == "/playerFerhat")
		{
			connection = true;
			posP2 = oscMessageRX.getArgAsInt(0);
		}
		else
		{
			connection = false;
		}
	}
}

//--------------------------------------------------------------
void ofApp::draw() {
	if (toggleWebcam)
	{
		image.draw(0, 0);
	}

	if (toggleGui)
	{
		gui.draw();
	}

	if (toggleContour)
	{
		contour1.draw();
		contour2.draw();
	}

	ofSetColor(color1);
	ofDrawRectangle(playerSpacing, posP1, playerWidth, playerHeight);

	ofSetColor(color2);
	ofDrawRectangle(ofGetWidth() - playerWidth - playerSpacing, posP2, playerWidth, playerHeight);

	ofSetColor(ofColor::white);
	ofDrawRectangle(ballPosX, ballPosY, ballSize, ballSize);
	//ofDrawCircle(ballPosX, ballPosY, ballSize);  // runder Ball
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
	if (key == ' ')
	{
		startGame = !startGame;
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
