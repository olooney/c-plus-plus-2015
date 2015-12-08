#include <iostream>
#include <fstream>
#include <iterator>
#include "compress.h"



int main(int argc, char **argv) {
    
    // Read the huffman tree at the beginning of the file
    Huffman<char> huff;
    huff.read_tree(std::cin);
    // huff.dump_tree(std::cerr);

    // read the encoded file bit by bit. With every new bit,
    // move one level down the huffman tree. When we reach a leaf,
    // emit the symbol and start over. The process expects to
    // be terminated by an explicit sentinel.
    BitReader bit_reader(std::cin);
    Huffman<char>::Node* state = nullptr;
    char symbol;
    while ( bit_reader ) {
        int bit = bit_reader.read_bit();
        bool symbol_complete = huff.read_huff_bit(bit, state, symbol);
        // std::cerr << bit << " -> " << state->bits;
        // if ( symbol_complete ) {
        //     std::cerr << " \"" << symbol << "\"";
        // }
        // std::cerr << std::endl;
        
        if ( symbol_complete ) {
            if ( symbol == '\0' ) {
                // we've reached the sentinel value, stop immediately.
                return 0;
            } else {
                std::cout << symbol;
            }
        }
    }
    
    return 1;
}

