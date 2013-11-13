/**
 * Copyright (C) <year> <copyright holders>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Wavefront OBJ model file format reader/writer/manipulator for openFrameworks
 *
 * GLM library by Nate Robins, 1997
 * ndr@pobox.com, http://www.pobox.com/~ndr/
 *
 * Addon by Satoru Higa ( http://structor.jp/ )
 * Improvements by James George ( http://jamesgeorge.org/ ) at YCAM InterLab ( http://interlab.ycam.jp/ )
 *
 * Example sphere from: http://liszt.stanford.edu/meshes/sphere.obj
 */

#pragma once

#include "ofMain.h"

#define OFX_OBJLOADER_BEGIN_NAMESPACE namespace ofx { namespace ObjLoader {
#define OFX_OBJLOADER_END_NAMESPACE } }

OFX_OBJLOADER_BEGIN_NAMESPACE

void load(string path, ofMesh& mesh, bool generateNormals = true, bool flipFace = false);
void save(string path, const ofMesh& mesh, bool flipFace = false, bool flipNormals = false, bool export_vertexcolor_to_texture = false);

void loadGroup(string path, map<string, ofMesh>& groups, bool generateNormals = true);

// utils
void vertexColorToFaceColor(ofMesh& mesh);
void faceColorToTexture(ofMesh& mesh, ofImage& image);

OFX_OBJLOADER_END_NAMESPACE

namespace ofxObjLoader = ofx::ObjLoader;