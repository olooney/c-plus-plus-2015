#include <iostream>
#include <vector>
#include <memory>

class Node {
public:
    explicit Node(const std::string & new_value): 
        value(new_value)
    {}
    std::unique_ptr<Node> next;
    std::string value;

    Node* add(std::string new_value) {
        auto tail = this;
        while ( tail->next ) tail = tail->next.get();
        tail->next = std::make_unique<Node>(new_value);
        return tail->next.get();
    }

    ~Node() {
        std::cout << "~Node(" << value << ")\n";
    }
};

std::ostream& operator<< (std::ostream& out, const Node& node) {
    out << node.value;
    if ( node.next ) {
        out << ", ";
        out << *(node.next);
    } else {
        out << ".";
    }
    return out;
}

int main(int argc, char** argv) {
    
    Node n1("one");
    n1.add("two");
    auto midpoint = n1.add("three");
    n1.add("four");
    n1.add("five");

    std::cout << n1 << std::endl;
    midpoint->next = nullptr;
    std::cout << n1 << std::endl;

    return 0;
}

