#include "ofMain.h"

#include "ofxObjLoader.h"

class ofApp : public ofBaseApp
{
public:
	
	ofMesh mesh;
	
	void setup()
	{
		ofSetFrameRate(60);
		ofSetVerticalSync(true);
		ofBackground(0);
		
		float s = 400;
		for (int i = 0; i < 100; i++)
		{
			ofColor c = ofColor::fromHsb(ofRandom(255), 255, 255);
			
			mesh.addColor(c);
			mesh.addColor(c);
			mesh.addColor(c);
			
			mesh.addVertex(ofVec3f(ofRandom(-s, s),
								   ofRandom(-s, s),
								   ofRandom(-s, s)));
			
			mesh.addVertex(ofVec3f(ofRandom(-s, s),
								   ofRandom(-s, s),
								   ofRandom(-s, s)));
			
			mesh.addVertex(ofVec3f(ofRandom(-s, s),
								   ofRandom(-s, s),
								   ofRandom(-s, s)));
		}
		
		ofxObjLoader::save("test.obj", mesh, false, false, true);

	}
	
	void update()
	{
	}
	
	ofEasyCam cam;
	void draw()
	{
		ofEnableDepthTest();
		
		cam.begin();
		mesh.draw();
		cam.end();
	}

	void keyPressed(int key)
	{
	}

	void keyReleased(int key)
	{
	}
	
	void mouseMoved(int x, int y)
	{
	}

	void mouseDragged(int x, int y, int button)
	{
	}

	void mousePressed(int x, int y, int button)
	{
	}

	void mouseReleased(int x, int y, int button)
	{
	}
	
	void windowResized(int w, int h)
	{
	}
};


int main(int argc, const char** argv)
{
	ofSetupOpenGL(1280, 720, OF_WINDOW);
	ofRunApp(new ofApp);
	return 0;
}
