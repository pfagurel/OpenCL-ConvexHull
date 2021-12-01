#pragma once
#include <CL/cl2.hpp>
#include <fstream>
#include <utility>
#include <vector>
#include <set>
#include <algorithm>
#include <iostream>
#include "Point.h"
#include <math.h> 
#include <Godot.hpp>
#include <Node.hpp>

class MergeHull
{
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
	cl::Buffer buffer_D_SIZE;
	cl::Buffer buffer_GLOBAL_CH_X;
	cl::Buffer buffer_GLOBAL_CH_Y;

	int ndrange_size;
	int ndrange_group_size;

	cl::Program make_program_from_file(std::shared_ptr<std::ifstream> sourceFile, cl::Context& context);


	void print_platform_vendors(std::vector<cl::Platform>& platforms);


	void print_devices(std::vector<cl::Device>& devices);

    void upper_bottom_points(int m, std::vector<Point>& V, int n, std::vector<Point>& W, int* t1, int* t2, int* t3, int* t4);

    std::vector<Point> merger(std::vector<Point >& a, std::vector<Point>& b);

    std::vector<Point> jm(float* points_x, float* points_y, int size);

    void jm(float* points_x, float* points_y, int lo, int hi, int g_lo, int g_hi, std::vector<Point>& global_ch);

	void jm_gpua(float* points_x, float* points_y,int size, int d_size,  float* global_ch_x, float* global_ch_y, int global_size );

public:
	void init();

    std::vector<Point> divide(float* points_x, float* points_y, int size);

    std::vector<Point> bottom_up(float* points_x, float* points_y, int size, int d_size);

	std::vector<Point> bottom_up_with_step(float* points_x, float* points_y, int size, godot::Node* node);

	std::vector<Point> bottom_up_gpua(float* points_x, float* points_y, int size, int d_size);
};

