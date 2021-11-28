// QuickHullGPU.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//
#pragma once

#include <CL/cl2.hpp>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <fstream>
#include "Point.h"


class QuickHullGPU
{
private:
	int max_compute_units;
	int max_work_group_size;

	cl_int err = CL_SUCCESS;
	cl::Event event;

	std::vector<cl::Platform> platforms;
	std::vector<cl::Device> devices;
	cl::Context context;
	cl::Program program;
	cl::Kernel kernel;
	cl::CommandQueue queue;
	cl::Buffer buffer_POINTS_X;
	cl::Buffer buffer_POINTS_Y;
	cl::Buffer buffer_SIZE;
	cl::Buffer buffer_P1;
	cl::Buffer buffer_P2;
	cl::Buffer buffer_SIDE;
	cl::Buffer buffer_RESULT_INDEX;
	cl::Buffer buffer_RESULT_DISTANCE;

	int* result_index;
	float* result_distance;

	int ndrange_size;
	int ndrange_group_size;

	cl::Program make_program_from_file(std::shared_ptr<std::ifstream> sourceFile, cl::Context& context);


	void print_platform_vendors(std::vector<cl::Platform>& platforms);


	void print_devices(std::vector<cl::Device>& devices);

	void find_hull(float* points_x, float* points_y, int size, float* p1, float* p2, int _side, std::vector<Point>& result);

	void find_max_index(float* p1, float* p2, float* candidate_p, int _side, int candidate_index, int& max_index, float& max_distance);

	int side(float* p1, float* p2, float* p);

	float distance(float* p1, float* p2, float* p);

	


public:
	void init();

	void test();

	std::vector<Point> quick_hull(float* points_x, float* points_y, int size);

	~QuickHullGPU()
	{
		delete[] result_index;
		delete[] result_distance;
	}
};



