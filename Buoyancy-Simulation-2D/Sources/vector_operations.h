#pragma once


//Use consts, use arrays, debug mode, size
//Adding functions for specific numbers to avoid for loops and precalculate stuff

std::vector<float> subtract(std::vector<float>& input1, std::vector<float>& b) {
	std::vector<float> output;
	output.reserve(input1.size());
	for (size_t i = 0; i < input1.size(); i++) {
		output.emplace_back(input1[i] - b[i]);
	}

	return output;
}

std::vector<float> normalize(const std::vector<float>& input1) {
	std::vector<float> output;
	output.reserve(input1.size());

	float interm = 0;
	for (size_t i = 0; i < input1.size(); i++) {
		interm += input1[i] * input1[i];
	}


	if (interm == 0) {
		std::cout << "ERROR: normalize()";
		std::exit(EXIT_FAILURE);
	}

	float length = std::sqrt(interm);
	for (size_t i = 0; i < input1.size(); i++) {
		output.emplace_back(input1[i] / length);
	}
	/* std::transform(input1.begin(), input1.end(), std::back_inserter(output),
		 [length](float val) { return val / length; });*/



	return output;
}

std::vector<float> crossProduct3d(const std::vector<float>& a, const std::vector<float>& b) {//perpendicular vector to the other two with magnitude: ∣A × B∣ = ∣A∣⋅∣B∣⋅sin(θ)
	if (a.size() != 3 || b.size() != 3) { //this should be at debug
		std::cout << "ERROR: crossProduct3d() size";
		std::exit(EXIT_FAILURE);
	}

	return {
		a[1] * b[2] - a[2] * b[1],//i
		-a[0] * b[2] + a[2] * b[0],//j
		a[0] * b[1] - a[1] * b[0]//k
	};
}

float dotProduct3d(const std::vector<float>& a, const std::vector<float>& b) {// A · B = ∣A∣⋅∣B∣⋅cos(θ)
	if (a.size() != 3 || b.size() != 3) {
		std::cout << "ERROR: dotProduct3d() size";
		std::exit(EXIT_FAILURE);
	}

	return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

inline std::vector<float> mul4x4(const std::vector<float>& a, const std::vector<float>& b) {    // ?
	if (a.size() != 16 || b.size() != 16) {
		std::cout << "ERROR: mul4x4() size";
		std::exit(EXIT_FAILURE);
	}

	std::vector<float> output(16, 0.0f);

	for (int row = 0; row < 4; ++row) {
		for (int col = 0; col < 4; ++col) {
			float sum = 0.0f;
			int outputIndex = row + col * 4;
			for (int k = 0; k < 4; ++k) {
				int aIndex = row + k * 4;
				int bIndex = k + col * 4;
				sum += a[aIndex] * b[bIndex];
			}
			output[outputIndex] = sum;
		}
	}

	return output;

}

std::vector<float> scaleVector(const std::vector<float>& vec, float k) {
	std::vector<float> output(vec.size());
	for (size_t i = 0; i < vec.size(); ++i) {
		output[i] = vec[i] * k;
	}
	return output;
}

std::vector<float> identityMatrix = {		//function for creating identity matrices of any size
1.0f, 0.0f, 0.0f, 0.0f,
0.0f, 1.0f, 0.0f, 0.0f,
0.0f, 0.0f, 1.0f, 0.0f,
0.0f, 0.0f, 0.0f, 1.0f
};
void printMatrix(const std::vector<float>& matrix, const std::string& matrixName, int rows, int cols) {
	if (matrix.size() != rows * cols) {
		std::cout << "Invalid matrix size. Should be " << rows * cols << " elements for a " << rows << "x" << cols << " matrix." << std::endl;
		return;
	}
	std::cout <<matrixName << std::endl;

	for (int row = 0; row < rows; ++row) {
		for (int col = 0; col < cols; ++col) {
			std::cout << matrix[row * cols + col] << "\t";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}




