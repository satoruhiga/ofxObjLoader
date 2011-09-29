#include "ofxObjLoader.h"

#include "glm.h"

const ofMesh& ofxObjLoader::load(string path)
{
	path = ofToString(path);
	
	GLMmodel* m;
	
	m = glmReadOBJ((char*)path.c_str());
	
	glmFacetNormals(m);
	glmVertexNormals(m, 90);
	
	for (int i = 0; i < m->numtriangles; i++)
	{
		GLMtriangle &t = m->triangles[i];
		
		GLfloat *v, *n, *c;
		
		v = &m->vertices[3 * t.vindices[0]];
		mesh.addVertex(ofVec3f(v[0], v[1], v[2]));
		v = &m->vertices[3 * t.vindices[1]];
		mesh.addVertex(ofVec3f(v[0], v[1], v[2]));
		v = &m->vertices[3 * t.vindices[2]];
		mesh.addVertex(ofVec3f(v[0], v[1], v[2]));
		
		if (m->numnormals > 0)
		{
			n = &m->normals[3 * t.nindices[0]];
			mesh.addNormal(ofVec3f(n[0], n[1], n[2]));
			n = &m->normals[3 * t.nindices[1]];
			mesh.addNormal(ofVec3f(n[0], n[1], n[2]));
			n = &m->normals[3 * t.nindices[2]];
			mesh.addNormal(ofVec3f(n[0], n[1], n[2]));
		}
		
		if (m->numtexcoords > 0)
		{
			c = &m->texcoords[2 * t.tindices[0]];
			mesh.addTexCoord(ofVec2f(c[0], c[1]));
			c = &m->texcoords[2 * t.tindices[1]];
			mesh.addTexCoord(ofVec2f(c[0], c[1]));
			c = &m->texcoords[2 * t.tindices[2]];
			mesh.addTexCoord(ofVec2f(c[0], c[1]));
		}
	}
	
	glmDelete(m);
	
	return mesh;
}