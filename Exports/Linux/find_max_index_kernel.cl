
int side(float* p1, float* p2, float p_x, float p_y)
{
	float side = (p_y - p1[1]) * (p2[0] - p1[0]) - (p2[1] - p1[1]) * (p_x - p1[0]);
	if (side > 0) return 1;
	if (side < 0) return -1;
	return 0;
}


float dist(float* p1, float* p2, float p_x, float p_y)
{
    return fabs((p_y - p1[1]) * (p2[0] - p1[0]) - (p2[1] - p1[1]) * (p_x - p1[0]));
}


void find_max_index(float* p1, float* p2, float p_x, float p_y, int _side, int candidate_index, int* max_index, float* max_distance)
{
	if (side(p1, p2, p_x, p_y) != _side) return;

	float candidate_distance = dist(p1, p2, p_x, p_y);
	if (candidate_distance <= *max_distance) return;

	*max_index = candidate_index;
	*max_distance = candidate_distance;
    return;
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

    if (get_global_id(0) > *size-1) return;

    int max_index = -1;
    float max_distance = 0;

    const int g_id = get_global_id(0);

    find_max_index(p1, p2, points_x[g_id], points_y[g_id], *_side, g_id, &max_index, &max_distance);

    float max_group_distance = work_group_reduce_max(max_distance);

    if (max_distance != max_group_distance)
    {
        max_index = -1;
    }

    int max_group_index = work_group_reduce_max(max_index);

    if (get_local_id(0) == 0)
    {
        result_index[get_group_id(0)] = max_group_index;
        result_distance[get_group_id(0)] = max_group_distance;
    }
}
