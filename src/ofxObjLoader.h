#pragma once

#include "ofMain.h"

class ofxObjLoader
{
  public:
	void load(string path, ofMesh& mesh, bool generateNormals = true);
	void save(string path, ofMesh& mesh);

//    void generateNormals(ofMesh& mesh, float smoo);
};