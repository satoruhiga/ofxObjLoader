#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    ofBackground(0);
    ofSetFrameRate(30);
    
	loader.load("loadTest.obj", mesh, true);
    
    cout << " num vertices " << mesh.getNumVertices() << endl;
    cout << " num tex coords " << mesh.getNumTexCoords() << endl;
    cout << " num normals " << mesh.getNumNormals() << endl;
    cout << " num indices " << mesh.getNumIndices() << endl;
    
    loader.save("loadtest_saved.obj", mesh);
    loader.load("loadtest_saved.obj", mesh, true);

    cout << " num vertices " << mesh.getNumVertices() << endl;
    cout << " num tex coords " << mesh.getNumTexCoords() << endl;
    cout << " num normals " << mesh.getNumNormals() << endl;
    cout << " num indices " << mesh.getNumIndices() << endl;

}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){
	cam.setPosition(mesh.getCentroid() + ofVec3f(0,0,mouseX));
    cam.lookAt(mesh.getCentroid(), ofVec3f(0,1,0));
    
    cam.begin();
    ofSetColor(255);
	mesh.drawWireframe();
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