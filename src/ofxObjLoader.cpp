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

#include "ofxObjLoader.h"
#include "glm.h"

void ofxObjLoader::load(string path, ofMesh& mesh, bool generateNormals, bool flipFace) {
	path = ofToDataPath(path);
    
    mesh.clear();
    
	GLMmodel* m;
	
	m = glmReadOBJ((char*)path.c_str());
	
    if (generateNormals)
	{
        glmFacetNormals(m);
		glmVertexNormals(m, 90);
    }
	
	if (flipFace)
	{
		glmReverseWinding(m);
	}
	
	for (int j = 0; j < m->numtriangles; j++)
	{
		const GLMtriangle &tri = m->triangles[j];
		
		for (int k = 0; k < 3; k++)
		{
			GLfloat *v = m->vertices + (tri.vindices[k] * 3);
			mesh.addVertex(ofVec3f(v[0], v[1], v[2]));
			
			if (m->normals && ofInRange(tri.nindices[k], 0, m->numnormals))
			{
				GLfloat *n = m->normals + (tri.nindices[k] * 3);
				mesh.addNormal(ofVec3f(n[0], n[1], n[2]));
			}
			
			if (m->texcoords && ofInRange(tri.tindices[k], 0, m->numtexcoords))
			{
				GLfloat *c = m->texcoords + (tri.tindices[k] * 2);
				mesh.addTexCoord(ofVec2f(c[0], c[1]));
			}
		}
	}

	glmDelete(m);
}

void ofxObjLoader::loadGroup(string path, map<string, ofMesh>& groups, bool generateNormals)
{
	path = ofToDataPath(path);
    
    groups.clear();
    
	GLMmodel* m;
	
	m = glmReadOBJ((char*)path.c_str());
	
    if(generateNormals){
        glmFacetNormals(m);
    }
	
	glmReverseWinding(m);

	GLMgroup *g = m->groups;
	
	while (g)
	{
		string name = g->name;
		
		ofMesh t;
		GLMtriangle *p = m->triangles;
		
		for (int j = 0; j < g->numtriangles; j++)
		{
			GLMtriangle tri = p[g->triangles[j]];
			
			for (int k = 0; k < 3; k++)
			{
				GLfloat *v = m->vertices + (tri.vindices[k] * 3);
				t.addVertex(ofVec3f(v[0], v[1], v[2]));
				
				if (m->normals && ofInRange(tri.nindices[k], 0, m->numnormals))
				{
					GLfloat *n = m->normals + (tri.nindices[k] * 3);
					t.addNormal(ofVec3f(n[0], n[1], n[2]));
				}
				
				if (m->texcoords && ofInRange(tri.tindices[k], 0, m->numtexcoords))
				{
					GLfloat *c = m->texcoords + (tri.tindices[k] * 2);
					t.addTexCoord(ofVec2f(c[0], c[1]));
				}
			}
		}

		groups[name] = t;
		g = g->next;
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
	    m->vertices = new GLfloat[(m->numvertices+1)*3];
        memcpy(&m->vertices[3], &mesh.getVertices()[0].x, sizeof(ofVec3f) * mesh.getNumVertices());
    }
    else {
        ofLogError("ofxObjLoader::save -- No vertices to save!");
        return;
    }

    if(mesh.getNumNormals() > 0){
        m->numnormals = mesh.getNumNormals();
        m->normals = new GLfloat[(m->numnormals+1)*3];
        memcpy(&m->normals[3], &mesh.getNormals()[0].x, sizeof(ofVec3f)*mesh.getNumNormals());
        writeMode |= GLM_SMOOTH;
    }
	
    if(mesh.getNumTexCoords() > 0){
        m->numtexcoords = mesh.getNumTexCoords();
        m->texcoords = new GLfloat[(m->numtexcoords+1)*2];
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
        for(int i = 0; i < mesh.getNumIndices(); i+=3){
			for(int j = 0; j < 3; j++){
				m->triangles[i/3].vindices[j] = mesh.getIndices()[i+j]+1;
				m->triangles[i/3].nindices[j] = mesh.getIndices()[i+j]+1;
				m->triangles[i/3].tindices[j] = mesh.getIndices()[i+j]+1;
			}
            m->groups->triangles[i/3] = i/3;
        }
    }
    
	
    glmWriteOBJ(m, (char*)path.c_str(), writeMode);
    glmDelete(m);
}