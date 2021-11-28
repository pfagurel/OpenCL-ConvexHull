#include "QuickHullGPU.h"


cl::Program QuickHullGPU::make_program_from_file(std::shared_ptr<std::ifstream> sourceFile, cl::Context& context)
{
	std::string sourceCode(std::istreambuf_iterator<char>(*sourceFile.get()), (std::istreambuf_iterator<char>()));
	cl::Program::Sources source{ sourceCode };
	return cl::Program(context, source);
}

void QuickHullGPU::print_platform_vendors(std::vector<cl::Platform>& platforms)
{
	// Print number of platforms and list of platforms
	std::cout << "Platform number is: " << platforms.size() << std::endl;
	std::string platformVendor;
	for (unsigned int i = 0; i < platforms.size(); ++i) {
		platforms[i].getInfo((cl_platform_info)CL_PLATFORM_VENDOR, &platformVendor);
		std::cout << "Platform is by: " << platformVendor << std::endl;
	}
}

void QuickHullGPU::print_devices(std::vector<cl::Device>& devices)
{
	// Print number of devices and list of devices
	std::cout << "Device number is: " << devices.size() << std::endl;
	for (unsigned int i = 0; i < devices.size(); ++i) {
		std::cout << "Device #" << i << ": " << devices[i].getInfo<CL_DEVICE_NAME>() << " " << devices[i].getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>() << " "
			<< devices[i].getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>() << " " << std::endl;
	}
}



void QuickHullGPU::init()
{
	cl::Platform::get(&platforms);
	if (platforms.size() == 0) {
		std::cout << "Platform size 0\n";
		return;
	}

	cl_context_properties properties[] =
	{
			CL_CONTEXT_PLATFORM,
			(cl_context_properties)(platforms[0])(),
			0
	};
	context = cl::Context(CL_DEVICE_TYPE_ALL, properties);

	devices = context.getInfo<CL_CONTEXT_DEVICES>();

	program = make_program_from_file(std::shared_ptr<std::ifstream>(new std::ifstream("find_max_index_kernel.cl")), context);

	program.build(devices);

	kernel = cl::Kernel(program, "find_max_index_kernel", &err);

	queue = cl::CommandQueue(context, devices[0], 0, &err);

	max_compute_units = devices[0].getInfo<CL_DEVICE_MAX_COMPUTE_UNITS >();
	max_work_group_size = devices[0].getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>();
}

void QuickHullGPU::test()
{
	print_platform_vendors(platforms);

	print_devices(devices);
}

std::vector<Point> QuickHullGPU::quick_hull(float* points_x, float* points_y, int size)
{
	ndrange_size = ((size % 2 == 0) * size + (size % 2 != 0) * (size + 1));
	ndrange_group_size = ((ndrange_size <= max_work_group_size) * ndrange_size) + ((ndrange_size > max_work_group_size) * max_work_group_size);

	if (ndrange_group_size < ndrange_size)
		ndrange_size += ndrange_group_size - (ndrange_size % ndrange_group_size);

	result_index = new int[ndrange_size / ndrange_group_size];
	result_distance = new float[ndrange_size / ndrange_group_size];

	std::vector<Point> result;

	if (size <= 2) return result;

	float left[2] = { points_x[0],points_y[0] };
	float right[2] = { points_x[0],points_y[0] };

	for (int i = 1; i < size; i++)
	{
		if (points_x[i] < left[0])
		{
			left[0] = points_x[i];
			left[1] = points_y[i];
		}
		if (points_x[i] > right[0])
		{
			right[0] = points_x[i];
			right[1] = points_y[i];
		}
	}

	
	
	buffer_POINTS_X = cl::Buffer(context, CL_MEM_READ_ONLY, size * sizeof(float));
	buffer_POINTS_Y = cl::Buffer(context, CL_MEM_READ_ONLY, size * sizeof(float));
	buffer_SIZE = cl::Buffer(context, CL_MEM_READ_ONLY, sizeof(int));
	buffer_P1 = cl::Buffer(context, CL_MEM_READ_ONLY, 2 * sizeof(float));
	buffer_P2 = cl::Buffer(context, CL_MEM_READ_ONLY, 2 * sizeof(float));
	buffer_SIDE = cl::Buffer(context, CL_MEM_READ_ONLY, sizeof(int));
	buffer_RESULT_INDEX = cl::Buffer(context, CL_MEM_WRITE_ONLY, (ndrange_size/ndrange_group_size) * sizeof(int));
	buffer_RESULT_DISTANCE = cl::Buffer(context, CL_MEM_WRITE_ONLY, (ndrange_size/ndrange_group_size) * sizeof(float));

	queue.enqueueWriteBuffer(buffer_POINTS_X, CL_TRUE, 0, size * sizeof(float), points_x);
	queue.enqueueWriteBuffer(buffer_POINTS_Y, CL_TRUE, 0, size * sizeof(float), points_y);

	// Set the kernel arguments
	kernel.setArg(0, buffer_POINTS_X);
	kernel.setArg(1, buffer_POINTS_Y);
	kernel.setArg(2, buffer_SIZE);
	kernel.setArg(3, buffer_P1);
	kernel.setArg(4, buffer_P2);
	kernel.setArg(5, buffer_SIDE);
	kernel.setArg(6, buffer_RESULT_INDEX);
	kernel.setArg(7, buffer_RESULT_DISTANCE);

	find_hull(points_x, points_y, size, left, right, 1,result);
	find_hull(points_x, points_y, size, left, right, -1,result);

	return result;
}

