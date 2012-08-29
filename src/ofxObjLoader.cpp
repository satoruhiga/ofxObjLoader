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
 * Improvements by James George ( http://jamesgeorge.org/ )
 *
 * Example sphere from: http://liszt.stanford.edu/meshes/sphere.obj
 */

#include "ofxObjLoader.h"
#include "glm.h"

void ofxObjLoader::load(string path, ofMesh& mesh, bool generateNormals) {
	path = ofToDataPath(path);
    
    mesh.clear();
    
	GLMmodel* m;
	
	m = glmReadOBJ((char*)path.c_str());
	
    if(generateNormals){
        glmFacetNormals(m);
        glmVertexNormals(m, 90);
    }
    
    GLfloat *v, *n, *c;
    for(int i = 1; i <= m->numvertices; i++){
        v = &m->vertices[3 * i];
		mesh.addVertex(ofVec3f(v[0], v[1], v[2]));
    }
    
	for(int i = 1; i <= m->numnormals; i++){
        n = &m->normals[3 * i];
        mesh.addNormal(ofVec3f(n[0], n[1], n[2]));
    }
    
    for(int i = 1; i <= m->numtexcoords; i++){
        c = &m->texcoords[2 * i];
        mesh.addTexCoord(ofVec2f(c[0], c[1]));
    }
    
	for (int i = 0; i < m->numtriangles; i++) {
		GLMtriangle &t = m->triangles[i];
        
        //NOTE: ofMesh does not have support for different indices for tex coords and mormals
		mesh.addIndex(t.vindices[0]);
        mesh.addIndex(t.vindices[1]);
        mesh.addIndex(t.vindices[2]);
	}
	
	glmDelete(m);
}

void ofxObjLoader::save(string path, ofMesh& mesh){
    path = ofToDataPath(path);
    
    GLuint writeMode = GLM_NONE;
//    cout << "saving mesh verts: " << mesh.getNumVertices() << " norms " << mesh.getNumNormals() << " tex coords" << mesh.getNumTexCoords() << " indeces " << mesh.getNumIndices() << endl;
    GLMmodel* m = new GLMmodel();
    if(mesh.getNumVertices() > 0){
        m->numvertices = mesh.getNumVertices();
	    m->vertices = new GLfloat[m->numvertices*3+1];
        memcpy(&m->vertices[3], &mesh.getVertices()[0].x, sizeof(ofVec3f) * mesh.getNumVertices());
    }
    else {
        ofLogError("ofxObjLoader::save -- No vertices to save!");
        return;
    }

    if(mesh.getNumNormals() > 0){
        m->numnormals = mesh.getNumNormals();
        m->normals = new GLfloat[m->numnormals*3+1];
        memcpy(&m->normals[3], &mesh.getNormals()[0].x, sizeof(ofVec3f)*mesh.getNumNormals());
        writeMode |= GLM_SMOOTH;
    }
	
    if(mesh.getNumTexCoords() > 0){
        m->numtexcoords = mesh.getNumTexCoords();
        m->texcoords = new GLfloat[m->numtexcoords*2+1];
        memcpy(&m->texcoords[2], &mesh.getTexCoords()[0].x, sizeof(ofVec2f)*mesh.getNumTexCoords());
        writeMode |= GLM_TEXTURE;
    }
    
    if(mesh.getNumIndices() > 0){
        //create triangles
		m->numtriangles = mesh.getNumIndices()/3;
        m->triangles = new GLMtriangle[m->numtriangles];
        
        //add them all to one group
        m->groups = new GLMgroup();
        m->groups->next = NULL;
        m->groups->material = NULL;
        string name = "ofMesh";
        m->groups->name = (char*)malloc(sizeof(char) * name.length()+1);
        strcpy(m->groups->name, name.c_str());
        
        m->groups->numtriangles = mesh.getNumIndices()/3;
		m->groups->triangles = new GLuint[m->groups->numtriangles];
        m->numgroups = 1;
        for(int i = 0; i < mesh.getNumIndices()/3; i++){
            memcpy(m->triangles[i].vindices, &mesh.getIndices()[i*3], sizeof(GLuint)*3);
            memcpy(m->triangles[i].nindices, &mesh.getIndices()[i*3], sizeof(GLuint)*3);
            memcpy(m->triangles[i].tindices, &mesh.getIndices()[i*3], sizeof(GLuint)*3);
            m->groups->triangles[i] = i;
        }
    }
    
    glmWriteOBJ(m, (char*)path.c_str(), writeMode);
    glmDelete(m);
}