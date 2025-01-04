#include <iostream>
#include <fstream>
#include <vector>
#include <complex>
#include <tbb/parallel_for.h> // Include only the necessary header for parallel_for
#include <chrono>             // For timing

#define WIDTH 1000
#define HEIGHT 1000
#define MAX_ITER 1000

class Mandelbrot {
private:
    int width, height, max_iter;
    std::vector<int> image; // Stores RGB values

public:
    Mandelbrot(int w, int h, int max_iter) : width(w), height(h), max_iter(max_iter) {
        image.resize(width * height * 3); // 3 values (R, G, B) per pixel
    }

    void compute_row(int y) {
        for (int x = 0; x < width; x++) {
            std::complex<double> c(
                (x - width / 2.0) * 4.0 / width,
                (y - height / 2.0) * 4.0 / width);
            std::complex<double> z(0, 0);
            int iter = 0;
            while (abs(z) <= 2.0 && iter < max_iter) {
                z = z * z + c;
                iter++;
            }

            // Map iteration count to RGB
            int r = (255 * iter) / max_iter;
            int g = (255 * (iter * 2 % max_iter)) / max_iter;
            int b = (255 * (iter * 3 % max_iter)) / max_iter;

            int index = (y * width + x) * 3;
            image[index] = r;
            image[index + 1] = g;
            image[index + 2] = b;
        }
    }

    void generate() {
        tbb::parallel_for(0, height, [&](int y) {
            compute_row(y);
        });
    }

    void save_to_file(const std::string &filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error opening file: " << filename << std::endl;
            return;
        }

        file << "P3\n" << width << " " << height << "\n255\n"; // PPM Header
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                int index = (y * width + x) * 3;
                file << image[index] << " " << image[index + 1] << " " << image[index + 2] << " ";
            }
            file << "\n";
        }
        file.close();
        std::cout << "Fractal image saved to " << filename << std::endl;
    }
};

int main() {
    const std::string output_file = "mandelbrot_tbb_color.ppm";

    Mandelbrot mandelbrot(WIDTH, HEIGHT, MAX_ITER);

    // Measure execution time
    auto start_time = std::chrono::high_resolution_clock::now();
    mandelbrot.generate();
    auto end_time = std::chrono::high_resolution_clock::now();

    mandelbrot.save_to_file(output_file);

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    std::cout << "Execution time: " << duration / 1000.0 << " seconds\n";

    return 0;
}