void QuickHullGPU::find_hull(float* points_x, float* points_y, int size, float* p1, float* p2, int _side, std::vector<Point> &result )
{
	queue.enqueueWriteBuffer(buffer_SIZE, CL_TRUE, 0, sizeof(int), &size);
	queue.enqueueWriteBuffer(buffer_P1, CL_TRUE, 0, 2 * sizeof(float), p1);
	queue.enqueueWriteBuffer(buffer_P2, CL_TRUE, 0, 2 * sizeof(float), p2);
	queue.enqueueWriteBuffer(buffer_SIDE, CL_TRUE, 0, sizeof(int), &_side);

	

	queue.enqueueNDRangeKernel(
		kernel,
		cl::NullRange,
		cl::NDRange(ndrange_size),
		cl::NDRange(ndrange_group_size), //should be computed according to the points size
		NULL,
		&event);


	queue.enqueueReadBuffer(buffer_RESULT_INDEX, CL_TRUE, 0, (ndrange_size / ndrange_group_size) * sizeof(int), result_index);
	queue.enqueueReadBuffer(buffer_RESULT_DISTANCE, CL_TRUE, 0, (ndrange_size / ndrange_group_size) * sizeof(float), result_distance);



	int max_index = -1;
	float max_distance = 0.0;
	float candidate_p[2] = { 0,0 };

	for (int i = 0; i < ndrange_size / ndrange_group_size; i++)
	{
		if (result_index[i] < 0) continue;
		candidate_p[0] = points_x[result_index[i]];
		candidate_p[1] = points_y[result_index[i]];
		find_max_index(p1, p2, candidate_p, _side, result_index[i], max_index, max_distance);
	}

	if (max_index < 0)
	{
		result.push_back(Point(p1[0],p1[1]));
		result.push_back(Point(p2[0], p2[1]));
		return;
	}

	float max_index_p[2] = { points_x[max_index],points_y[max_index] };
	int new_side = side(max_index_p, p1, p2);
	find_hull(points_x, points_y, size, max_index_p, p1, -new_side, result);
	find_hull(points_x, points_y, size, max_index_p, p2, new_side, result);

}


void QuickHullGPU::find_max_index(float* p1, float* p2, float* candidate_p, int _side, int candidate_index, int& max_index, float& max_distance)
{
	if (side(p1, p2, candidate_p) != _side) return;

	float candidate_distance = distance(p1, p2, candidate_p);
	if (candidate_distance <= max_distance) return;

	max_index = candidate_index;
	max_distance = candidate_distance;
}

int QuickHullGPU::side(float* p1, float* p2, float* p)
{
	float side = (p[1] - p1[1]) * (p2[0] - p1[0]) - (p2[1] - p1[1]) * (p[0] - p1[0]);
	if (side > 0) return 1;
	if (side < 0) return -1;
	return 0;
}

float QuickHullGPU::distance(float* p1, float* p2, float* p)
{
	return abs((p[1] - p1[1]) * (p2[0] - p1[0]) - (p2[1] - p1[1]) * (p[0] - p1[0]));
}