#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H
#include <map>






struct Text {
	unsigned int textID;


	unsigned int VA, VB, ibo;
	vector<float> positions;
	string textToDraw;

	vector<unsigned int> indices;

	FT_Library ft;
	FT_Face face;
	string glyphPath = R"(C:\dev\C++ libs\Helvetica\Helvetica.otf)";

	string allGlyphs = "1";



	struct GlyphMetrics {
		float width, height; // The width and height of the glyph
		float bearingX, bearingY; // The left bearing and top bearing of the glyph
		float advance; // The advance width for the glyph
		float texCoordX0, texCoordY0; // The bottom-left texture coordinates of the glyph in the atlas
		float texCoordX1, texCoordY1; // The top-right texture coordinates of the glyph in the atlas
	};

	std::map<char, GlyphMetrics> glyphMetricsMap;

	void processGlyphs() {



		for (char c : allGlyphs) {

			storeGlyph(c, 0, 0);

			//pushIndices(i);
		}

	}

	

	void storeGlyph(char character, float x, float y) {
		// Load the character glyph into the face
		if (FT_Load_Char(face, character, FT_LOAD_RENDER)) {
			std::cerr << "Failed to load glyph" << std::endl;

		}




		// Get the glyph metrics
		FT_Bitmap& bitmap = face->glyph->bitmap;
		float bearingX = face->glyph->bitmap_left;
		float bearingY = face->glyph->bitmap_top;


		// Calculate the quad positions based on the glyph metrics
		float w = bitmap.width;
		float h = bitmap.rows;
		/*float offsetX = bearingX;
		float offsetY = (bearingY - bitmap.rows);*/
		float advance = face->glyph->advance.x >> 6;


		/*positions.insert(positions.end(), {
			x + offsetX,     y + offsetY	,   0.0f, 1.0f,
			x + offsetX + w, y + offsetY	, 	1.0f, 1.0f,
			x + offsetX + w, y + offsetY + h, 	1.0f, 0.0f,
			x + offsetX,     y + offsetY + h,   0.0f, 0.0f});*/


		glyphMetricsMap.emplace(character, GlyphMetrics{w, h, bearingX, bearingY, advance,
																							0.0f, 0.0f, // texCoordX0, texCoordY0 (bottom-left)
																							1.0f, 1.0f  // texCoordX1, texCoordY1 (top-right)
																									});
	}

	void pushIndices(size_t i) {
		unsigned int aux = i * 4;
		indices.insert(indices.end(), { aux,aux + 1,aux + 2,aux,aux + 2,aux + 3 });
	}
	void renderGlyph() {
		float x = 100,y=100;//input en constructor

		for (size_t i = 0; i < textToDraw.length(); ++i) {
			char c = textToDraw[i];

			GlyphMetrics metrics = glyphMetricsMap[c];

			// Calculate the vertex positions based on the glyph metrics
			float x0 = x + metrics.bearingX;
			float y0 = y - (metrics.height - metrics.bearingY); // Y is down in OpenGL so subtract the difference
			float x1 = x0 + metrics.width;
			float y1 = y0 + metrics.height;

			// The texture coordinates are taken directly from the glyph metrics
			float s0 = metrics.texCoordX0;
			float t0 = metrics.texCoordY0;
			float s1 = metrics.texCoordX1;
			float t1 = metrics.texCoordY1;

			// Insert positions and texture coordinates into the positions vector
			positions.insert(positions.end(), {
				x0, y0, s0, t1, // Bottom-left
				x1, y0, s1, t1, // Bottom-right
				x1, y1, s1, t0, // Top-right
				x0, y1, s0, t0  // Top-left
				});
			
			pushIndices(i);
		}
		//cout << endl;
		//cout << "advanceSum " << advanceSum << endl;
		//cout << "positions.size() " << positions.size() << endl;
		//printMatrix(positions, "positions", 4, 12);
	}
	

	void createTextureAtlas() {
		glGenTextures(1, &textID);
		glBindTexture(GL_TEXTURE_2D, textID);

		// Set the unpack alignment to 1 byte to handle bitmaps that are not aligned to 4 bytes
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);*/ //smoother, but somewhat blurry 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); //pixelated, but sharp 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glBindTexture(GL_TEXTURE_2D, 0);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	}

	Text(string textToDraw_) :textToDraw(textToDraw_) {
		initializeFreeType(glyphPath);
		processGlyphs();
		createTextureAtlas();
		renderGlyph();
		initializeBuffer();
		initializeIBO();
		textBind();
	}


	void initializeFreeType(const std::string& fontPath) {
		if (FT_Init_FreeType(&ft)) {
			std::cerr << "Could not init FreeType Library" << std::endl;
			return;
		}

		if (FT_New_Face(ft, fontPath.c_str(), 0, &face)) {
			std::cerr << "Failed to load font" << std::endl;
			return;
		}

		FT_Set_Pixel_Sizes(face, 0, 48); // Set font size



	}


	void initializeBuffer() {
		glBindVertexArray(0);
		glGenVertexArrays(1, &VA);
		glBindVertexArray(VA);

		glGenBuffers(1, &VB);
		glBindBuffer(GL_ARRAY_BUFFER, VB);
		glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(float), /*positions.data()*/ nullptr, GL_DYNAMIC_DRAW);



		// Position attribute
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

		// text coordinate attribute
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	}

	void initializeIBO() {


		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * 4, indices.data(), GL_DYNAMIC_DRAW);
	}

	void draw() {
		glBindVertexArray(0);
		glBindVertexArray(VA);
		glBindBuffer(GL_ARRAY_BUFFER, VB);
		glBufferSubData(GL_ARRAY_BUFFER, 0, positions.size() * 4, positions.data());
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
	}


	void textBind(unsigned int slot = 0) {
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, textID);
	} //slot optional parameter for loading one specific texture between multiple



	void textUnbind() {
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	~Text() {
		// Clean up the resources
		glDeleteTextures(1, &textID);
		glDeleteBuffers(1, &VB);
		glDeleteBuffers(1, &ibo); // Ensure you've created and stored the IBO handle
		glDeleteVertexArrays(1, &VA);

		// Cleanup FreeType resources
		FT_Done_Face(face);
		FT_Done_FreeType(ft);
	}

};