#include "texture.h"
#include "../ew/external/stb_image.h"
#include "../ew/external/glad.h"

namespace rm {
	unsigned int loadTexture(const char* filePath, int wrapMode, int filterMode)
	{
		stbi_set_flip_vertically_on_load(true);

		int width, height, numComponents;
		unsigned char* data = stbi_load(filePath, &width, &height, &numComponents, 4);

		if (data == NULL)
		{
			printf("Failed to load image %s", filePath);
			// Returns an array of bytes, width and height in pixels, and number of components per pixel
			stbi_image_free(data);

			return 0;
		}

		unsigned int texture;
		// Creates new texture name
		glGenTextures(1, &texture);
		// Binds texture name to GL_TEXTURE_2D alias
		glBindTexture(GL_TEXTURE_2D, texture);

		GLenum format;
		GLint internalFormat;

		switch (numComponents)
		{
		case 1:
			format = GL_RED;
			internalFormat = GL_RED;
			break;
		case 2:
			format = GL_RG;
			internalFormat = GL_RG;
			break;
		case 3:
			format = GL_RGB;
			internalFormat = GL_RGB;
			break;
		case 4:
			format = GL_RGBA;
			internalFormat = GL_RGBA;
			break;
		default:
			printf("Too many or too little components");
		}

		// Used to reserve memory and set texture data
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		// Wrapping
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);

		// Filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMode);

		// Mipmap
		glGenerateMipmap(GL_TEXTURE_2D);

		// Handling
		glBindTexture(GL_TEXTURE_2D, 0);
		stbi_image_free(data);

		return texture;
	}

}