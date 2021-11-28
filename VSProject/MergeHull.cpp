#include "MergeHull.h"

int side(Point a, Point b, Point c)
{
    float side = (c.y - a.y) * (b.x- a.x) - (b.y - a.y) * (c.x - a.x);
    if (side > 0) return 1;
    if (side < 0) return -1;
    return 0;
}

void MergeHull::upper_bottom_points(int m, std::vector<Point>& V, int n, std::vector<Point>& W, int* t1, int* t2, int* t3, int* t4)
{
    int r1 = 0;
    int l2 = 0;

    for (int i = 1; i < m; i++)
        if (V[i].x > V[r1].x)
            r1 = i;

    int ix1 = r1, ix2 = l2;
    bool done = 0;
    while (!done)
    {
        done = 1;
        if(m>1)
        while (side(W[ix2], V[ix1], V[(ix1 + 1) % m]) >= 0)
            ix1 = (ix1 + 1) % m;
        if(n>1)
        while (side(V[ix1], W[ix2], W[(n + ix2 - 1) % n]) <= 0)
        {
            ix2 = (n + ix2 - 1) % n;
            done = 0;
        }
    }

    int uppera = ix1, upperb = ix2;
    ix1 = r1, ix2 = l2;
    done = 0;
    while (!done)
    {
        done = 1;
        if(n>1)
        while (side(V[ix1], W[ix2], W[(ix2 + 1) % n]) >= 0)
            ix2 = (ix2 + 1) % n;
        if(m>1)
        while (side(W[ix2], V[ix1], V[(m + ix1 - 1) % m]) <= 0)
        {
            ix1 = (m + ix1 - 1) % m;
            done = 0;
        }
    }

    *t1 = uppera;
    *t2 = upperb;
    *t3 = ix1;
    *t4 = ix2;

    return;
}
// Merge the two polygons 'a' and 'b'
// represented as two vectors.
std::vector<Point> MergeHull::merger(std::vector<Point >& a,
    std::vector<Point>& b)
{
    int n1 = a.size();
    int n2 = b.size();
    int uppera = 0; int upperb = 0;
    int lowera = 0; int lowerb = 0;

    upper_bottom_points(n1, a, n2, b, &uppera, &upperb, &lowera, &lowerb);
    std::vector<Point> ret;

    int ind = n1-1; //leftmost-1

    while (ind != lowera)
    {
        ind = (ind + 1) % n1;
        ret.push_back(a[ind]);
    }

    ind = lowerb;
    ret.push_back(b[lowerb]);

    while (ind != upperb)
    {
        ind = (ind + 1) % n2;
        ret.push_back(b[ind]);
    }

    ind = uppera;
    if (0 != uppera)
    {
        ret.push_back(a[uppera]);
        while (ind != n1 - 1)
        {
            ind = (ind + 1) % n1;
            ret.push_back(a[ind]);
        }
    }
    return ret;
}


// Returns the convex hull for the given set of points
std::vector<Point> MergeHull::divide(float* points_x, float* points_y, int size)
{
    if (size <= 5)
        return jm(points_x, points_y, size);

    float* left_x = new float[size / 2];
    float* left_y = new float[size / 2];
    float* right_x = new float[(int)ceil(size / 2.0)];
    float* right_y = new float[(int)ceil(size / 2.0)];

    for (int i = 0; i < size / 2; i++)
    {
        left_x[i] = points_x[i];
        left_y[i] = points_y[i];
    }


    for (int i = floor(size / 2.0); i < size; i++)
    {
        right_x[i - (int)floor(size / 2.0)] = points_x[i];
        right_y[i - (int)floor(size / 2.0)] = points_y[i];
    }

    std::vector<Point> left_hull = divide(left_x, left_y, size / 2);
    std::vector<Point> right_hull = divide(right_x, right_y, ceil(size / 2.0));

    delete[] left_x;
    delete[] left_y;
    delete[] right_x;
    delete[] right_y;

    return merger(left_hull, right_hull);
}

// Jarvi's March algorithm to find convex hull
std::vector<Point> MergeHull::jm(float* points_x, float* points_y, int size) {
    std::vector<Point> ch_v;

    if (size < 2)
    {
        ch_v.push_back(Point(points_x[0], points_y[0]));

        return ch_v;
    }
    int l = 0;
    int p = l, q;
    float d = 0;
    do
    {
        ch_v.push_back(Point(points_x[p], points_y[p]));
        q = (p + 1) % size;

        for (int i = 0; i < size; i++)
        {
            if (side(Point(points_x[p], points_y[p]), Point(points_x[i], points_y[i]), Point(points_x[q], points_y[q])) < 0)
                q = i;
        }
        p = q;
    } while (p != l);
    return ch_v;
}

