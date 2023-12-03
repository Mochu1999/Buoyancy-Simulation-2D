#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H
#include <map>






struct Text {
	//Creates a texture atlas for text and displays it correctly

	unsigned int vertexArray, vertexBuffer, indexBuffer, textureBuffer;

	vector<float> positions;	//quads
	vector<unsigned int> indices;

	size_t currentBufferSize = 1000 * sizeof(float);


	FT_Library ft;
	FT_Face face;
	string glyphPath = R"(C:\dev\C++ libs\Helvetica\Helvetica.otf)";

	string allGlyphs = "abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890.:,;'^(!?)+-*/=";



	struct GlyphMetrics {
		float width, height;
		float bearingX, bearingY;
		float advertexArraynce;
		float texCoordX0 = 0, texCoordY0 = 0;
		float texCoordX1 = 1, texCoordY1 = 1;
	};
	std::map<char, GlyphMetrics> glyphMetricsMap;


	Text() {

		genBuffers();
		initializeFreeType();
		createTextureAtlas();
	}




	struct textStruct {
		string streamText;
		float startX;
		float startY;
	};

	vector<textStruct> textToDraw;




	template <typename... Args>
	//typename its a type placeholder	//... vertexArrayriadic templates, for an unknown number of elements
	//Args is an arbitrary name for this collection of types

	//There might be ways of modifying the function so the call is made with << and {} for the postions, but is advertexArraynced shit
	void addText(float startX_, float startY_, Args... args) { //args, the actual vertexArraylues that are passed
		std::ostringstream stream;
		(stream << ... << args);  // Format the string using the arguments
		std::string formattedText = stream.str();

		textToDraw.push_back(textStruct{ formattedText, startX_, startY_ });
	}





