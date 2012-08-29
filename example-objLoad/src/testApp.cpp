#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    ofBackground(0);
    ofSetFrameRate(30);

	ofxObjLoader::load("sphere.obj", original, true);
    ofxObjLoader::save("sphere_saved.obj", original);
    ofxObjLoader::load("sphere_saved.obj", saved, false);

    cout << " num vertices " << original.getNumVertices() << " " <<  saved.getNumVertices() <<  endl;
    cout << " num texcords " << original.getNumTexCoords() << " " <<  saved.getNumTexCoords() << endl;
    cout << " num normals " << original.getNumNormals() << " " <<  saved.getNumNormals() << endl;
    cout << " num indices " << original.getNumIndices() << " " <<  saved.getNumIndices() << endl;

}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){
	cam.setPosition(original.getCentroid() + ofVec3f(0,0,mouseY/10));
    cam.lookAt(original.getCentroid(), ofVec3f(0,1,0));
    
    cam.begin();
    ofSetColor(255);
	if(ofGetMouseX() > ofGetWidth()/2){
		original.drawWireframe();
	}
	else{
		saved.drawWireframe();
	}
    cam.end();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}