#pragma once

#include "ofMain.h"

class ofxObjLoader
{
public:
	
	ofMesh mesh;
	const ofMesh& load(string path);
	
};