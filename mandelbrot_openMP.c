#include <stdio.h>
#include <stdlib.h>
#include <omp.h> // Include OpenMP header

#define WIDTH 1000
#define HEIGHT 1000
#define MAX_ITER 1000

void generate_mandelbrot_parallel(const char *filename) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        perror("Error opening file");
        return;
    }

    fprintf(fp, "P3\n%d %d\n255\n", WIDTH, HEIGHT); // PPM header

    // Allocate memory for the image buffer (storing RGB values)
    int *image = (int *)malloc(WIDTH * HEIGHT * 3 * sizeof(int)); // 3 values per pixel (R, G, B)
    if (!image) {
        perror("Memory allocation failed");
        fclose(fp);
        return;
    }

    // Parallelize the fractal generation using OpenMP
    #pragma omp parallel for schedule(dynamic)
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            double c_re = (x - WIDTH / 2.0) * 4.0 / WIDTH;
            double c_im = (y - HEIGHT / 2.0) * 4.0 / WIDTH;
            double z_re = 0, z_im = 0;
            int iter;
            for (iter = 0; iter < MAX_ITER; iter++) {
                if (z_re * z_re + z_im * z_im > 4.0)
                    break;
                double new_re = z_re * z_re - z_im * z_im + c_re;
                double new_im = 2 * z_re * z_im + c_im;
                z_re = new_re;
                z_im = new_im;
            }

            // Map iteration count to color (R, G, B)
            int r = (int)(255.0 * iter / MAX_ITER);
            int g = (int)(255.0 * (iter * 2 % MAX_ITER) / MAX_ITER);
            int b = (int)(255.0 * (iter * 3 % MAX_ITER) / MAX_ITER);

            int index = (y * WIDTH + x) * 3;
            image[index] = r;
            image[index + 1] = g;
            image[index + 2] = b;
        }
    }

    // Write the image buffer to the file
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            int index = (y * WIDTH + x) * 3;
            fprintf(fp, "%d %d %d ", image[index], image[index + 1], image[index + 2]); // RGB
        }
        fprintf(fp, "\n");
    }

    free(image);
    fclose(fp);
    printf("Fractal image saved to %s\n", filename);
}

int main() {
    const char *output_file = "mandelbrot_parallel_color.ppm";

    // Measure execution time
    double start_time = omp_get_wtime();
    generate_mandelbrot_parallel(output_file);
    double end_time = omp_get_wtime();

    printf("Execution time: %.3f seconds\n", end_time - start_time);

    return 0;
}
