#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "utils.h"
#include <float.h>

const int width = 400;
const int height = 400;
pixel pixel_buffer[height][width];
float z_buffer[height][width];
vertex3 *vertices;
unsigned int *faces;
unsigned int vcount = 0;
unsigned int fcount = 0;
float t_matrix[4][4] = {0};
float model_matrix[4][4] = {0};
float view_matrix[4][4] = {0};
float projection_matrix[4][4] = {0};
float viewport_matrix[4][4] = {0};

void initialize()
{
    // initialize z-buffer to -inf
    for (int row = 0; row < height; row++)
    {
        for (int col = 0; col < width; col++)
        {
            z_buffer[row][col] = -INFINITY;
        }
    }
    // TODO: allocate dynamic array for vertices and faces
    vertices = (vertex3 *) malloc(sizeof(vertex3) * 1000);
    faces = (unsigned int *) malloc(sizeof(int) * 3000);
}

void cleanup()
{
    free(vertices);
    free(faces);
}

void clear()
{
    int c = 0;
    // write pixel buffer
    for (int row = 0; row  < height; row++)
    {   
        for (int col = 0; col < width; col++)
        {
            pixel p;
            p.r = 0;
            p.g = 0;
            p.b = 0;
            pixel_buffer[row][col] = p;
        }
    }
}

void point(int x, int y, int c)
{
    if (x >= 0 && x < width && y >= 0 && y < height)
    {
        pixel p;
        p.b = c;
        p.g = c;
        p.r = c;
        pixel_buffer[y][x] = p;
    }
}

void transform_vertex(float vertex[3], int res[3])
{
    float va[4] = {vertex[0], vertex[1], vertex[2], 1.0};
    vec4_matrix4_mul(va, model_matrix, res);
}

void bounding_box(int *v0, int *v1, int *v2, int *box)
{
    int x_min = v0[0];
    int x_max = v0[0];
    int y_min = v0[1];
    int y_max = v0[1];

    x_min = v1[0] < x_min ? v1[0] : x_min;
    x_min = v2[0] < x_min ? v2[0] : x_min;
    
    y_min = v1[1] < y_min ? v1[1] : y_min;
    y_min = v2[1] < y_min ? v2[1] : y_min;

    x_max = v1[0] > x_max ? v1[0] : x_max;
    x_max = v2[0] > x_max ? v2[0] : x_max;
    
    y_max = v1[1] > y_max ? v1[1] : y_max;
    y_max = v2[1] > y_max ? v2[1] : y_max;

    box[0] = x_min;
    box[1] = y_min;
    box[2] = x_max;
    box[3] = y_max;
}

void barycentric(int *v0, int *v1, int *v2, int *p, float *res)
{
    float temp0[3] = {0};
    float temp1[3] = {0};
    float cross_res[3];
    temp0[0] = v1[0] - v0[0];
    temp0[1] = v2[0] - v0[0];
    temp0[2] = v0[0] - p[0];
    temp1[0] = v1[1] - v0[1];
    temp1[1] = v2[1] - v0[1];
    temp1[2] = v0[1] - p[1];
    vec_cross(temp0, temp1, cross_res);
    
    if (cross_res[2] == 0)
    {
        res[0] = -1;
        res[1] = -1;
        res[2] = -1;
        return;
    }
    
    // u, v, w
    res[0] = cross_res[0] / cross_res[2];
    res[1] = cross_res[1] / cross_res[2];
    res[2] = 1 - (res[0] + res[1]);
}

void triangle(int *v0, int *v1, int *v2, int color)
{
    int b_box[4];
    bounding_box(v0, v1, v2, b_box);

    int p[2];
    float b_res[3];
    for (int x = b_box[0]; x < b_box[2]; x++)
    {
        for (int y = b_box[1]; y < b_box[3]; y++)
        {
            p[0] = x;
            p[1] = y;
            barycentric(v0, v1, v2, p, b_res);
            if (b_res[0] < 0 || b_res[1] < 0 || b_res[2] < 0)
                continue;

            float z = v0[2] * b_res[2] + v1[2] * b_res[1] + v2[2] * b_res[0];
            if (z_buffer[y][x] < z)
            {
                z_buffer[y][x] = z;
                point(x, y, color);
            }
        }
    }
}

void load_matrices(unsigned int translate[3], unsigned int scale[3], unsigned int rot[3])
{
    float translate_matrix[4][4] = 
    {
        {1,0,0, translate[0]},
        {0,1,0, translate[1]},
        {0,0,1, translate[2]},
        {0,0,0,1}
    };

    float scale_matrix[4][4] = 
    {
        {scale[0], 0,       0,       0},
        {0,       scale[1], 0,       0},
        {0,       0,       scale[2], 0},
        {0,       0,       0,        1}
    };

    float x_rot_matrix[4][4] = 
    {
        {1,    0,            0,        0},
        {0, cosf((rot[0]/180.0) * M_PI), -sin((rot[0]/180.0) * M_PI), 0},
        {0, sinf((rot[0]/180.0)* M_PI), cos((rot[0]/180.0) * M_PI),  0},
        {0,    0,            0,        1}
    };

    float y_rot_matrix[4][4] = 
    {
        {cosf((rot[1]/180.0) * M_PI),  0, sinf((rot[1]/180.0) * M_PI), 0},
        {   0,         1,     0,       0},
        {-sinf((rot[1]/180.0) * M_PI), 0, cosf((rot[1]/180.0) * M_PI), 0},
        {   0,         0,     0,       1}
    };

    float z_rot_matrix[4][4] = 
    {
        {cosf((rot[2]/180.0) * M_PI), -sinf((rot[2]/180.0) * M_PI), 0, 0},
        {sinf((rot[2]/180.0) * M_PI), cosf((rot[2]/180.0) * M_PI),  0, 0},
        {   0,            0,        1, 0},
        {   0,            0,        0, 1}
    };

    float temp_matrix[4][4] = {0};
    float rot_matrix[4][4] = {0};
    // build rotation matrix
    matrix_44_mul(x_rot_matrix, y_rot_matrix, temp_matrix);
    matrix_44_mul(temp_matrix, z_rot_matrix, rot_matrix);
    // translation * rotation
    matrix_44_mul(translate_matrix, rot_matrix, temp_matrix);
    // translation * rotation * scale = model_matrix
    matrix_44_mul(temp_matrix, scale_matrix, model_matrix);
}

