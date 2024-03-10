#pragma once

//common file that does not depend on other files

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <cmath>
#include <numeric> 
#include <algorithm>

#include <array>
#include <vector>
#include <unordered_set>
#include <set>
#include <unordered_map>
#include <list>
#include <deque>

#include <chrono>
using namespace std;
using namespace std::chrono;




inline float fastInverseSqrt(float number) {
	long i;
	float x2, y;
	x2 = number * 0.5F;
	y = number;
	std::memcpy(&i, &y, sizeof(i)); // Safer type-punning
	i = 0x5f3759df - (i >> 1);
	std::memcpy(&y, &i, sizeof(y)); // Safer type-punning
	y = y * (1.5f - (x2 * y * y));
	return y;
}

template<typename T>
struct vec2 {
	T x, y;

	vec2() : x(0), y(0) {} //initializing paramenters to 0 if not specified 

	vec2(T x, T y) : x(x), y(y) {} //normal constructor

	// Operator overloads for comparison
	bool operator == (const vec2& other) const {
		return x == other.x && y == other.y;
	}

	bool operator!=(const vec2& other) const {
		return !(*this == other);
	}

	// Operator overloads for arithmetic
	vec2 operator + (const vec2& other) const {
		return { x + other.x, y + other.y };
	}

	vec2 operator - (const vec2& other) const {
		return { x - other.x, y - other.y };
	}

	vec2 operator *(T scalar) const {
		return { x * scalar, y * scalar };
	}

	vec2 operator /(T scalar) const {
		return { x / scalar, y / scalar };
	}

	// Compound assignment operators
	vec2& operator += (const vec2& other) {
		x += other.x;
		y += other.y;
		return *this;
	}

	vec2& operator -= (const vec2& other) {
		x -= other.x;
		y -= other.y;
		return *this;
	}

	vec2& operator *= (T scalar) {
		x *= scalar;
		y *= scalar;
		return *this;
	}

	vec2& operator /= (T scalar) {
		x /= scalar;
		y /= scalar;
		return *this;
	}
};

using p = vec2<float>;
using ui2 = vec2<unsigned int>;


//sum of products, is also equal to v1*v2*cos(theta)
template<typename T>
T dot2(const vec2<T>& v1, const vec2<T>& v2) {
	return v1.x * v2.x + v1.y * v2.y;
}


template<typename T>
T cross2(const vec2<T>& v1, const vec2<T>& v2) {
	return v1.x * v2.y - v1.y * v2.x;
}





template<typename T>
struct vec3 {
	T x, y, z;

	vec3() : x(0), y(0), z(0) {} //initializing paramenters to 0 if not specified 

	vec3(T x_, T y_, T z_) : x(x_), y(y_), z(z_) {} //normal constructor

	// operator overloads
	bool operator == (const vec3& other) const {
		return x == other.x && y == other.y && z == other.z;
	}

	bool operator!=(const vec3& other) const {
		return !(*this == other);
	}
};

using p3 = vec3<float>;
using ui3 = vec3<unsigned int>;





//////////////// variables
inline p cursor = { 0,0 };
#define PI 3.14159265358979323846	//constexpr?

inline float windowHeight = 1080;
inline float windowWidth = 1920;

inline bool isRunning = true;

inline float inv36 = 1.0f / 36;



inline void getCursorPos(GLFWwindow* window) {

	double intermX, intermY;

	glfwGetCursorPos(window, &intermX, &intermY);

	cursor = { static_cast<float>(intermX), static_cast<float>(windowHeight - intermY) };

}


#define printv2(var) printv2_without_macro(#var, var)
template<typename T>
void printv2_without_macro(const string& name, const vector<vec2<T>>& items) {
	stringstream ss;
	ss << name << endl;
	for (size_t i = 0; i < items.size(); ++i) {
		ss << "{" << items[i].x << "," << items[i].y << "}";

		if (i != items.size() - 1) { // Check if it's not the last element
			ss << ",";
		}
	}
	cout << ss.str() << endl << endl;
}

#define printflat(var) printflat_without_macro(#var, var)
template<typename T>
void printflat_without_macro(const string& name, const vector<T>& items) {
	stringstream ss;
	ss << name << endl;
	ss << "{";
	for (size_t i = 0; i < items.size(); i++) {
		ss << items[i];

		if (i != items.size() - 1) { // Check if it's not the last element
			ss << ",";
		}
	}
	ss << "}";
	cout << ss.str() << endl << endl;
}

#define printflat2(var) printflat2_without_macro(#var, var)
template<typename T>
void printflat2_without_macro(const string& name, const vector<T>& items) {
	stringstream ss;
	ss << name << endl;
	for (size_t i = 0; i < items.size(); i += 2) {
		ss << "{" << items[i] << "," << items[i + 1] << "}";

		if (i != items.size() - 2) { // Check if it's not the last element
			ss << ",";
		}
	}
	cout << ss.str() << endl << endl;
}

#define printflat3(var) printflat3_without_macro(#var, var)
template<typename T>
void printflat3_without_macro(const string& name, const vector<T>& items) {
	stringstream ss;
	ss << name << endl;
	for (size_t i = 0; i < items.size(); i += 3) {
		ss << "{" << items[i] << "," << items[i + 1] << "," << items[i + 2] << "}";

		if (i != items.size() - 3) { // Check if it's not the last element
			ss << ",";
		}
	}
	cout << ss.str() << endl << endl;
}


#define printp(var) printp_without_macro(#var, var)
template<typename T>
void printp_without_macro(const string& name, const T& items) {
	stringstream ss;
	ss << name << endl;
	ss << "{" << items.x << "," << items.y << "}";

	cout << ss.str() << endl << endl;
}

//consts, cambiale el nombre y deja el nombre "is" para bools
inline float isRightOfLine(p& A, p& B, p& P) {		 //is P to the right of AB?
	p AB = { B.x - A.x,  B.y - A.y };
	p AP = { P.x - A.x, P.y - A.y };

	return AB.x * AP.y - AB.y * AP.x;	//if negative it is to its right, if 0, P is on the infinite line of AB
}

//aun con p chatgpt sigue diciendo que referencia podría no ser lo mejor. También sigue diciendo que un valor positivo es que p2 está a la derecha
template<typename T>
inline float crossProduct(const vec2<T>& p0, const vec2<T>& p1, const vec2<T>& p2) {
	vec2<T> v01 = { p1.x - p0.x, p1.y - p0.y };
	vec2<T> v12 = { p2.x - p1.x, p2.y - p1.y };

	return v01.x * v12.y - v01.y * v12.x;
}

template<typename T>
vec3<T> crossProduct3(const vec3<T>& v1, const vec3<T>& v2) {
	return vec3<T>(
		v1.y * v2.z - v1.z * v2.y,
		v1.z * v2.x - v1.x * v2.z,
		v1.x * v2.y - v1.y * v2.x 
	);
}