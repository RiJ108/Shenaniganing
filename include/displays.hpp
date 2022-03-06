#pragma once

#include <iostream>
#include <string>
#include <glm/glm.hpp>

#include "constant.hpp"

using namespace std;
using namespace glm;

class Display {
public:
	static void disp(GRIDCELL aGridcell) {
		cout << "Coordinates :\t Value :\n";
		for(int i = 0; i < 8; i++)
			cout << i << "_\t" << aGridcell.p[i].x << ", " << aGridcell.p[i].y << ", " << aGridcell.p[i].z << "\t" << aGridcell.val[i] << endl;
	}

	static void disp(string values) {
		cout << values;
	}
	static void dispLn(string values) {
		disp(values);
		cout << endl;
	}

	static void disp(unsigned int values) {
		cout << values;
	}
	static void dispLn(unsigned int values) {
		disp(values);
		cout << endl;
	}

	static void disp(ivec2 values) {
		cout << "[ " << values.x << ", " << values.y << "] ";
	}
	static void dispLn(ivec2 values) {
		disp(values);
		cout << endl;
	}

	static void disp(ivec3 values) {
		cout << "[ " << values.x << ", " << values.y << ", " << values.z << "] ";
	}
	static void dispLn(ivec3 values) {
		disp(values);
		cout << endl;
	}

	static void disp(vec2 values) {
		cout << "[ " << values.x << ", " << values.y << "] ";
	}
	static void dispLn(vec2 values) {
		disp(values);
		cout << endl;
	}

	static void disp(vec3 values) {
		cout << "[ " << values.x << ", " << values.y << ", " << values.z << "] ";
	}
	static void dispLn(vec3 values) {
		disp(values);
		cout << endl;
	}

	static void disp(vec4 values) {
		cout << "[ " << values.x << ", " << values.y << ", " << values.z << ", " << values.w << "] ";
	}
	static void dispLn(vec4 values) {
		disp(values);
		cout << endl;
	}
};