#pragma once

//cout << "positions: " << endl;
//for (int i = 0; i < positions.size(); i += 2) {
//	cout << positions[i] << ", " << positions[i + 1] << "," << endl;
//}cout << endl;
//cout << "indices" << endl;
//for (unsigned int i = 0; i < lines.indices.size(); i++) {
//	cout << lines.indices[i] << ", ";
//}cout << endl;


struct DLines {	//D for dynamic, constanly updated
	//draw lines defined in sets, this sets creates indices for their components

	vector<float> positions;
	vector <unsigned int> indices;
	unsigned int vertexBuffer;
	unsigned int vertexArray;
	unsigned int indexBuffer;

	size_t currentBufferSize = 1000 * sizeof(float);	//simplifying the value for positions as well as indices as it is always the bigger

	//method for reserve?

	DLines() {
		genBuffers();
	}

	void clear() {
		positions.clear(); indices.clear(); setOffset = 0; setCount = 0;
	}


	int setOffset = 0;	//the offset that let multiple indices be over the past ones
	int setCount = 0;	//to avoid the fact that the last index of each set is not being counted
	void addSet(vector<float> items) {
		

		positions.insert(positions.end(), items.begin(), items.end());

		for (unsigned int i = 0; i < items.size() / 2 - 1; i++) {
			indices.insert(indices.end(), { i + setOffset,i + setOffset + 1 });
		}
		setCount++;
		setOffset = indices.size() / 2 + setCount;

	}

	void genBuffers() {
		glGenVertexArrays(1, &vertexArray);
		glBindVertexArray(vertexArray);

		glGenBuffers(1, &vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, currentBufferSize, /*positions.data()*/ nullptr, GL_DYNAMIC_DRAW);

		glGenBuffers(1, &indexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, currentBufferSize, nullptr, GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

		glBindVertexArray(0);
	}


	void draw() {

		glBindVertexArray(vertexArray);

		if (positions.size() * sizeof(float) > currentBufferSize) {

			currentBufferSize += positions.size() * sizeof(float);
			glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
			glBufferData(GL_ARRAY_BUFFER, currentBufferSize, positions.data(), GL_DYNAMIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, currentBufferSize, nullptr, GL_DYNAMIC_DRAW);
		}



		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, positions.size() * sizeof(float), positions.data());

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof(unsigned int), indices.data());

		glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);


		
	}


	~DLines() {
		
		glDeleteVertexArrays(1, &vertexArray);
		glDeleteBuffers(1, &vertexBuffer);
		glDeleteBuffers(1, &indexBuffer);

		positions.clear(); indices.clear();
	}
};














struct SLines {	//Static, if you want to change its values you must create bufferData again
	//draw lines defined in sets, this sets creates indices for their components

	vector<float> positions;
	vector <unsigned int> indices;
	unsigned int vertexBuffer;
	unsigned int vertexArray;
	unsigned int indexBuffer;


	void clear() {
		positions.clear();
		indices.clear();
		setOffset = 0; setCount = 0;
	}

	int setOffset = 0;	//the offset that let multiple indices be over the past ones
	int setCount = 0;	//to avoid the fact that the last index of each set is not being counted

	void addSet(vector<float> items) {

		positions.insert(positions.end(), items.begin(), items.end());

		for (unsigned int i = 0; i < items.size() / 2 - 1; i++) {
			indices.insert(indices.end(), { i + setOffset,i + setOffset + 1 });
		}
		setCount++;
		setOffset = indices.size() / 2 + setCount;

	}

	void genBuffers() {
		glGenVertexArrays(1, &vertexArray);
		glBindVertexArray(vertexArray);

		glGenBuffers(1, &vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, positions.size()*sizeof(float), positions.data(), GL_STATIC_DRAW);

		glGenBuffers(1, &indexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(float), indices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

		glBindVertexArray(0);
	}


	void draw() {

		glBindVertexArray(vertexArray);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);


		glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);

	}


	~SLines() {
		glDeleteVertexArrays(1, &vertexArray);
		glDeleteBuffers(1, &vertexBuffer);
		glDeleteBuffers(1, &indexBuffer);
	}
};