// loads the obj into the pixel buffer
void read_obj(char *filename)
{
    FILE *file;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    file = fopen(filename, "r");
    if (file)
    {
        while ((read = getline(&line, &len, file)) != EOF)
        {
            // make a copy of the line because strsep modifies it
            char *l = strdup(line);
            // make the first "split"
            char *token = strsep(&l, " ");
            // vertex
            if (strcmp(token, "v") == 0)
            {
                float c[3] = {0, 0, 0};
                int i = 0;
                token = strsep(&l, " ");
                // keep splitting
                while (token != NULL)
                {
                    if (i < 3) c[i++] = atof(token);
                    token = strsep(&l, " ");
                }
                // save vertex
                vertex3 v;
                v.x = c[0];
                v.y = c[1];
                v.z = c[2];
                vertices[vcount++] = v;
            }
            // face
            else if (strcmp(token, "f") == 0) 
            {
                while (token != NULL)
                {
                    token = strsep(&l, " ");
                    if (token != NULL)
                    {
                        faces[fcount++] = atoi(strsep(&token, "/"));
                        token = l;
                    }
                }
            }
            
        }

        fclose(file);
    }
}

void draw_obj(unsigned int translation[3], unsigned int scale[3], unsigned int rotation[3])
{
    load_matrices(translation, scale, rotation);
    unsigned int c = 0;
    float light[3] = {0, 0, 1};

    while (c < fcount)
    {
        unsigned int f0 = faces[c++]-1;
        unsigned int f1 = faces[c++]-1;
        unsigned int f2 = faces[c++]-1;
        
        // grab 3 vertices
        float v0[3];
        float v1[3];
        float v2[3];
        
        v0[0] = vertices[f0].x;
        v0[1] = vertices[f0].y;
        v0[2] = vertices[f0].z;
        v1[0] = vertices[f1].x;
        v1[1] = vertices[f1].y;
        v1[2] = vertices[f1].z;
        v2[0] = vertices[f2].x;
        v2[1] = vertices[f2].y;
        v2[2] = vertices[f2].z;

        // calculate face color
        float t0[3] = {0};
        float t1[3] = {0};
        float cross[3] = {0};
        vec_diff(v1, v0, t0);
        vec_diff(v2, v1, t1);
        vec_cross(t0, t1, cross);
        vec_normalized(cross, t0);
        float i = vec_dot(t0, light);
        int color = (int)(255 * i);
        if (color < 0) continue;

        // transform vertices
        int vt0[3];
        int vt1[3];
        int vt2[3];
        transform_vertex(v0, vt0);
        transform_vertex(v1, vt1);
        transform_vertex(v2, vt2);

        triangle(vt0, vt1, vt2, color);
    }
}

void write()
{
    FILE *file;
    file = fopen("out.bmp", "wb");

    char header[] = {'B', 'M'};
    int image_size = width * height * 3;
    int zero = 0;
    int one = 1;
    int data_offset = 54;
    int header_size = 40;
    int bits_per_pixel = 24;
    
    // BM standard
    fwrite(header, sizeof(char), sizeof(header), file);
    // file size
    fwrite(&image_size + data_offset, 4, 1, file);
    // reserved
    fwrite(&zero, 4, 1, file);
    // data offset
    fwrite(&data_offset, 4, 1, file);
    // header size
    fwrite(&header_size, 4, 1, file);
    // width
    fwrite(&width, 4, 1, file);
    // height
    fwrite(&height, 4, 1, file);
    // planes
    fwrite(&one, 2, 1, file);
    // bits per pixel
    fwrite(&bits_per_pixel, 2, 1, file);
    // compression
    fwrite(&zero, 4, 1, file);
    // image size
    fwrite(&image_size, 4, 1, file);
    // x and y pixels per meter
    fwrite(&zero, 4, 1, file);
    fwrite(&zero, 4, 1, file);
    // number of colors
    fwrite(&zero, 4, 1, file);
    // important colors
    fwrite(&zero, 4, 1, file);

    // write pixel buffer
    int c = 0;
    for (int row = 0; row  < height; row++)
        for (int col = 0; col < width; col++)
            fwrite(&pixel_buffer[row][col], sizeof(pixel), 1, file);
    
    fclose(file);
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("%s\n", "Usage: sr <filename.obj>");
        return -1;
    }

    initialize();
    clear();
    read_obj(argv[1]);
    
    unsigned int translation[3] = {200, 200, 0};
    unsigned int scale[3] = {100, 100, 100};
    unsigned int rotation[3] = {330, 30, 0};
    draw_obj(translation, scale, rotation);
    
    write();

    cleanup();
    
    return 0;
}