void MergeHull::jm(float* points_x, float* points_y, int lo, int hi, int g_lo, int g_hi, std::vector<Point>& global_ch) {

    if (hi + 1 - lo < 2)
    {
        global_ch[g_lo] = Point(points_x[lo], points_y[lo]);
        global_ch[g_lo + 1] = global_ch[g_lo];
        global_ch[g_hi + 2] = Point(1, -1);
        return;
    }

    int l = lo;
    int p = l, q;
    int ch_size = 0;
    do
    {
        global_ch[g_lo + ch_size] = Point(points_x[p], points_y[p]);
        ++ch_size;

        q = (p + 1) % (hi + 1) + ((p + 1) % (hi + 1) == 0) * lo;
        for (int i = lo; i < hi + 1; i++)
        {
            if (side(Point(points_x[p], points_y[p]), Point(points_x[i], points_y[i]),
                Point(points_x[q], points_y[q])) < 0)
                q = i;
        }
        p = q;
    } while (p != l);

    global_ch[g_lo + ch_size] = global_ch[g_lo];
    global_ch[g_hi + 2] = Point(ch_size, -1);
}

void MergeHull::jm_gpua(float* points_x, float* points_y, int size, int d_size, float* global_ch_x, float* global_ch_y, int global_size)
{
    int ch_count = ceil(size / (float)d_size);

    ndrange_size = ((ch_count % 2 == 0) * ch_count + (ch_count % 2 != 0) * (ch_count + 1));
    ndrange_group_size = ((ndrange_size <= max_work_group_size) * ndrange_size) + ((ndrange_size > max_work_group_size) * max_work_group_size);

    if (ndrange_group_size < ndrange_size)
    ndrange_size += ndrange_group_size - (ndrange_size % ndrange_group_size);

    buffer_POINTS_X = cl::Buffer(context, CL_MEM_READ_ONLY, size * sizeof(float));
    buffer_POINTS_Y = cl::Buffer(context, CL_MEM_READ_ONLY, size * sizeof(float));
    buffer_SIZE = cl::Buffer(context, CL_MEM_READ_ONLY, sizeof(int));
    buffer_D_SIZE = cl::Buffer(context, CL_MEM_READ_ONLY, sizeof(int));
    buffer_GLOBAL_CH_X = cl::Buffer(context, CL_MEM_WRITE_ONLY, global_size * sizeof(float));
    buffer_GLOBAL_CH_Y = cl::Buffer(context, CL_MEM_WRITE_ONLY, global_size * sizeof(float));
   
    queue.enqueueWriteBuffer(buffer_POINTS_X, CL_TRUE, 0, size * sizeof(float), points_x);
    queue.enqueueWriteBuffer(buffer_POINTS_Y, CL_TRUE, 0, size * sizeof(float), points_y);
    queue.enqueueWriteBuffer(buffer_SIZE, CL_TRUE, 0, sizeof(int), &size);
    queue.enqueueWriteBuffer(buffer_D_SIZE, CL_TRUE, 0, sizeof(int), &d_size);

    // Set the kernel arguments
    kernel.setArg(0, buffer_POINTS_X);
    kernel.setArg(1, buffer_POINTS_Y);
    kernel.setArg(2, buffer_SIZE);
    kernel.setArg(3, buffer_D_SIZE);
    kernel.setArg(4, buffer_GLOBAL_CH_X);
    kernel.setArg(5, buffer_GLOBAL_CH_Y);
    
    queue.enqueueNDRangeKernel(
        kernel,
        cl::NullRange,
        cl::NDRange(ndrange_size),
        cl::NDRange(ndrange_group_size), //should be computed according to the points size
        NULL,
        &event);

    queue.enqueueReadBuffer(buffer_GLOBAL_CH_X, CL_TRUE, 0, global_size * sizeof(float), global_ch_x);
    queue.enqueueReadBuffer(buffer_GLOBAL_CH_Y, CL_TRUE, 0, global_size * sizeof(float), global_ch_y);

 
}