	float widthAtlas = 0;
	float heightAtlas = 0;
	void createTextureAtlas() {

		float currentX = 0;
		for (char& c : allGlyphs) {

			storeGlyph(c); //store metrics and increases widthAtlas and totalAdvertexArraynce
			if (glyphMetricsMap[c].height > heightAtlas) {
				heightAtlas = glyphMetricsMap[c].height;
			}
		}

		createTextureAtlasLayout(); //sets empty texture

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //Sets the unpack alignment to 1 byte to handle bitmaps that are not aligned to 4 bytes

		for (char& c : allGlyphs) {
			//stores each metric in the texture

			glBindTexture(GL_TEXTURE_2D, textureBuffer);//necessary here

			if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
				std::cerr << "Failed to load glyph " << c << std::endl;
				continue;
			}

			FT_Bitmap& bitmap = face->glyph->bitmap;


			glTexSubImage2D(	//subtexture for each glyph
				GL_TEXTURE_2D,
				0, //detail number, 0 is base image level
				currentX, // x position in the texture from the left
				0, // y position in the texture
				bitmap.width, // width of the subimage
				bitmap.rows, //  height of the subimage
				GL_RED,
				GL_UNSIGNED_BYTE,
				bitmap.buffer // The actual bitmap data for the glyph.
			);



			//adding final metrics
			glyphMetricsMap[c].texCoordX0 = currentX / widthAtlas;
			glyphMetricsMap[c].texCoordY0 = 0;
			glyphMetricsMap[c].texCoordX1 = (currentX + glyphMetricsMap[c].width) / widthAtlas;
			glyphMetricsMap[c].texCoordY1 = (glyphMetricsMap[c].height) / heightAtlas;

			currentX += glyphMetricsMap[c].width;


		}
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);



		glBindTexture(GL_TEXTURE_2D, 0);
	}



	void storeGlyph(char character) {
		//Stores metrics inside a map 


		if (FT_Load_Char(face, character, FT_LOAD_RENDER)) {
			std::cerr << "Failed to load glyph" << std::endl;

		}



		FT_Bitmap& bitmap = face->glyph->bitmap;

		float bearingX = face->glyph->bitmap_left;
		float bearingY = face->glyph->bitmap_top;
		float width = bitmap.width;
		float height = bitmap.rows;

		float advertexArraynce = face->glyph->advance.x >> 6;


		widthAtlas += width;



		glyphMetricsMap.emplace(character, GlyphMetrics{ width, height, bearingX, bearingY, advertexArraynce });

	}



	void renderGlyph() {	//creates the quads and takes atlas positions

		int count = 0;
		int lastIndex = 0;

		for (const auto& item : textToDraw) {

			int x = item.startX;
			int y = item.startY;

			for (size_t i = 0; i < item.streamText.length(); ++i) {
				char c = item.streamText[i];


				GlyphMetrics metrics = glyphMetricsMap[c];


				float x0 = x + metrics.bearingX;
				float y0 = y - (metrics.height - metrics.bearingY);
				float x1 = x0 + metrics.width;
				float y1 = y0 + metrics.height;


				float s0 = metrics.texCoordX0;
				float t0 = metrics.texCoordY0;
				float s1 = metrics.texCoordX1;
				float t1 = metrics.texCoordY1;


				positions.insert(positions.end(), {
					x0, y0, s0, t1,
					x1, y0, s1, t1,
					x1, y1, s1, t0,
					x0, y1, s0, t0
					});


				x += metrics.advertexArraynce;
				pushIndices(i, count);
			}
			count = indices.size() / 6;
		}
		textToDraw.clear();
	}

	void pushIndices(size_t i, int count) {
		unsigned int aux = i * 4 + count * 4;
		indices.insert(indices.end(), { aux,aux + 1,aux + 2,aux,aux + 2,aux + 3 });
	}

	void createTextureAtlasLayout() {

		glGenTextures(1, &textureBuffer);
		glBindTexture(GL_TEXTURE_2D, textureBuffer);



		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,	//this gives the internal format, look for more info
			widthAtlas, // width of the entire texture atlas.
			heightAtlas, //height of the tallest glyph in the atlas, used as the atlas height.
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			nullptr
		);



		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //smoother, but somewhat blurry 
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); //pixelated, but sharp 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glBindTexture(GL_TEXTURE_2D, 0);

	}




	void initializeFreeType() {
		if (FT_Init_FreeType(&ft)) {
			std::cerr << "Could not init FreeType Library" << std::endl;
			return;
		}

		if (FT_New_Face(ft, glyphPath.c_str(), 0, &face)) {
			std::cerr << "Failed to load font" << std::endl;
			return;
		}

		FT_Set_Pixel_Sizes(face, 0, 18); // Set font size	//Por que no son los tamaños de word?
	}

	void genBuffers() {
		glGenVertexArrays(1, &vertexArray);
		glBindVertexArray(vertexArray);

		glGenBuffers(1, &vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, currentBufferSize, /*positions.data()*/ nullptr, GL_DYNAMIC_DRAW);

		glGenBuffers(1, &indexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, currentBufferSize, indices.data(), GL_DYNAMIC_DRAW);

		// Position attributes
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

		// text coordinate attributes
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));


	}


	void draw() {		//set to be full dynamic, it shouldn't be
		indices.clear(); positions.clear();

		renderGlyph();

		glBindVertexArray(vertexArray);

		if (positions.size() * sizeof(float) > currentBufferSize) {

			currentBufferSize += positions.size() * sizeof(float);
			glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
			glBufferData(GL_ARRAY_BUFFER, currentBufferSize, positions.data(), GL_DYNAMIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, currentBufferSize, nullptr, GL_DYNAMIC_DRAW);
		}

		

		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, positions.size() * 4, positions.data());

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof(unsigned int), indices.data());

		glBindTexture(GL_TEXTURE_2D, textureBuffer);

		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
	}



	~Text() {
		positions.clear(); indices.clear();
		// Clean up the resources
		glDeleteTextures(1, &textureBuffer);
		glDeleteBuffers(1, &vertexBuffer);
		glDeleteBuffers(1, &indexBuffer); // Ensure you've created and stored the indexBuffer handle
		glDeleteVertexArrays(1, &vertexArray);

		// Cleanup FreeType resources
		FT_Done_Face(face);
		FT_Done_FreeType(ft);
	}

};