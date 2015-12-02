#include <iostream>
#include <vector>
#include <memory>

template<typename ValueT>
class Node {
public:
    explicit Node(const ValueT & new_value): 
        value(new_value)
    {}
    std::unique_ptr<Node> next;
    ValueT value;

    Node* add(const ValueT& new_value) {
        auto tail = this;
        while ( tail->next ) tail = tail->next.get();
        // TODO only works with std::unique_ptr right now!
        tail->next = std::make_unique<Node>(new_value);
        return tail->next.get();
    }

    ~Node() {
        std::cout << "~Node(" << value << ")\n";
    }
};

template<typename ValueT>
std::ostream& operator<< (std::ostream& out, const Node<ValueT>& node) {
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

    {
        // create a linked list
        // every head owns the entire downstream chain
        Node<std::string> n1("one");
        n1.add("two");
        auto midpoint = n1.add("three");
        n1.add("four");
        n1.add("five");

        std::cout << n1 << std::endl;
        midpoint->next = nullptr;
        std::cout << n1 << std::endl;
    }

    Node<int> m1(42);
    m1.add(13);
    std::cout << m1 << std::endl;

    return 0;
}

