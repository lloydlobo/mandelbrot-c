#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define IMAGE_WIDTH 800
#define IMAGE_HEIGHT 600
#define MAX_ITER 1000

/**
 * Define the pixel structure (24-bit RGB)
 */
struct pixel {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

struct pixel pixel_get_color(int iter)
{
    struct pixel self;
    size_t WRAP_MAX_VALUES = 256; // 0 -> 255

    self.red = iter % WRAP_MAX_VALUES;
    self.green = (iter * 7) % WRAP_MAX_VALUES;
    self.blue = (iter * 13) % WRAP_MAX_VALUES;

    return self;
}

struct complex {
    double real;
    double imag;
};

int complex_get_mandelbrot_iter(struct complex c)
{
    int iter = 0;
    struct complex cz = { .real = c.real, .imag = c.imag };

    while (iter < MAX_ITER) {
        double czr_sq = cz.real * cz.real;
        double czi_sq = cz.imag * cz.imag;

        if (czr_sq + czi_sq > 4.0) {
            return iter;
        }

        cz.imag = 2 * cz.real * cz.imag + c.imag;
        cz.real = czr_sq - czi_sq + c.real;
        iter++;
    }

    return iter;
}

int main()
{
    struct pixel image[IMAGE_WIDTH][IMAGE_HEIGHT];

    int init_val = 255; // 0: black, 255: white
    for (int x = 0; x < IMAGE_WIDTH; x++) {
        for (int y = 0; y < IMAGE_HEIGHT; y++) {
            image[x][y].red = init_val;
            image[x][y].green = init_val;
            image[x][y].blue = init_val;
        }
    }

    FILE *file = fopen("output.bmp", "wb");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    uint8_t bmp_fheader[54] =
        // clang-format off
    {
        0x42, 0x4D,         // BM - Magic bytes
        0x36, 0x00, 0x0C, 0x00, // File size (54 bytes for header + pixel data)
        0x00, 0x00, 0x00, 0x00, // Reserved
        0x36, 0x00, 0x00, 0x00, // Offset to pixel data
        0x28, 0x00, 0x00, 0x00, // Info header size
        IMAGE_WIDTH & 0xFF, (IMAGE_WIDTH >> 8) & 0xFF, 0x00, 0x00, // Image width
        IMAGE_HEIGHT & 0xFF, (IMAGE_HEIGHT >> 8) & 0xFF, 0x00, 0x00, // Image height
        0x01, 0x00,         // Number of color planes
        0x18, 0x00,         // Bits per pixel (24-bit)
        0x00, 0x00, 0x00, 0x00, // Compression method (none)
        0x00, 0x00, 0x00, 0x00, // Image size (unspecified)
        0x00, 0x00, 0x00, 0x00, // Horizontal resolution (unspecified)
        0x00, 0x00, 0x00, 0x00, // Vertical resolution (unspecified)
        0x00, 0x00, 0x00, 0x00, // Number of colors in palette
        0x00, 0x00, 0x00, 0x00  // Number of important colors
    }; // clang-format on

    fwrite(bmp_fheader, sizeof(uint8_t), 54, file); // Write header to the file

    /*
     * Write the pixel data (from bottom to top)
     */
    for (int y = IMAGE_HEIGHT - 1; y >= 0; y--) {
        for (int x = 0; x < IMAGE_WIDTH; x++) {
            struct complex c = {
                .real = (x - IMAGE_WIDTH / 2.0) * 4.0 / IMAGE_WIDTH,
                .imag = (y - IMAGE_HEIGHT / 2.0) * 4.0 / IMAGE_HEIGHT,
            };
            int iter = complex_get_mandelbrot_iter(c);
            struct pixel tmp_image = pixel_get_color(iter);

            image[x][y] = tmp_image;
            fwrite(&image[x][y], sizeof(struct pixel), 1, file);
        }
    }

    fclose(file);

    printf("Image created successfully!\n");

    return 0;
}
