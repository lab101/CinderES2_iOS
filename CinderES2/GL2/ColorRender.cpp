//
//  colorRender.cpp
//  CinderES2
//
//  Created by Kris Meeusen on 15/02/13.
//  Copyright (c) 2013 Kris Meeusen. All rights reserved.
//

#include "colorRender.h"
#include "shaderLoader.h"
#include "checkError.h"

using namespace ci;

ColorRender* ColorRender::instance;


ColorRender* ColorRender::Instance(){
    if(instance == NULL)
	{
	     instance = new ColorRender();
		 instance->setup();
	}
    return instance;
}


ColorRender::ColorRender(){
	isSetup = false;
}


void ColorRender::setup(){
	const char vShaderStr[] =
    "attribute vec4 position;   \n"
	"uniform mat4 modelViewProjectionMatrix; \n"
	"uniform mat3 normalMatrix;			\n"
	""
    "void main()                 \n"
    "{                           \n"
    "   gl_Position = modelViewProjectionMatrix * position; \n"
    "}                           \n";
    
    const char fShaderStr[] =
    "precision mediump float;                   \n"
	"uniform vec4 vColor;						\n"
    "void main()                                \n"
    "{                                          \n"
    "  gl_FragColor = vColor;					\n"
    "}                                          \n";
	
	
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
			// esLogMessage("Error linking program:\n%s\n", infoLog);
            
            gl2::CheckForErrors();
			
            free(infoLog);
        }
        return;
    }else{
		glUseProgram(program);
		
		uWorldMatrix= glGetUniformLocation(program, "modelViewProjectionMatrix");
		uColor = glGetUniformLocation(program, "vColor");

	}
	
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glUseProgram(0);
	isSetup = true;
	
}

void ColorRender::setCameraMatrix(ci::Matrix44f worldMatrix){
	glUseProgram(program);
	glUniformMatrix4fv(uWorldMatrix, 1, 0,worldMatrix.m);
	glUseProgram(0);

}

void ColorRender::setColor(const ci::ColorA color){
	glUseProgram(program);
	glUniform4fv (uColor,1, color);
	glUseProgram(0);

	gl2::CheckForErrors();
}

void ColorRender::setLineWidth(const float width){
	glUseProgram(program);
	glLineWidth(width);
	glUseProgram(0);

}


void ColorRender::drawMesh(const ci::TriMesh mesh){
	drawMesh(mesh,GL_TRIANGLES);
}


void ColorRender::drawLine(const ci::Vec3f &start,const ci::Vec3f &end){
	glUseProgram(program);

	ci::TriMesh mesh;
	mesh.appendVertex(start);
	mesh.appendVertex(end);

	drawMesh(mesh, GL_LINE_LOOP);
	glUseProgram(0);

}


void ColorRender::drawMesh(const ci::TriMesh mesh,GLint shape){
	glUseProgram(program);

	glVertexAttribPointer(ATTRIB_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, &(mesh.getVertices()[0]));
    glEnableVertexAttribArray(ATTRIB_VERTEX);
	
    glDrawArrays(shape, 0, mesh.getNumVertices());
	
	glDisableVertexAttribArray(ATTRIB_VERTEX);
	glUseProgram(0);
}

