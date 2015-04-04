#include "stdafx.h"
#ifdef _WIN32
	#pragma comment (lib,"lib/FreeImage.lib")
#endif

//Includes
#ifdef _WIN32
	#include "../GL/glew.h"
#else
	#include <GL/glew.h>
#endif
#ifdef _WIN32
	#include "FreeImage.h"
#else
	#include <FreeImage.h>
#endif
#include <iostream>
#include "Texture.h"
using namespace std;
#ifdef _WIN32
	#pragma comment (lib,"lib/glew32.lib")
#endif
/**
* Method to construct a Texture
*
*/
CTexture::CTexture()
{
	m_bValid = false;	//all are invalid until load something valid
}


/**
* Method to desctruct a Texture.
*
*/
CTexture::~CTexture()
{
	if(glIsTexture(m_iText))
		glDeleteTextures(1,&m_iText);
}


/**
* Create an empty texture
* @params Width the width of the texture
* @params Height the height of the texture
* @params internalFormat the internal format of the texture
* @params pixelFormat the pixel format of the texture
* @params pixelType the pixel type of the texture
* @params minFilter the minification filter of the texture
* @params magFilter the magnification texture of the texture
* @params data the data of te texture (if any is present)	
*
*/
void CTexture::createEmptyTexture2D(GLuint Width, GLuint Height, 
								GLint internalFormat, GLenum pixelFormat, GLenum pixelType,
								GLint minFilter, GLint magFilter,
								GLvoid* data)
{	
	glGenTextures( 1, &m_iText );
	glBindTexture( GL_TEXTURE_2D, m_iText );
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,magFilter);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,minFilter);

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, Width,Height, 0, pixelFormat, pixelType, data);
}


/**
* Create an empty texture
* @params Width the width of the texture
* @params Height the height of the texture
* @params internalFormat the internal format of the texture
* @params pixelFormat the pixel format of the texture
* @params pixelType the pixel type of the texture
* @params minFilter the minification filter of the texture
* @params magFilter the magnification texture of the texture
* @params data the data of te texture (if any is present)
* @params border[4] the color of the border
*
*/
void CTexture::createEmptyTexture2DClampToBorder(GLuint Width, GLuint Height, 
								GLint internalFormat, GLenum pixelFormat, GLenum pixelType,
								GLint minFilter, GLint magFilter,
								GLvoid* data, GLfloat border[4])
{
	glGenTextures( 1, &m_iText );
	glBindTexture( GL_TEXTURE_2D, m_iText );
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,magFilter);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,minFilter);

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, Width,Height, 0, pixelFormat, pixelType, data);
	
}

#define GL_CHECK_ERRORS assert(glGetError()== GL_NO_ERROR);
#define printOpenGLError() printOglError2(__FILE__, __LINE__)
int printOglError2(char *file, int line)
{
	GLenum glErr;
	int    retCode = 0;
	glErr = glGetError();
	while (glErr != GL_NO_ERROR)
	{
		///std::cout << "glError in file " << file << "@ line " << line << ": " << gluErrorString(glErr) << std::endl;
		TRACE(L"glError in file %s @ line %d", file, line);
		retCode = 1;
		glErr = glGetError();
	}
	return retCode;
}
//#include "SOIL\SOIL.h"
/**
* Method to load a texture from a file
*
* @params farchivo its a string if the namefile of the texture.
*
*/
bool CTexture::loadTexture(std::string strFilename)
{
	glGenTextures(1, &m_iText );
	//glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_iText);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	//The bmp is loaded with freeImage y and store in m_Bits
	FREE_IMAGE_FORMAT fifmt = FreeImage_GetFileType(strFilename.c_str(), 0);
	FIBITMAP *dib = FreeImage_Load(fifmt, strFilename.c_str(), 0);
	dib = FreeImage_ConvertTo24Bits(dib);
	
	unsigned char * m_Bits;
	
	if( dib != NULL ){
		
		m_Bits = new BYTE[FreeImage_GetWidth(dib) * FreeImage_GetHeight(dib) * 3];
		GLuint m_iAlto = FreeImage_GetHeight(dib);
		GLuint m_iAncho = FreeImage_GetWidth(dib);
		BYTE *pixels = (BYTE*)FreeImage_GetBits(dib);

		for(GLuint pix=0; pix<FreeImage_GetWidth(dib) * FreeImage_GetHeight(dib); pix++){
			m_Bits[pix*3+0]=pixels[pix*3+2];
			m_Bits[pix*3+1]=pixels[pix*3+1];
			m_Bits[pix*3+2]=pixels[pix*3+0];
		}
		FreeImage_Unload(dib);
		//glTexStorage2D(GL_TEXTURE_2D,	1, GL_RGB8, m_iAncho, m_iAlto);
		//glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_iAncho, m_iAlto, GL_RGB, GL_UNSIGNED_BYTE, m_Bits);
		//glGenerateMipmap(GL_TEXTURE_2D);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, m_iAncho, m_iAlto, 0,
			GL_RGB, GL_UNSIGNED_BYTE, m_Bits);
		//glGenerateMipmap(GL_TEXTURE_2D);
			printOpenGLError();
		delete m_Bits;
		m_Bits = NULL;
		m_bValid = true;
		return true;
	}else{
		//TRACE("Couldn't open texture %s\n",strFilename.c_str());
		m_bValid = false;
		return false;
	}
}

/**
* Method for binding a texture
*
*/
void CTexture::bindTexture()
{
	glBindTexture(GL_TEXTURE_2D, m_iText);
}


/**
* Method to get Texture ID
*
*/
GLuint CTexture::getTextId()
{
	return m_iText;
}