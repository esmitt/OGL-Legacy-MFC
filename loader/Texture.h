#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include <string>

/**
* Class Texture.
* Allow to load a texture from a namefile and acces the texels.
*
*/
class CTexture
{

	//Functions
	public:

		///Default constructor
		CTexture();

		///Default destructor
		~CTexture();

		///Create an empty texture
		void createEmptyTexture2D(GLuint Width = 512, GLuint Height  = 512, 
								GLint internalFormat = GL_RGB, GLenum pixelFormat = GL_RGB, GLenum pixelType = GL_UNSIGNED_BYTE,
								GLint minFilter = GL_NEAREST, GLint magFilter = GL_NEAREST,
								GLvoid* data = 0);

		///Create an empty texture with a border
		void createEmptyTexture2DClampToBorder(GLuint Width = 512, GLuint Height  = 512, 
								GLint internalFormat = GL_RGB, GLenum pixelFormat = GL_RGB, GLenum pixelType = GL_UNSIGNED_BYTE,
								GLint minFilter = GL_NEAREST, GLint magFilter = GL_NEAREST,
								GLvoid* data = 0, GLfloat border[4] = NULL);

		///Method to load a texture from the file archivo
		bool loadTexture(std::string strFilename);

		///Bind the texture
		void bindTexture();

		///Method to get Texture ID
		GLuint getTextId();

		///Return if there is some texture loaded
		inline bool isValid(){return m_bValid;}

	//Variables

	private:
		GLuint m_iText;
		bool m_bValid;
};

#endif
