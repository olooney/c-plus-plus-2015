#include "lib/lodepng.h"
#include <iostream>
#include <fstream>
#include <math.h>
#include "compress.h"

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

    Huffman<unsigned char> huff;
    for ( auto pc = image.begin(); pc != image.end(); pc++ ) {
        huff.add_symbol(*pc);
    }
    huff.build_tree();
    huff.dump_tree();

    std::cout << "bits(255) = " << huff.symbol_to_bits( (unsigned char)255) << std::endl;
    std::cout << "symbol(001100) = " << std::hex << (int)huff.bits_to_symbol("001100") << std::dec << std::endl;

    std::ofstream out("img.huff");
    BitWriter bit_writer(out);
    for ( auto pc = image.begin(); pc != image.end(); pc++ ) {
        auto bits = huff.symbol_to_bits(*pc);
        //std::cout << bits;
        for ( auto pb = bits.begin(); pb != bits.end(); pb++ ) {
            if ( *pb == '0' ) {
                bit_writer.write_bit(0);
            } else {
                bit_writer.write_bit(1);
            }
        }
    }
    bit_writer.flush();
    out.close();

    std::cout << "\n\n\n";

    std::ifstream in("img.huff");
    BitReader bit_reader(in);
    Huffman<unsigned char>::Node* state = nullptr;
    unsigned char symbol;
    int y = 0;
    while ( bit_reader ) {
        int bit = bit_reader.read_bit();
        //std::cout << bit;
        bool symbol_complete = huff.read_huff_bit(bit, state, symbol);
        if ( symbol_complete ) {
            std::cout << std::hex << (int)symbol << std::dec << " ";
            y++;
            if ( y >= height ) {
                y=0;
                std::cout << std::endl;
            }
        }
    }

    in.close();

    return 0;

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

