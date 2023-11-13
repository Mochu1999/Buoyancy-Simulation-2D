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

	string allGlyphs = "C";


	int startX, startY;

	struct GlyphMetrics {
		float width, height;
		float bearingX, bearingY;
		float advance;
		float texCoordX0 = 0, texCoordY0 = 0;
		float texCoordX1 = 1, texCoordY1 = 1;
	};
	std::map<char, GlyphMetrics> glyphMetricsMap;



	Text(string textToDraw_, int startX_, int startY_) :textToDraw(textToDraw_), startX(startX_), startY(startY_) {
		initializeFreeType(glyphPath);
		processGlyphs();
		renderGlyph();

		initializeBuffer();
		initializeIBO();
		textBind();
	}
	float widthAtlas = 0;
	float heightAtlas = 0;
	void processGlyphs() {

		glBindTexture(GL_TEXTURE_2D, textID);

		int i = 0;
		float currentX = 0;
		float xOffset = 0;
		for (char& c : allGlyphs) {

			widthAtlas += storeGlyph(c);
			if (glyphMetricsMap[c].height > heightAtlas) {
				heightAtlas = glyphMetricsMap[c].height;
			}
		}
		createTextureAtlas();
		for (char& c : allGlyphs) {
			glBindTexture(GL_TEXTURE_2D, textID);
			if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
				std::cerr << "Failed to load glyph" << std::endl;
				continue;
			}

			FT_Bitmap& bitmap = face->glyph->bitmap;

			glTexSubImage2D(
				GL_TEXTURE_2D,
				0,
				xOffset,
				0,
				bitmap.width,
				bitmap.rows,
				GL_RED,
				GL_UNSIGNED_BYTE,
				bitmap.buffer
			);

			xOffset += bitmap.width;

			glyphMetricsMap[c].texCoordX0 = currentX / widthAtlas;
			glyphMetricsMap[c].texCoordY0 = 0;
			glyphMetricsMap[c].texCoordX1 = (currentX + glyphMetricsMap[c].width) / widthAtlas;
			glyphMetricsMap[c].texCoordY1 = 1;

			currentX += glyphMetricsMap[c].advance;
		}
		for (auto& pair : glyphMetricsMap) {
			cout << endl << endl;
			char i = pair.first;
			cout << "Glyph: " << i << endl;
			//cout << "glyphMetricsMap[i].width: " << glyphMetricsMap[i].width << endl;
			//cout << "glyphMetricsMap[i].height: " << glyphMetricsMap[i].height << endl;
			//cout << "glyphMetricsMap[i].bearingX: " << glyphMetricsMap[i].bearingX << endl;
			//cout << "glyphMetricsMap[i].bearingY: " << glyphMetricsMap[i].bearingY << endl;
			//cout << "glyphMetricsMap[i].advance: " << glyphMetricsMap[i].advance << endl;
			cout << "glyphMetricsMap[i].texCoordX0: " << glyphMetricsMap[i].texCoordX0 << endl;
			cout << "glyphMetricsMap[i].texCoordY0: " << glyphMetricsMap[i].texCoordY0 << endl;
			cout << "glyphMetricsMap[i].texCoordX1: " << glyphMetricsMap[i].texCoordX1 << endl;
			cout << "glyphMetricsMap[i].texCoordY1: " << glyphMetricsMap[i].texCoordY1 << endl << endl;
		}//cout
		glBindTexture(GL_TEXTURE_2D, 0);
	}



	int storeGlyph(char character) {		//Stores metrics inside a map //change to allow multiple glyphs

		if (FT_Load_Char(face, character, FT_LOAD_RENDER)) {
			std::cerr << "Failed to load glyph" << std::endl;

		}



		FT_Bitmap& bitmap = face->glyph->bitmap;

		float bearingX = face->glyph->bitmap_left;
		float bearingY = face->glyph->bitmap_top;
		float w = bitmap.width;
		float h = bitmap.rows;
		/*float offsetX = bearingX;
		float offsetY = (bearingY - bitmap.rows);*/
		float advance = face->glyph->advance.x >> 6;





		glyphMetricsMap.emplace(character, GlyphMetrics{ w, h, bearingX, bearingY, advance });
		return advance;
	}

	void pushIndices(size_t i) {
		unsigned int aux = i * 4;
		indices.insert(indices.end(), { aux,aux + 1,aux + 2,aux,aux + 2,aux + 3 });
	}

	void renderGlyph() {
		int x = startX;
		int y = startY;

		for (size_t i = 0; i < textToDraw.length(); ++i) {
			char c = textToDraw[i];


			GlyphMetrics metrics = glyphMetricsMap[c];

			// Calculate the vertex positions based on the glyph metrics
			float x0 = x + metrics.bearingX;
			float y0 = y - (metrics.height - metrics.bearingY);
			float x1 = x0 + metrics.width;
			float y1 = y0 + metrics.height;

			// The texture coordinates are taken directly from the glyph metrics
			float s0 = metrics.texCoordX0;
			float t0 = metrics.texCoordY0;
			float s1 = metrics.texCoordX1;
			float t1 = metrics.texCoordY1;

			// Insert positions and texture coordinates into the positions vector
			positions.insert(positions.end(), {
				x0, y0, s0, t1,
				x1, y0, s1, t1,
				x1, y1, s1, t0,
				x0, y1, s0, t0
				});
			/*positions.insert(positions.end(), {
				x0, y0, 0, 0,
				x1, y0, 1, 0,
				x1, y1, 1, 1,
				x0, y1, 0, 1
				});*/


			x += metrics.advance;

			pushIndices(i);
		}

	}


	void createTextureAtlas() {
		glGenTextures(1, &textID);
		glBindTexture(GL_TEXTURE_2D, textID);

		// Set the unpack alignment to 1 byte to handle bitmaps that are not aligned to 4 bytes
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		/*glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);*/
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			widthAtlas,     // Total width of the atlas
			heightAtlas,    // Height to fit the tallest glyph
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			nullptr         // No data yet
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