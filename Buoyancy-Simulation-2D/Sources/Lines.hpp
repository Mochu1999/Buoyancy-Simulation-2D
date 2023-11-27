#pragma once


struct Lines {
	//draw lines defined in sets, this sets creates indices for their components

	vector<float> positions;
	vector <unsigned int> indices;
	unsigned int vertexBufferLines;
	unsigned int vertexArrayLines;

	Lines() {

		
	}

	void createPolygonsLines() { //int feature is not being used?
		

		linesBuffer();
		linesIBO();

		glBindVertexArray(0);
	}

	void addSet(vector<float> items) {
		positions.insert(positions.end(), items.begin(), items.end());

		for (unsigned int i = 0; i < items.size() / 2 - 1; i++) {
			indices.insert(indices.end(), { i,i + 1 });
		}
	}

	void linesBuffer() {
		glGenVertexArrays(1, &vertexArrayLines);
		glBindVertexArray(vertexArrayLines);

		glGenBuffers(1, &vertexBufferLines);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferLines);
		glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(float), /*positions.data()*/ nullptr, GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(0); // Attribute index 0
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	}

	void linesIBO()
	{
		//indices.clear();



		unsigned int ibo;
		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * 4, indices.data(), GL_DYNAMIC_DRAW);
	}

	void draw() {

		glBindVertexArray(0);
		glBindVertexArray(vertexArrayLines);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferLines);
		glBufferSubData(GL_ARRAY_BUFFER, 0, positions.size() * 4, positions.data());
		glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
	}
};