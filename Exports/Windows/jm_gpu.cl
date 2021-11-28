int side(float a_x, float a_y, float b_x, float b_y, float c_x, float c_y)
{
    float side = (c_y - a_y) * (b_x - a_x) - (b_y - a_y) * (c_x - a_x);
    if (side > 0) return 1;
    if (side < 0) return -1;
    return 0;
}

void kernel jm_gpu(
    global float* points_x, 
    global float* points_y,
    global int* size,
    global int* d_size,
    global float* global_ch_x, 
    global float* global_ch_y
) {

    int i = get_global_id(0);
    if (i > ceil(*size/ (float)*d_size) - 1) return;

    int lo = i * *d_size;
    int hi = (((lo + *d_size - 1) >= *size) * (*size - 1)) + (((lo + *d_size - 1) < *size) * (lo + *d_size - 1));
    int g_lo = i * (*d_size + 2);
    int g_hi = (i * (*d_size + 2)) + *d_size - 1;

    if (hi + 1 - lo < 2)
    {
        global_ch_x[g_lo] = points_x[lo];
        global_ch_y[g_lo] = points_y[lo];
        global_ch_x[g_lo + 1] = global_ch_x[g_lo];
        global_ch_y[g_lo + 1] = global_ch_y[g_lo];
        global_ch_x[g_hi + 2] = 1;
        return;
    }

    int l = lo;
    int p = l, q;
    int ch_size = 0;
    do
    {
        global_ch_x[g_lo + ch_size] = points_x[p];
        global_ch_y[g_lo + ch_size] = points_y[p];
        ++ch_size;
        
        q = ((p + 1) % (hi + 1)) + ((((p + 1) % (hi + 1)) == 0) * lo);
        for (int j = lo; j < hi + 1; j++)
        {
            if (side(points_x[p], points_y[p], points_x[j], points_y[j],
                points_x[q], points_y[q]) < 0)
                q = j;
        }
        p = q;
        
    } while (p != l); 

    global_ch_x[g_lo + ch_size] = global_ch_x[g_lo];
    global_ch_y[g_lo + ch_size] = global_ch_y[g_lo];
    global_ch_x[g_hi + 2] = ch_size;
}