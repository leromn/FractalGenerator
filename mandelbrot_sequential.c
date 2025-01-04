#include <stdio.h>
#include <stdlib.h>
#include <time.h> // Include time.h for clock()

#define WIDTH 1000
#define HEIGHT 1000
#define MAX_ITER 1000

// Function to map iteration count to RGB values for a color gradient
void get_color(int iter, int *r, int *g, int *b) {
    if (iter == MAX_ITER) {
        *r = *g = *b = 0; // Black color for points inside the set
    } else {
        *r = (iter % 256);      // Red component
        *g = (iter * 5) % 256;  // Green component
        *b = (iter * 10) % 256; // Blue component
    }
}

void generate_mandelbrot(const char *filename) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        perror("Error opening file");
        return;
    }

    fprintf(fp, "P3\n%d %d\n255\n", WIDTH, HEIGHT); // PPM header

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

            int r, g, b;
            get_color(iter, &r, &g, &b); // Get the RGB color for the current point

            // Write the RGB values to the file
            fprintf(fp, "%d %d %d ", r, g, b);
        }
        fprintf(fp, "\n");
    }

    fclose(fp);
    printf("Colorful fractal image saved to %s\n", filename);
}

int main() {
    // Start measuring time
    clock_t start_time = clock();

    const char *output_file = "mandelbrot_color.ppm";
    generate_mandelbrot(output_file);

    // End measuring time
    clock_t end_time = clock();
    
    // Calculate time taken in seconds (clock() returns the number of clock ticks)
    double time_taken = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    // Output the time taken to generate the fractal
    printf("Time taken to generate the fractal: %.4f seconds\n", time_taken);

    return 0;
}
