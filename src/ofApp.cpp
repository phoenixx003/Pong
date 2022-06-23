#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	ofSetWindowTitle("Pong");

	// Setze die Framerate auf 30 FPS
	ofSetFrameRate(30);
	ofSetVerticalSync(true);

	// GUI Setup
	//
	gui.setup();
	gui.setSize(300, 0);
	gui.setPosition(641, 0);
	gui.setName("GUI");
	gui.add(threshold1.set("Threshold1", 20, 0, 255));
	gui.add(threshold2.set("Threshold2", 20, 0, 255));
	gui.add(minRadius.set("Contour Minimum Radius", 5, 0, 640));
	gui.add(maxRadius.set("Contour Maximum Radius", 640, 0, 640));
	
	gui.add(hue1.set("Hue 1", 0, 0, 360));
	gui.add(sat1.set("Saturation 1", 0, 0, 255));
	gui.add(val1.set("Value 1", 0, 0, 255));

	gui.add(hue2.set("Hue 2", 0, 0, 360));
	gui.add(sat2.set("Saturation 2", 0, 0, 255));
	gui.add(val2.set("Value 1", 0, 0, 255));

	gui.add(toggleWebcam.set("Toggle webcam", true));
	gui.add(toggleContour.set("Toggle contours", true));

	gui.add(master.set("Master", true));
	labelConnection = "No Connection";
	gui.add(labelConnection.setup("Connection Status", labelConnection));
	
	gui.add(ip.set("IP: ", ""));
	gui.add(portTX.set("Port zum Senden: ", ""));
	gui.add(portRX.set("Port zum Empfangen: ", ""));
	gui.add(myAdress.set("My Adress: ", ""));
	gui.add(opponentAdress.set("Opponent Adress: ", ""));

	// Webcam Setup
	webcam.listDevices();
	webcam.setDeviceID(0);
	webcam.setup(640, 480);  // Auflösung der Webcam

	// Contour Setup
	contour1.setMinAreaRadius(minRadius);
	contour1.setMaxAreaRadius(maxRadius);
	contour2.setMinAreaRadius(minRadius);
	contour2.setMaxAreaRadius(maxRadius);

	// OSC Setup
	// 
	// Ziel IP, Port und Addresse zum Senden
	ip = "172.20.10.3";
	portTX = "100";
	opponentAdress = "/playerB";
	// Eigener Port und Addresse zum Empfangen
	portRX = "101";
	myAdress = "/playerA";
	//
	oscSender.setup(ip, ofToInt(portTX));
	oscReceiver.setup(ofToInt(portRX));
	connection = false;
	//___
	// 
	// Startposition Spieler 1 & 2
	posP1 = FHD_HEIGHT / 2 - playerHeight / 2;
	posP2 = FHD_HEIGHT / 2 - playerHeight / 2;

	// Spielergröße und Abstand zum Rand
	playerHeight = 150;
	playerWidth = 10;
	playerSpacing = 10;

	// Startposition Ball
	ballPosXf = FHD_WIDTH / 2.0 - ballSize / 2.0;
	ballPosYf = FHD_HEIGHT / 2.0 - ballSize / 2.0;

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

	// Timer Setup
	timeToWait = 5.0;     // 5 Sekunden
}

