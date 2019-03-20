#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

const int width = 400;
const int height = 400;
pixel *pixel_buffer;
vertex3 *vertices;
unsigned int *faces;
unsigned int vcount = 0;
unsigned int fcount = 0;

void initialize()
{
    pixel_buffer = (pixel *) malloc(sizeof(pixel) * width * height);
    vertices = (vertex3 *) malloc(sizeof(vertex3) * 500);
    faces = (unsigned int *) malloc(sizeof(int) * 1000);
}

void cleanup()
{
    free(pixel_buffer);
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
            pixel_buffer[row + col] = p;
        }
    }
}

// Receives normalized coordinates as input
void point(float x, float y)
{
    unsigned int xs = (int)((x + 1) * width / 2);
    unsigned int ys = (int)((y + 1) * height / 2);
    if (xs >= 0 && xs < width && ys >= 0 && ys < height)
    {
        pixel p;
        p.b = 255;
        p.g = 255;
        p.r = 255;
        pixel_buffer[xs * width + ys] = p;
    }
}

// loads the obj into the pixel buffer
void load_obj(char *filename)
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
                
                // draw point
                point(v.x, v.y);
            }
            // face
            else if (strcmp(token, "f") == 0) 
            {
                while (token != NULL)
                {
                    token = strsep(&l, " ");
                    if (token != NULL)
                        faces[fcount++] = atoi(strsep(&token, "/"));
                }
            }
            
        }

        fclose(file);
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
    while (c < width * height)
    {
        fwrite(&pixel_buffer[c++], sizeof(pixel), 1, file);
    }
    
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

    load_obj(argv[1]);
    
    write();

    cleanup();
    
    return 0;
}