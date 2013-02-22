//
//  textureRender.cpp
//  CinderES2
//
//  Created by Kris Meeusen on 15/02/13.
//  Copyright (c) 2013 Kris Meeusen. All rights reserved.
//

#include "textureRender.h"
#include "shaderLoader.h"
#include "checkError.h"

#include "cinder/ImageIo.h"
#include "cinder/app/AppBasic.h"


using namespace ci;

textureRender* textureRender::instance;


textureRender* textureRender::Instance(){
    if(instance == NULL)
	{
		instance = new textureRender();
		instance->setup();
	}
    return instance;
}


textureRender::textureRender(){
	isSetup = false;
}


void textureRender::setup(){
	const char vShaderStr[] =
    "attribute vec4 position;   \n"
	"attribute vec3 uv;			\n"

	"uniform mat4 modelViewProjectionMatrix;"
	"uniform mat3 normalMatrix;"
	"varying vec3 uvVarying;			\n"

    "void main()                 \n"
    "{                           \n"
    "   gl_Position = modelViewProjectionMatrix * position; \n"
	"	uvVarying = uv;		\n"

    "}                           \n";
    
    const char fShaderStr[] =
    "precision mediump float;						\n"
	"uniform sampler2D texture;						\n"
	"varying vec3 uvVarying;						\n"
	
    "void main()									\n"
    "{												\n"
    "  gl_FragColor = texture2D(texture, uvVarying.xy);		\n"
    "  //gl_FragColor.a   *= uvVarying.z;						\n"
    "  //gl_FragColor.xyz /= gl_FragColor.a;					\n"
    "}														\n";
	
	
    GLuint vertexShader;
    GLuint fragmentShader;
    GLint linked;
    
	
	gl2::CheckForErrors();
	
    // Load the vertex/fragment shaders
    vertexShader = loadShader( vShaderStr,GL_VERTEX_SHADER);
    fragmentShader = loadShader(fShaderStr, GL_FRAGMENT_SHADER);
	
	gl2::CheckForErrors();
	
    // Create the program object
    program = glCreateProgram();
	gl2::CheckForErrors();
	
    if(program == 0)
        return;
	
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    
    gl2::CheckForErrors();
	
    glBindAttribLocation(program, ATTRIB_VERTEX, "position");
    glBindAttribLocation(program, ATTRIB_UV, "uv");
    
	glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    
    gl2::CheckForErrors();
	
    if(!linked)
    {
        GLint infoLen = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLen);
        
        if(infoLen > 1)
        {
            char* infoLog = (GLchar *) malloc(sizeof(char) * infoLen);
            glGetProgramInfoLog(program, infoLen, NULL, infoLog);
            std::cout << " !! " <<  infoLog << std::endl;
            gl2::CheckForErrors();
			
            free(infoLog);
        }
        return;
    }else{
		glUseProgram(program);
		
		uWorldMatrix= glGetUniformLocation(program, "modelViewProjectionMatrix");
		//utexture = glGetUniformLocation(program, "vtexture");
		
	}
	
	
	isSetup = true;
	gl2::CheckForErrors();
	
	texture = ci::loadImage(ci::app::loadResource("1.png"));
	std::cout << "texture loaded " << texture.getWidth() << std::endl;
	
	
}



void textureRender::setCameraMatrix(ci::Matrix44f worldMatrix){
	glUseProgram(program);
	glUniformMatrix4fv(uWorldMatrix, 1, 0,worldMatrix.m);
	glUseProgram(0);
	
}

void textureRender::bindTexture(const ci::gl::Texture texture)
{
//	glUseProgram(program);
//	glUniform4fv (utexture,1, texture);
//	glUseProgram(0);
//	
//	gl2::CheckForErrors();
}

void textureRender::setLineWidth(const float width){
	glLineWidth(width);
	
}


void textureRender::drawMesh(const ci::TriMesh mesh){
	drawMesh(mesh,GL_TRIANGLES);
}


void textureRender::drawLine(const ci::Vec3f &start,const ci::Vec3f &end){
	glUseProgram(program);
	
	ci::TriMesh mesh;
	mesh.appendVertex(start);
	mesh.appendVertex(end);
	
	drawMesh(mesh, GL_LINE_LOOP);
	glUseProgram(0);
	
}


void textureRender::drawMesh(const ci::TriMesh mesh,GLint shape){


	glUseProgram(program);
	glVertexAttribPointer(ATTRIB_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, &(mesh.getVertices()[0]));
    glEnableVertexAttribArray(ATTRIB_VERTEX);
	
	
	texture.bind();
	gl2::CheckForErrors();

	
	glVertexAttribPointer(ATTRIB_UV, 2, GL_FLOAT, GL_FALSE, 0, &mesh.getTexCoords()[0]);
	glEnableVertexAttribArray(ATTRIB_UV);

	gl2::CheckForErrors();

    glDrawArrays(shape, 0, mesh.getNumVertices());
	gl2::CheckForErrors();

	glDisableVertexAttribArray(ATTRIB_VERTEX);
	glDisableVertexAttribArray(ATTRIB_UV);

	texture.unbind();

	glUseProgram(0);
	//glDisable  (GL_BLEND);

	
	gl2::CheckForErrors();

}
