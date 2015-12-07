#include <iostream>
#include <fstream>
#include <iterator>
#include "compress.h"



int main(int argc, char **argv) {
    
    // TODO store the huffman tree in the file itself!!!
    // currently, we cheat by rebuilding the tree from the source file.
    std::ifstream fin("noise.txt");
    Huffman<char> huff;
    using InIt = std::istreambuf_iterator<char>;
    for ( auto pc = InIt(fin); pc != InIt(); pc++ ) {
        huff.add_symbol(*pc);
    }
    huff.add_symbol('\0');
    huff.build_tree();
    //huff.dump_tree(std::cerr);
    
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

