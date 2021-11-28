
int side(float p1[2], float p2[2], float p[2])
{
    float side = (p[1] - p1[1]) * (p2[0] - p1[0]) - (p2[1] - p1[1]) * (p[0] - p1[0]);
    if (side > 0) return 1;
    if (side < 0) return -1;
    return 0;
}

float distance(float p1[2], float p2[2], float p[2])
{
    return abs((p[1] - p1[1]) * (p2[0] - p1[0]) - (p2[1] - p1[1]) * (p[0] - p1[0]));
}

void find_max_index(float p1[2], float p2[2], float candidate_p[2], int _side, int candidate_index, int& max_index, float& max_distance)
{
    if (side(p1, p2, candidate_p) != _side) return;

    float candidate_distance = distance(p1, p2, candidate_p);
    if (candidate_distance <= max_distance) return;

    max_index = candidate_index;
    max_distance = candidate_distance;
}

kernel void find_max_index_kernel(
    global float* points_x,
    global float* points_y,
    global int* size, 
    global float* p1,
    global float* p2,
    global int* _side,
    global int* result_index,
    global float* result_distance
    ) 
{
    const int idx = get_global_id(0);
    const int idy = get_global_id(1);
    const int w_id = get_group_id(0);
    result_index[0] = 10;
}
