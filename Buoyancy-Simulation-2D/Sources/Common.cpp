#pragma once

//This is created because wettedSurfaces.hpp doesn't let me include utilities, so till I know how to resolve that I am creating common that doesn't
//depend on other project files

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <cmath>
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






template<typename T>
struct vec2 {
	T x, y;

	vec2() : x(0), y(0) {} //initializing paramenters to 0 if not specified 

	vec2(T x, T y) : x(x), y(y) {} //normal constructor

	// operator overloads
    bool operator == (const vec2& other) const {
        return x == other.x && y == other.y;
    }

    bool operator!=(const vec2& other) const {
        return !(*this == other);
    }
};

using p = vec2<float>;
using ui2 = vec2<unsigned int>;



//////////////// variables
inline p cursor = { 0,0 };
#define PI 3.14159265358979323846	//constexpr?

inline float windowHeight = 1080;
inline float windowWidth = 1920;






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

#define printflat2(var) printflat2_without_macro(#var, var)
template<typename T>
void printflat2_without_macro(const string& name, const vector<T>& items) {
    stringstream ss;
    ss << name << endl;
    for (size_t i = 0; i < items.size(); ++i) {
        ss << items[i];

        if (i != items.size() - 1) { // Check if it's not the last element
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
inline float newisRightOfLine(p& A, p& B, p& P) {		 //is P to the right of AB?
    p AB = { B.x - A.x,  B.y - A.y };
    p AP = { P.x - A.x, P.y - A.y };

    return AB.x * AP.y - AB.y * AP.x;	//if negative it is to its right, if 0, P is on the infinite line of AB
}