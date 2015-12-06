#include "lib/lodepng.h"
#include <iostream>
#include <fstream>
#include <math.h>

int main(int argc, char **argv) {
    if ( argc != 2 ) {
        std::cerr << "Usage: img PNG" << std::endl;
        return 1;
    }

    // storage for the image
    std::vector<unsigned char> image; 
    unsigned width, height;

    // load the image
    auto error = lodepng::decode(image, width, height, argv[1]);
    if( error ) {
        std::cerr << "decoder error " << error << 
            ": " << lodepng_error_text(error) << std::endl;
        return 1;
    } 

    // print the image as greyscale ascii art
    static const std::string symbols = " .-+*#";
    for ( size_t y=0; y<height; y++ ) {
        for ( size_t x=0; x<width; x++ ) {
            size_t i = (y * width + x)*4;
            double r = static_cast<double>(image[i+0])/255;
            double g = static_cast<double>(image[i+1])/255;
            double b = static_cast<double>(image[i+2])/255;
            double a = static_cast<double>(image[i+3])/255;
            
            double greyscale = (0.30 * r +  0.59 * g + 0.11 * b) * a;
            int symbol_index = ceil(greyscale * symbols.size());
            char symbol = symbols[ symbol_index ];
            std::cout << symbol << symbol;
        }
        std::cout << std::endl;
    }

    return 0;
}