//--------------------------------------------------------------
void ofApp::update() {
	// Webcam Update
	//
	webcam.update();
	webcam.getPixels().mirrorTo(pixel, false, true);
	image = pixel;

	// Berechnung der Normalen
	//
	widthNorm = ofGetWidth() / FHD_WIDTH;
	heightNorm = ofGetHeight() / FHD_HEIGHT;

	// Farberkennung und Positionierung von P1
	//
	contour1.setMinAreaRadius(minRadius);
	contour1.setMaxAreaRadius(maxRadius);
	contour2.setMinAreaRadius(minRadius);
	contour2.setMaxAreaRadius(maxRadius);

	color1.setHueAngle(hue1);
	color1.setSaturation(sat1);
	color1.setBrightness(val1);

	contour1.setTargetColor(color1);
	contour1.setThreshold(threshold1);
	contour1.findContours(image);

	// magical For-Loop
	for (int i = 0; i < contour1.size(); i++)
	{
		centerContour1 = contour1.getCenter(i);
	}
	// Zuweisung Position von Player1 anhang der Mitte der gefundenen Kontur mit zusätzlicher Skalierung von WebcamBild auf Fenstergröße
	posP1 = (centerContour1.y - playerHeight / 2.0) * FHD_HEIGHT / 480.0;

	if (posP1 <= 0)
	{
		posP1 = 0;
	}

	if (posP1 >= FHD_HEIGHT - playerHeight)
	{
		posP1 = FHD_HEIGHT - playerHeight;
	}

	// Farberkennung und Positionierung von P2
	//
	//Dieser Abschnitt wird nur dann benutzt, wenn keine Verbindung zu einem anderen Spieler besteht
	if (!connection)
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
		posP2 = (centerContour2.y - playerHeight / 2.0) * FHD_HEIGHT / 480.0;

		if (posP2 <= 0)
		{
			posP2 = 0;
		}

		if (posP2 >= FHD_HEIGHT - playerHeight)
		{
			posP2 = FHD_HEIGHT - playerHeight;
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

		ballSpeed = ballSpeed++;
	}

	// Ball trifft auf Spieler 2
	if ((ballPosX == (FHD_WIDTH - playerSpacing - playerWidth - ballSize)) && (ballPosY >= posP2) && (ballPosY <= (posP2 + playerHeight)))
	{
		signY = -signY;
		ballAngleY = signY * ((float)rand() / RAND_MAX);
		ballAngleX = -ballAngleX;

		ballSpeed = ballSpeed++;
	}
	//
	// Ball geht ins aus
	//
	// Spieler 1 GameOver
	if ((ballPosX == (playerSpacing + playerWidth)) && !((ballPosY >= posP1) && (ballPosY <= (posP1 + playerHeight))))
	{
		// GameOver
		ballPosXf = FHD_WIDTH / 2.0 - ballSize / 2.0;
		ballPosYf = FHD_HEIGHT / 2.0 - ballSize / 2.0;

		ballSpeed = 10;
	}
	// Spieler 2 GameOver
	if ((ballPosX == (FHD_WIDTH - playerSpacing - playerWidth - ballSize)) && !((ballPosY >= posP2) && (ballPosY <= (posP2 + playerHeight))))
	{
		ballPosXf = FHD_WIDTH / 2.0 - ballSize / 2.0;
		ballPosYf = FHD_HEIGHT / 2.0 - ballSize / 2.0;

		ballSpeed = 10;
	}
	//
	// Ball trifft auf oberen/unteren Rand
	if (ballPosY >= FHD_HEIGHT - ballSize)
	{
		ballAngleY = -ballAngleY;
	}

	if (ballPosY <= 0)
	{
		ballAngleY = -ballAngleY;
	}
	//
	// Ballposition zuweisen
	if ((startGame && !connection) || (startGame && connection && master))
	{
		ballPosXf = ballPosXf + ballSpeed * ballAngleX;
		ballPosYf = ballPosYf + ballSpeed * ballAngleY;
		ballPosX = ballPosXf;
		ballPosY = ballPosYf;
	}
	else if ((!startGame && connection && master) || (!startGame && !connection))
	{
		ballPosXf = FHD_WIDTH / 2.0 - ballSize / 2.0;
		ballPosYf = FHD_HEIGHT / 2.0 - ballSize / 2.0;
		ballPosX = ballPosXf;
		ballPosY = ballPosYf;
	}
	//
	//
	// X-Korrektur
	if (ballPosX < playerSpacing + playerWidth)
	{
		ballPosX = playerSpacing + playerWidth;
	}

	if (ballPosX > FHD_WIDTH - playerSpacing - playerWidth - ballSize)
	{
		ballPosX = FHD_WIDTH - playerSpacing - playerWidth - ballSize;
	}
	//
	// Y-Korrektur
	if (ballPosY < 0)
	{
		ballPosY = 0;
	}

	if (ballPosY > FHD_HEIGHT - ballSize)
	{
		ballPosY = FHD_HEIGHT - ballSize;
	}
	//__
	// Osc Nachricht Senden/Empfangen
	// Senden
	//
	// Update Sender und Receiver wenn sich Daten ändern
	if ((ofToString(ipAlt) != ofToString(ip)) || (ofToString(portTXalt) != ofToString(portTX)))
	{
		oscSender.setup(ip, ofToInt(portTX));
	}
	ipAlt = ip;
	portTXalt = portTX;
	if (ofToString(portRX) != ofToString(portRXalt))
	{
		oscReceiver.setup(ofToInt(portRX));
	}
	portRXalt = portRX;
	// Lösche alle Inhalte der Nachricht
	oscMessageTX.clear();
	// Setze eigene Adresse, welche der Gegner als oscMessageRX.getAdress(" ") benutzen muss 
	oscMessageTX.setAddress(myAdress);
	// Füge eigene Position zur Nachricht hinzu
	oscMessageTX.addIntArg(posP1);

	// Füge Position vom Ball zur Nachricht hinzu wenn Master
	if (master)
	{
		oscMessageTX.addIntArg(ballPosX);
		oscMessageTX.addIntArg(ballPosY);
	}

	// Füge Farbe vom P1 zur Nachricht hinzu
	oscMessageTX.addIntArg(color1.getHueAngle());
	oscMessageTX.addIntArg(color1.getSaturation());
	oscMessageTX.addIntArg(color1.getBrightness());

	// Sende Nachricht
	oscSender.sendMessage(oscMessageTX, true);

	// Empfangen
	if (oscReceiver.hasWaitingMessages())
	{
		labelConnection = "Connected!";
		connection = true;
		ofResetElapsedTimeCounter();
		timer = ofGetElapsedTimef();
	}
	else if ((oscReceiver.hasWaitingMessages() == false) && (timer >= timeToWait))
	{
		labelConnection = "No Connection";
		connection = false;
	}
	else if (!oscReceiver.hasWaitingMessages())
	{
		timer = ofGetElapsedTimef();
	}

	while (oscReceiver.hasWaitingMessages())
	{
		oscReceiver.getNextMessage(oscMessageRX);
		// Adresse vom Gegner einsetzen, welcher in oscMessageTX.setAdress(" ") benutzt wurde
		if (oscMessageRX.getAddress() == ofToString(opponentAdress))
		{
			posP2 = oscMessageRX.getArgAsInt(0);  // Position von P2 zuweisen

			if (!master)
			{
				ballPosX = FHD_WIDTH - oscMessageRX.getArgAsInt(1); // Position des Balls zuweisen (X-Achse)
				ballPosY = oscMessageRX.getArgAsInt(2); // Position des Balls zuweisen (Y-Achse)
			}

			// Farbe des Gegners zuweisen
			if (master)
			{
				color2.setHueAngle(oscMessageRX.getArgAsInt(1));
				color2.setSaturation(oscMessageRX.getArgAsInt(2));
				color2.setBrightness(oscMessageRX.getArgAsInt(3));
			}
			else
			{
				color2.setHueAngle(oscMessageRX.getArgAsInt(3));
				color2.setSaturation(oscMessageRX.getArgAsInt(4));
				color2.setBrightness(oscMessageRX.getArgAsInt(5));
			}
		}
	}
}

//--------------------------------------------------------------
void ofApp::draw() {
	if (toggleWebcam)
	{
		image.draw(0,0);
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
	// Zeichne Player 1
	ofSetColor(color1);
	ofDrawRectangle((playerSpacing * widthNorm), (posP1 * heightNorm), (playerWidth * widthNorm), (playerHeight * heightNorm));

	// Zeichne Player
	ofSetColor(color2);
	ofDrawRectangle((FHD_WIDTH * widthNorm) - (playerWidth * widthNorm) - (playerSpacing * widthNorm), (posP2 * heightNorm), (playerWidth * widthNorm), (playerHeight * heightNorm));

	// Zeichne Ball
	ofSetColor(ofColor::white);
	ofDrawRectangle((ballPosX * widthNorm), (ballPosY * heightNorm), (ballSize * widthNorm), (ballSize * heightNorm));

	// Zeichne Framerate
	ofDrawBitmapString("FPS: "+ofToString(ofGetFrameRate()), 10, 10);
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