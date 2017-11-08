#include<iostream>

class Movable {
    int member = 42;
public:

    Movable() { 
        std::cerr << "Movable()\n";
    }

    virtual ~Movable() { 
        std::cerr << "~Movable()\n";
    }

    Movable(const Movable & original) {
        std::cerr << "Movable(Movable&)\n";
    }

    Movable(const Movable && original) {
        std::cerr << "Movable(Movable&&)\n";
    }

    Movable& operator=(const Movable& rhs) {
        std::cerr << "operator=(Movable&)\n";
        if ( this != &rhs ) {
            member = rhs.member;
        }
        return *this;
    }

    Movable& operator=(const Movable&& rhs) {
        std::cerr << "operator=(Movable&&)\n";
        if ( this != &rhs ) {
            member = rhs.member;
        }
        return *this;
    }

    bool operator==(const Movable& rhs) const {
        return member == rhs.member;
    }
};

template<typename T> 
T noop(T t) { return t; }

void rvalue() {
    int x = 42;
    int&& y = 42;
    int&& x2 = std::move(x);
    int&& y2 = std::move(y);

    if ( x2 != y2 ) {
	std::cerr << "int move failed" << std::endl;
    }

    Movable m;
    Movable m2 = m;
    Movable m3 = Movable{};

    Movable m4 = noop( Movable{} );
    m4 = noop( Movable{} );
}
