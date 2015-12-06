#include <queue>
#include <vector>
#include <memory>
#include <map>

template<typename Symbol>
class Huffman {
    using SymbolToFrequency = std::map<Symbol, unsigned long>;
    using SymbolFrequency = std::pair<Symbol, unsigned long>;
    SymbolToFrequency frequency_table;
    using SymbolToBits = std::map<Symbol, std::string>;
    SymbolToBits bits_table;

public:
    void add_symbol(const Symbol& symbol) {
        add_symbol(symbol, 1ul);
    }

    struct Node;
    using OwnedNode = std::shared_ptr<Node>;
    OwnedNode root;

    struct Node {
        Symbol symbol;
        unsigned long frequency;
        OwnedNode zero_node;
        OwnedNode one_node;
        Node* parent_node;
        std::string bits;

        // leaf node constructor
        Node(Symbol _symbol, unsigned long _frequency): 
            symbol(_symbol), frequency(_frequency), zero_node(nullptr), one_node(nullptr) {}

        // internal node constructor
        Node(unsigned long _frequency, OwnedNode zero, OwnedNode one): 
            frequency(_frequency), zero_node(zero), one_node(one) {}

        ~Node() = default;

        bool is_leaf() const { return zero_node == nullptr; }

        void dump(std::ostream& out, int level) const {
            std::string indent(4*level, ' ');
            if ( is_leaf() ) {
                out << indent << std::hex <<  (int)symbol << " -> " << bits
                    << std::dec << " (" << frequency << ")\n";
            } else {
                out << indent << "? -> " << bits << "... (" << frequency << ")\n";
                zero_node->dump(out, level+1);
                one_node->dump(out, level+1);
            }
        }

        void parentify(Node* parent, std::string _bits, SymbolToBits& bits_table) {
            bits = _bits;
            parent_node = parent;
            bits_table[symbol] = bits;
            if ( !is_leaf() ) {
                zero_node->parentify(this, bits + "0", bits_table);
                one_node->parentify(this, bits + "1", bits_table);
            }
        }
    };

    void add_symbol(const Symbol& symbol, unsigned long n) {
        auto ppair = frequency_table.find(symbol);
        if ( ppair == frequency_table.end() ) {
            frequency_table.insert( std::make_pair(symbol, 1) );
        } else {
            ++ppair->second;
        }
    }


    void build_tree() {
        
        auto compare = [](const Node& lhs, const Node& rhs) {
            return lhs.frequency > rhs.frequency;
        };

        std::priority_queue<Node, std::vector<Node>, decltype(compare)> 
            pq(compare);

        for ( auto ppair = frequency_table.begin(); ppair != frequency_table.end(); ppair++ ) {
            pq.emplace(Node(ppair->first, ppair->second));
        }

        /* 
        while ( !pq.empty() ) {
            OwnedNode zero = new Node(pq.top());
            pq.pop();
            zero->dump(std::cout, 0);
        }
        */

        while ( pq.size() >= 2 ) {
            OwnedNode zero = std::make_shared<Node>(pq.top());
            pq.pop();
            OwnedNode one = std::make_shared<Node>(pq.top());
            pq.pop();
            pq.emplace( Node(zero->frequency + one->frequency, zero, one) );
        }

        root = std::make_shared<Node>(pq.top());
        root->parentify(nullptr, "", bits_table);
        
    }

    void dump_tree() {
        root->dump(std::cout, 0);
    }

    std::string symbol_to_bits(const Symbol& symbol) {
        return bits_table.at(symbol);
    }

    Symbol bits_to_symbol(const std::string& bits) {
        Node* node = root.get();
        for ( auto pbit = bits.begin(); pbit != bits.end(); pbit++ ) {
            if ( node->is_leaf() ) {
                throw std::exception();
            }

            if ( *pbit == '0' ) {
                node = node->zero_node.get();
            } else if ( *pbit == '1' ) {
                node = node->one_node.get();
            } else {
                throw std::exception();
            }
        }

        if ( !node->is_leaf() ) {
            throw std::exception();
        }

        return node->symbol;
    }

};
