#include "ImageLoader.h"
#include <string>
#define WIN32
#include "Image_Loading/nvImage.h"

imageMap ImageLoader::images;

GLuint ImageLoader::missing;

GLuint ImageLoader::getImage(string filePath) {
	if (images[filePath] != NULL) {
		return images[filePath];
	}
	nv::Image img;
	GLuint id;
	if (img.loadImageFromFile(filePath.c_str())) {
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
		glTexImage2D(GL_TEXTURE_2D, 0, img.getInternalFormat(), img.getWidth(), img.getHeight(), 0, img.getFormat(), img.getType(), img.getLevel(0));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);
		images[filePath] = id;
		return id;
	}
	return missing;
}

void ImageLoader::makeMissingTexture() {
	GLubyte missingImage[4][4][4];
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			missingImage[i][j][0] = (GLubyte)(i + j) % 2 == 0 ? 255 : 0;
			missingImage[i][j][1] = (GLubyte)0;
			missingImage[i][j][2] = (GLubyte)(i + j) % 2 == 0 ? 255 : 0;
			missingImage[i][j][3] = (GLubyte)255;
		}
	}
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glGenTextures(1, &missing);
	glBindTexture(GL_TEXTURE_2D, missing);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 4, 4, 0, GL_RGBA, GL_UNSIGNED_BYTE, missingImage);
}