std::vector<Point> MergeHull::bottom_up(float* points_x, float* points_y, int size, godot::Node* node)
{
    float d_size = 5.0;
    if (size <= d_size)
    {
        return jm(points_x, points_y, size);
    }
    const float v_size = d_size + 2;

    int global_size = (ceil(size / d_size) * 2) + (ceil(size / d_size) * d_size);

    std::vector<Point> global_ch(global_size);
    std::vector<Point> result;

    std::vector<Point> left_hull;
    std::vector<Point> right_hull;

    for (int sz = d_size; sz < size; sz = sz + sz)
        for (int lo = 0; lo < size - sz; lo += sz + sz)
        {
            left_hull.clear();
            right_hull.clear();

            int mid = (lo + sz - 1);
            int hi = std::min(lo + sz + sz - 1, size - 1);
            int global_lo = (ceil(lo / d_size) * 2) + lo;
            int global_mid = mid + (ceil((mid + 1) / d_size) * 2);
            int global_hi = (ceil((hi + d_size - (hi - mid) + 1) / d_size) * 2) + hi + d_size - (hi - mid) - 2;

            if (sz == d_size)
            {
                jm(points_x, points_y, lo, mid, global_lo, global_mid-2, global_ch);
            }

            if (std::min(lo + sz + sz - 1, size - 1) - (lo + sz - 1) <= d_size)
            {
                jm(points_x, points_y, mid+1, hi, global_mid+1, global_hi, global_ch);
            }

            for (int i = ceil(lo / d_size) + 1; i < ceil(mid / d_size) + 1; i++)
            {
                for (int j = 0; j < global_ch[(i * v_size) - 1].x; j++)
                    left_hull.push_back(global_ch[j + ((i - 1) * v_size)]);
            }

            for (int i = ceil(mid / d_size) + 1; i < ceil((hi + 1) / 5.0) + 1; i++)
            {
                for (int j = 0; j < global_ch[(i * v_size) - 1].x; j++)
                    right_hull.push_back(global_ch[j + ((i - 1) * v_size)]);
            }

            result = merger(left_hull, right_hull);

            int r = 0;
            int smaller;
            for (int i = ceil(lo / d_size) + 1; i < ceil((hi + 1) / d_size) + 1; i++)
            {
                smaller = true;
                for (int j = 0; j < d_size; j++)
                {
                    if (r > result.size() - 1)
                    {
                        global_ch[(i * v_size) - 1].x = j;
                        smaller = false;
                        break;
                    }
                    global_ch[j + ((i - 1) * v_size)] = result[r];
                    r++;
                }

                if (smaller) global_ch[(i * v_size) - 1].x = d_size;
            }
        }
    return result;
}

std::vector<Point> MergeHull::bottom_up_with_step(float* points_x, float* points_y, int size, godot::Node* node)
{
    float d_size = 5.0;
    if (size <= d_size)
    {
        return jm(points_x, points_y, size);
    }
    const float v_size = d_size + 2;

    int global_size = (ceil(size / d_size) * 2) + (ceil(size / d_size) * d_size);

    std::vector<Point> global_ch(global_size);
    std::vector<Point> result;

    std::vector<Point> left_hull;
    std::vector<Point> right_hull;

    for (int sz = d_size; sz < size; sz = sz + sz)
        for (int lo = 0; lo < size - sz; lo += sz + sz)
        {
            left_hull.clear();
            right_hull.clear();

            int mid = (lo + sz - 1);
            int hi = std::min(lo + sz + sz - 1, size - 1);
            int global_lo = (ceil(lo / d_size) * 2) + lo;
            int global_mid = mid + (ceil((mid + 1) / d_size) * 2);
            int global_hi = (ceil((hi + d_size - (hi - mid) + 1) / d_size) * 2) + hi + d_size - (hi - mid) - 2;

            if (sz == d_size)
            {
                jm(points_x, points_y, lo, mid, global_lo, global_mid - 2, global_ch);
            }

            if (std::min(lo + sz + sz - 1, size - 1) - (lo + sz - 1) <= d_size)
            {
                jm(points_x, points_y, mid + 1, hi, global_mid + 1, global_hi, global_ch);
            }

            for (int i = ceil(lo / d_size) + 1; i < ceil(mid / d_size) + 1; i++)
            {
                for (int j = 0; j < global_ch[(i * v_size) - 1].x; j++)
                    left_hull.push_back(global_ch[j + ((i - 1) * v_size)]);
            }

            for (int i = ceil(mid / d_size) + 1; i < ceil((hi + 1) / 5.0) + 1; i++)
            {
                for (int j = 0; j < global_ch[(i * v_size) - 1].x; j++)
                    right_hull.push_back(global_ch[j + ((i - 1) * v_size)]);
            }

            godot::Array a;
            godot::Array b;
            godot::Array c;
            for (auto p:left_hull)
            {
                a.append(godot::Vector2(p.x, p.y));
            }

            node->emit_signal("add_step", (godot::Variant) a);

            for (auto p : right_hull)
            {
                b.append(godot::Vector2(p.x, p.y));
            }

            node->emit_signal("add_step", (godot::Variant) b);

            result = merger(left_hull, right_hull);
            for (auto p : result)
            {
                c.append(godot::Vector2(p.x, p.y));
            }

            node->emit_signal("add_step", (godot::Variant) c);

            int r = 0;
            int smaller;
            for (int i = ceil(lo / d_size) + 1; i < ceil((hi + 1) / d_size) + 1; i++)
            {
                smaller = true;
                for (int j = 0; j < d_size; j++)
                {
                    if (r > result.size() - 1)
                    {
                        global_ch[(i * v_size) - 1].x = j;
                        smaller = false;
                        break;
                    }
                    global_ch[j + ((i - 1) * v_size)] = result[r];
                    r++;
                }

                if (smaller) global_ch[(i * v_size) - 1].x = d_size;
            }
        }
    return result;
}

