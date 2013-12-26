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

OFX_OBJLOADER_BEGIN_NAMESPACE

void load(string path, ofMesh& mesh, bool generateNormals, bool flipFace)
{
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

			if (m->colors)
			{
				GLfloat *c = m->colors + (tri.vindices[k] * 3);
				mesh.addColor(ofFloatColor(c[0], c[1], c[2]));
			}

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

void loadGroup(string path, map<string, ofMesh>& groups, bool generateNormals)
{
	path = ofToDataPath(path);

	groups.clear();

	GLMmodel* m;

	m = glmReadOBJ((char*)path.c_str());

	if (generateNormals)
	{
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

				if (m->colors)
				{
					GLfloat *c = m->colors + (tri.vindices[k] * 3);
					t.addColor(ofFloatColor(c[0], c[1], c[2]));
				}

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

void save(string path, const ofMesh& mesh_, bool flipFace, bool flipNormals, bool export_vertexcolor_to_texture)
{
	ofMesh mesh = mesh_;
	
	path = ofToDataPath(path);
	
	ofFilePath::createEnclosingDirectory(path);

	GLuint writeMode = GLM_NONE;
	GLMmodel* m = new GLMmodel();
	
	if (export_vertexcolor_to_texture)
	{
		if (mesh.getMode() != OF_PRIMITIVE_TRIANGLES)
		{
			ofLogError("ofxObjLoader::save") << "vertex color to texture supported only triangle primitive";
		}
		else
		{
			ofImage image;
			
			vertexColorToFaceColor(mesh);
			faceColorToTexture(mesh, image);
			
			ofFile file(path);
			string base_path = file.getEnclosingDirectory();
			string material_name = file.getBaseName();
			
			string image_name = material_name + ".png";
			ofPixels pix = image.getPixelsRef();
			
			// flip save texture
			pix.mirror(true, false);
			ofSaveImage(pix, ofFilePath::join(base_path, image_name));
			
			string mtl_filename = material_name + ".mtl";
			
			writeMode |= GLM_MATERIAL;
			m->mtllibname = (char*)malloc(mtl_filename.size());
			strcpy(m->mtllibname, mtl_filename.c_str());
			
			m->nummaterials = 1;
			m->materials = (GLMmaterial*)malloc(sizeof(GLMmaterial));
			GLMmaterial *mat = &m->materials[0];
			memset(mat, 0, sizeof(GLMmaterial));
			
			for (int i = 0; i < 4; i++)
			{
				mat->diffuse[i] = 1;
				mat->ambient[i] = 1;
				mat->specular[i] = 1;
				mat->emmissive[i] = 1;
			}
			mat->shininess = 1;
			
			mat->name = (char*)malloc(material_name.size());
			strcpy(mat->name, material_name.c_str());
			
			mat->texture_path = (char*)malloc(image_name.size());
			strcpy(mat->texture_path, image_name.c_str());
		}
	}

	if (mesh.getNumVertices() > 0)
	{
		m->numvertices = mesh.getNumVertices();
		m->vertices = new GLfloat[(m->numvertices + 1) * 3];
		memcpy(&m->vertices[3], &mesh.getVertices()[0].x, sizeof(ofVec3f) * mesh.getNumVertices());
	}
	else
	{
		ofLogError("ofxObjLoader::save -- No vertices to save!");
		return;
	}

	if (mesh.getNumNormals() > 0)
	{
		m->numnormals = mesh.getNumNormals();
		m->normals = new GLfloat[(m->numnormals + 1) * 3];
		vector<ofVec3f> normals = mesh.getNormals();
		
		if (flipNormals)
			for (int i = 0; i < normals.size(); i++)
				normals[i] *= -1;
		
		memcpy(&m->normals[3], &normals[0].x, sizeof(ofVec3f) * normals.size());
		writeMode |= GLM_SMOOTH;
	}

	if (mesh.getNumTexCoords() > 0)
	{
		m->numtexcoords = mesh.getNumTexCoords();
		m->texcoords = new GLfloat[(m->numtexcoords + 1) * 2];
		memcpy(&m->texcoords[2], &mesh.getTexCoords()[0].x, sizeof(ofVec2f) * mesh.getNumTexCoords());
		writeMode |= GLM_TEXTURE;
	}

	if (mesh.getNumIndices() > 0)
	{
		m->numtriangles = mesh.getNumIndices() / 3;
		m->triangles = new GLMtriangle[m->numtriangles];

		m->groups = new GLMgroup();
		m->groups->next = NULL;
		m->groups->material = NULL;
		
		string name = "ofMesh";
		m->groups->name = (char*)malloc(sizeof(char) * name.length() + 1);
		strcpy(m->groups->name, name.c_str());

		m->groups->numtriangles = mesh.getNumIndices() / 3;
		m->groups->triangles = new GLuint[m->groups->numtriangles];
		m->numgroups = 1;

		for (int i = 0; i < mesh.getNumIndices(); i += 3)
		{
			int idx = i / 3;
			for (int j = 0; j < 3; j++)
			{
				m->triangles[idx].vindices[j] = mesh.getIndices()[i + j] + 1;
				m->triangles[idx].nindices[j] = mesh.getIndices()[i + j] + 1;
				m->triangles[idx].tindices[j] = mesh.getIndices()[i + j] + 1;
			}
			m->groups->triangles[idx] = idx;
		}
	}
	else
	{
		m->numtriangles = mesh.getNumVertices() / 3;
		m->triangles = new GLMtriangle[m->numtriangles];

		m->groups = new GLMgroup();
		m->groups->next = NULL;
		m->groups->material = NULL;
		
		string name = "ofMesh";
		m->groups->name = (char*)malloc(sizeof(char) * name.length() + 1);
		strcpy(m->groups->name, name.c_str());

		m->groups->numtriangles = mesh.getNumVertices() / 3;
		m->groups->triangles = new GLuint[m->groups->numtriangles];
		m->numgroups = 1;

		for (int i = 0; i < mesh.getNumVertices(); i += 3)
		{
			int idx = i / 3;
			for (int j = 0; j < 3; j++)
			{
				m->triangles[idx].vindices[j] = i + j + 1;
				m->triangles[idx].nindices[j] = i + j + 1;
				m->triangles[idx].tindices[j] = i + j + 1;
			}
			m->groups->triangles[idx] = idx;
		}
	}
	
	if (flipFace)
		glmReverseWinding(m);

	glmWriteOBJ(m, (char*)path.c_str(), writeMode);
	glmDelete(m);
}

void saveGroup(string path, const vector<ofMesh> & meshGroup, bool flipFace, bool flipNormals) {
	path = ofToDataPath(path);
	
	ofFilePath::createEnclosingDirectory(path);
    
	GLuint writeMode = GLM_NONE;
	GLMmodel * m = new GLMmodel();
    GLMgroup * group = NULL;

    int numOfVerticesTotal = 0;
    int numOfNormalsTotal = 0;
    int numOfTexCoordsTotal = 0;
    int numOfIndicesTotal = 0;
    int numOfTrianglesTotal = 0;
    
    for(int i=0; i<meshGroup.size(); i++) {
        const ofMesh & mesh = meshGroup[i];
        
        numOfVerticesTotal += mesh.getNumVertices();
        numOfNormalsTotal += mesh.getNumNormals();
        numOfTexCoordsTotal += mesh.getNumTexCoords();

        if(mesh.getNumIndices() > 0) {
            numOfIndicesTotal += mesh.getNumIndices();
        } else {
            numOfIndicesTotal += mesh.getNumVertices();
        }
    }
    
    // glm skips the first array element when exporting.
    // not sure why?
    // but it means everything needs to be padded by 1.
    
    numOfVerticesTotal += 1;
    numOfNormalsTotal += 1;
    numOfTexCoordsTotal += 1;
    
    if(numOfVerticesTotal > 0) {
        m->numvertices = numOfVerticesTotal;
        m->vertices = new GLfloat[m->numvertices * 3];
    }
    
    if(numOfNormalsTotal > 0) {
        m->numnormals = numOfNormalsTotal;
        m->normals = new GLfloat[m->numnormals * 3];
    }
    
    if(numOfTexCoordsTotal > 0) {
        m->numtexcoords = numOfTexCoordsTotal;
        m->texcoords = new GLfloat[m->numtexcoords * 2];
    }
    
    if(numOfIndicesTotal > 0) {
        numOfTrianglesTotal = numOfIndicesTotal / 3;
        m->numtriangles = numOfTrianglesTotal;
        m->triangles = new GLMtriangle[m->numtriangles];
    }
    
    int numOfVertices = 0;
    int numOfNormals = 0;
    int numOfTexCoords = 0;
    int numOfIndices = 0;
    int numOfTriangles = 0;
    
    int indexVertices = 1;
    int indexNormals = 1;
    int indexTexCoords = 1;
    int indexIndices = 0;
    int indexTriangles = 0;
    
    for(int k=0; k<meshGroup.size(); k++) {
        const ofMesh & mesh = meshGroup[k];
        
        if(numOfVerticesTotal > 0) {
            numOfVertices = mesh.getNumVertices();
            memcpy(&m->vertices[indexVertices * 3], &mesh.getVertices()[0].x, sizeof(ofVec3f) * numOfVertices);
            indexVertices += numOfVertices;
        } else {
            ofLogError("ofxObjLoader::save -- No vertices to save!");
            return;
        }
        
        if(numOfNormalsTotal > 0) {
            numOfNormals = mesh.getNumNormals();
            vector<ofVec3f> normals = mesh.getNormals();
            
            if(flipNormals) {
                for(int i = 0; i < normals.size(); i++) {
                    normals[i] *= -1;
                }
            }
            
            memcpy(&m->normals[indexNormals * 3], &normals[0].x, sizeof(ofVec3f) * numOfNormals);
            indexNormals += numOfNormals;
            
            writeMode |= GLM_SMOOTH;
        }
        
        if(numOfTexCoordsTotal > 0) {
            numOfTexCoords = mesh.getNumTexCoords();
            memcpy(&m->texcoords[indexTexCoords * 2], &mesh.getTexCoords()[0].x, sizeof(ofVec2f) * numOfTexCoords);
            indexTexCoords += numOfTexCoords;

            writeMode |= GLM_TEXTURE;
        }
        
        numOfIndices = mesh.getNumIndices();
        if(numOfIndices > 0) {
            numOfTriangles = numOfIndices / 3;
        } else {
            numOfTriangles = numOfVertices / 3;
        }
        
        GLMgroup * groupPrev = group;
        group = new GLMgroup();
        group->next = NULL;
        group->material = NULL;
        
        if(m->groups == NULL) {
            m->groups = group;
            m->numgroups = 1;
        }
        if(groupPrev != NULL) {
            groupPrev->next = group;
            m->numgroups += 1;
        }
        
        string name = "ofMesh_" + ofToString(k);
        group->name = (char*)malloc(sizeof(char) * name.length() + 1);
        strcpy(group->name, name.c_str());
        
        group->numtriangles = numOfTriangles;
        group->triangles = new GLuint[group->numtriangles];
        
        const vector<ofIndexType> & indices = mesh.getIndices();
        int index = 0;
        
        for(int i=0; i<numOfTriangles; i++) {
            
            int t = i + indexTriangles;
            
            for(int j=0; j<3; j++) {
                
                int idx = i * 3 + j;
            
                if(numOfIndices > 0) {
                    index = indices[idx];
                } else {
                    index = idx;
                }
                
                index += (indexIndices + 1);
                
                m->triangles[t].vindices[j] = index;
                m->triangles[t].nindices[j] = index;
                m->triangles[t].tindices[j] = index;
            }
            
            group->triangles[i] = t;
        }
        
        indexIndices += numOfVertices;
        indexTriangles += numOfTriangles;
    }
	
	if(flipFace == true) {
		glmReverseWinding(m);
    }
    
	glmWriteOBJ(m, (char*)path.c_str(), writeMode);
	glmDelete(m);
}

void vertexColorToFaceColor(ofMesh& mesh)
{
	vector<ofFloatColor> face_color;
	vector<ofFloatColor> &color = mesh.getColors();
	
	for (int i = 0; i < color.size(); i += 3)
	{
		ofFloatColor c0 = color[i + 0];
		ofFloatColor c1 = color[i + 1];
		ofFloatColor c2 = color[i + 2];
		
		float r = (c0.r + c1.r + c2.r) / 3;
		float g = (c0.g + c1.g + c2.g) / 3;
		float b = (c0.b + c1.b + c2.b) / 3;
		ofFloatColor c(r, g, b, 1);
		
		face_color.push_back(c);
		face_color.push_back(c);
		face_color.push_back(c);
	}
	
	mesh.getColors() = face_color;
}

void faceColorToTexture(ofMesh& mesh, ofImage& image)
{
	vector<ofFloatColor> &color = mesh.getColors();
	int num_face = color.size() / 3;
	
	int tex_size = ofNextPow2(ceil(sqrt(num_face)));
	
	bool arb = ofGetUsingArbTex();
	ofDisableArbTex();
	image.allocate(tex_size, tex_size, OF_IMAGE_COLOR);
	if (arb) ofEnableArbTex();
	
	mesh.clearTexCoords();
	
	image.getPixelsRef().set(0);
	
	float texel_size = (1. / image.getWidth()) * 0.5;
	
	for (int i = 0; i < num_face; i++)
	{
		int u = (i % tex_size);
		int v = (i / tex_size);
		
		ofColor c = color[i * 3];
		
		image.setColor(u, v, c);
		
		float uu = (float)u / image.getWidth() + texel_size;
		float vv = (float)v / image.getHeight() + texel_size;
		
		mesh.addTexCoord(ofVec2f(uu, vv));
		mesh.addTexCoord(ofVec2f(uu, vv));
		mesh.addTexCoord(ofVec2f(uu, vv));
	}
	
	image.update();
	mesh.clearColors();
}

OFX_OBJLOADER_END_NAMESPACE