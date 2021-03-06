#include <iostream>
#include <fstream>
#include <iterator>
#include "compress.h"

int main(int argc, char **argv) {
    
    // read everything from stdin
    std::vector<char> input(
       (std::istreambuf_iterator<char>(std::cin.rdbuf())),
       std::istreambuf_iterator<char>()
    ); 
    
    // add a null terminator as a sentinal
    input.push_back('\0');
    
    // calculate the Huffman encoding tree
    Huffman<char> huff;
    for ( auto& c : input ) {
        huff.add_symbol(c);
    }
    huff.build_tree();
    // huff.dump_tree(std::cerr);

    huff.write_tree(std::cout);

    // write out the encoded file
    BitWriter bit_writer(std::cout);
    for ( auto& c : input ) {
        auto bits = huff.symbol_to_bits(c);
        // std::cerr << "symbol \"" << c << "\" -> " << bits << std::endl;
        for ( auto& b : bits ) {
            if ( b == '0' ) {
                bit_writer.write_bit(0);
            } else {
                bit_writer.write_bit(1);
            }
        }
    }
    bit_writer.flush();
    
    return 0;
}