std::vector<Point> MergeHull::bottom_up_gpua(float* points_x, float* points_y, int size)
{
    float d_size = 5.0;
    if (size <= d_size)
    {
        return jm(points_x, points_y, size);
    }
    const int v_size = d_size + 2;

    int global_size = ceil(size / d_size) * 2 + (ceil(size / d_size) * d_size);

    float* global_ch_x = new float[global_size];
    float* global_ch_y = new float[global_size];

    std::vector<Point> result;

    std::vector<Point> left_hull;
    std::vector<Point> right_hull;

    jm_gpua(points_x,points_y, size,  (int) d_size, global_ch_x, global_ch_y,global_size);

    for (int sz = d_size; sz < size; sz = sz + sz)
        for (int lo = 0; lo < size - sz; lo += sz + sz)
        {
            left_hull.clear();
            right_hull.clear();
            int mid = (lo + sz - 1);
            int hi = std::min(lo + sz + sz - 1, size - 1);
            int global_lo = (ceil(lo / d_size) * 2) + lo;
            int global_mid = mid + (ceil((mid + 1) / d_size) * 2);
            int global_hi = (ceil((hi + d_size - (hi - mid) + 1) / d_size) * 2) + hi + d_size - (hi - mid) - 2;

            for (int i = ceil(lo / d_size) + 1; i < ceil(mid / d_size) + 1; i++)
            {
                for (int j = 0; j < global_ch_x[(i * v_size) - 1]; j++)
                    left_hull.push_back(Point(global_ch_x[j + ((i - 1) * v_size)], global_ch_y[j + ((i - 1) * v_size)]));
            }

            for (int i = ceil(mid / d_size) + 1; i < ceil((hi + 1) / 5.0) + 1; i++)
            {
                for (int j = 0; j < global_ch_x[(i * v_size) - 1]; j++)
                    right_hull.push_back(Point(global_ch_x[j + ((i - 1) * v_size)], global_ch_y[j + ((i - 1) * v_size)]));
            }

            result = merger(left_hull, right_hull);

            int r = 0;
            int smaller;
            for (int i = ceil(lo / d_size) + 1; i < ceil((hi + 1) / d_size) + 1; i++)
            {
                smaller = true;
                for (int j = 0; j < d_size; j++)
                {
                    if (r > result.size() - 1)
                    {
                        global_ch_x[(i * v_size) - 1] = j;
                        smaller = false;
                        break;
                    }
                    global_ch_x[j + ((i - 1) * v_size)] = result[r].x;
                    global_ch_y[j + ((i - 1) * v_size)] = result[r].y;
                    r++;
                }

                if (smaller) global_ch_x[(i * v_size) - 1] = d_size;
            }
        }
    return result;
}



cl::Program MergeHull::make_program_from_file(std::shared_ptr<std::ifstream> sourceFile, cl::Context& context)
{
    std::string sourceCode(std::istreambuf_iterator<char>(*sourceFile.get()), (std::istreambuf_iterator<char>()));
    cl::Program::Sources source{ sourceCode };
    return cl::Program(context, source);
}

void MergeHull::print_platform_vendors(std::vector<cl::Platform>& platforms)
{
    // Print number of platforms and list of platforms
    std::cout << "Platform number is: " << platforms.size() << std::endl;
    std::string platformVendor;
    for (unsigned int i = 0; i < platforms.size(); ++i) {
        platforms[i].getInfo((cl_platform_info)CL_PLATFORM_VENDOR, &platformVendor);
        std::cout << "Platform is by: " << platformVendor << std::endl;
    }
}

void MergeHull::print_devices(std::vector<cl::Device>& devices)
{
    // Print number of devices and list of devices
    std::cout << "Device number is: " << devices.size() << std::endl;
    for (unsigned int i = 0; i < devices.size(); ++i) {
        std::cout << "Device #" << i << ": " << devices[i].getInfo<CL_DEVICE_NAME>() << " " << devices[i].getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>() << " "
            << devices[i].getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>() << " " << std::endl;
    }
}


void MergeHull::init()
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

    program = make_program_from_file(std::shared_ptr<std::ifstream>(new std::ifstream("jm_gpu.cl")), context);

    program.build(devices);

    kernel = cl::Kernel(program, "jm_gpu", &err);

    queue = cl::CommandQueue(context, devices[0], 0, &err);

    max_compute_units = devices[0].getInfo<CL_DEVICE_MAX_COMPUTE_UNITS >();
    max_work_group_size = devices[0].getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>();
}