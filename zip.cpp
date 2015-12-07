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
    for ( auto pc = input.begin(); pc != input.end(); pc++ ) {
        huff.add_symbol(*pc);
    }
    huff.build_tree();
    // huff.dump_tree(std::cerr);

    // write out the encoded file
    BitWriter bit_writer(std::cout);
    for ( auto pc = input.begin(); pc != input.end(); pc++ ) {
        auto bits = huff.symbol_to_bits(*pc);
        // std::cerr << "symbol \"" << *pc << "\" -> " << bits << std::endl;
        for ( auto pb = bits.begin(); pb != bits.end(); pb++ ) {
            if ( *pb == '0' ) {
                bit_writer.write_bit(0);
            } else {
                bit_writer.write_bit(1);
            }
        }
    }
    bit_writer.flush();
    
    return 0;
}

