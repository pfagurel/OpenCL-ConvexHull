#include "convex_hull.h"

//https://www.geeksforgeeks.org/cpp-program-for-quicksort/

void swap(godot::Array& arr, int i, int j)
{
    godot::Variant t = arr[i];
    arr[i] = arr[j];
    arr[j] = t;
}


int partition(godot::Array& arr, int low, int high)
{
    godot::Vector2 pivot = arr[high];
    int i = (low - 1); 

    for (int j = low; j <= high - 1; j++)
    {
        if (((godot::Vector2) arr[j]).x < pivot.x || (((godot::Vector2)arr[j]).x == pivot.x && ((godot::Vector2)arr[j]).y < pivot.y))
        {
            i++;  
            swap(arr, i, j);
        }
    }
    swap(arr,i + 1, high);
    return (i + 1);
}


void quickSort(godot::Array& arr, int low, int high)
{
    if (low < high)
    {
        int pi = partition(arr, low, high);
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

using namespace godot;

void ConvexHullCPP::_register_methods() {
    register_method("compute_hull", &ConvexHullCPP::compute_hull);
    register_signal<ConvexHullCPP>((char*)"add_step", "v", GODOT_VARIANT_TYPE_ARRAY);
}

ConvexHullCPP::ConvexHullCPP() {
}

ConvexHullCPP::~ConvexHullCPP() {
}

void ConvexHullCPP::_init() {
    quick_hull_gpu.init();
    quick_hull_gpu.test();
}


Array ConvexHullCPP::compute_hull(Array points, godot_int d_size, godot_bool step)
{
    std::string filename("log.txt");
    std::ofstream file_out;

    file_out.open(filename, std::ios_base::app);
   
    quickSort(points, 0, points.size()-1);

    std::chrono::steady_clock::time_point begin;
    std::chrono::steady_clock::time_point end;


    Point* vec_points = new Point[points.size()];
    float* points_x = new float[points.size()];
    float* points_y = new float[points.size()];

    std::cout << "\t\t\t***Preprocessing***\n";

    for (int i = 0; i < points.size(); i++)
    {
        vec_points[i] = Point(Vector2(points[i]).x, Vector2(points[i]).y);
        points_x[i] = Vector2(points[i]).x;
        points_y[i] = Vector2(points[i]).y;
    }
   
    std::cout << "\t\t\t***CPU MergeHull***\n";
    MergeHull merge_hull;
    begin = std::chrono::steady_clock::now();
    std::vector<Point> merge_hull_result = merge_hull.divide(points_x, points_y, points.size());
    end = std::chrono::steady_clock::now();
    std::cout << "Execution time = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[micros]" << std::endl;
    std::cout << "Execution time = " << std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count() << "[ms]" << std::endl;
    file_out << std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count() << std::endl;

    std::cout << "\t\t\t***CPU MergeHull BottomUp***\n";
    begin = std::chrono::steady_clock::now();
    if (step)
    {
        merge_hull_result = merge_hull.bottom_up_with_step(points_x, points_y, points.size(), this);
    }
    else
    {
        merge_hull_result = merge_hull.bottom_up(points_x, points_y, points.size(), d_size);
    }
    end = std::chrono::steady_clock::now();
    std::cout << "Execution time = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[micros]" << std::endl;
    std::cout << "Execution time = " << std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count() << "[ms]" << std::endl;
    file_out << std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count() << std::endl;

    std::cout << "\t\t\t***GPUA MergeHull***\n";
    merge_hull.init();
    begin = std::chrono::steady_clock::now();
    merge_hull_result = merge_hull.bottom_up_gpua(points_x, points_y, points.size(), d_size);
    end = std::chrono::steady_clock::now();
    std::cout << "Execution time = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[micros]" << std::endl;
    std::cout << "Execution time = " << std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count() << "[ms]" << std::endl;
    file_out << std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count() << std::endl;

    std::cout << "\t\t\t***CPU QuickHull***\n";
    QuickHullCPU quick_hull_cpu;
    begin = std::chrono::steady_clock::now();
    std::vector<Point> quick_hull_result = quick_hull_cpu.quick_hull(vec_points, points.size());
    end = std::chrono::steady_clock::now();
    std::cout << "Execution time = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[micros]" << std::endl;
    std::cout << "Execution time = " << std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count() << "[ms]" << std::endl;
    file_out << std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count() << std::endl;

    std::cout << "\t\t\t***GPUA QuickHull***\n";
    begin = std::chrono::steady_clock::now();
    std::vector<Point> gpu_result = quick_hull_gpu.quick_hull(points_x, points_y, points.size());
    end = std::chrono::steady_clock::now();
    std::cout << "Execution time = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[micros]" << std::endl;
    std::cout << "Execution time = " << std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count() << "[ms]" << std::endl;
    file_out << std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count()  << std::endl << "=========================================" << std::endl;

    

    Array result_array;
    //std::cout << (result.size() == gpu_result.size());
    for (Point p : merge_hull_result) {
        result_array.push_back(Vector2(p.x, p.y));
    }
    std::cout << "=====================================================\n";
    delete[] vec_points;
    delete[] points_x;
    delete[] points_y;

    file_out.close();

    return result_array;
}

