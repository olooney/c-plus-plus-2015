#include "lib/lodepng.h"
#include <iostream>
#include <vector>
#include <math.h>

class Problem {
};

struct Point {
public:
    size_t x;
    size_t y;

    bool operator==(const Point& rhs)const {
        return x == rhs.x && y == rhs.y;
    }

};

class Maze: public Problem {
public:
    
    Maze(const char* image_filename, const Point& start_, const Point& goal_):
        start(start_),
        goal(goal_)
    {
	    // storage for the image
	    std::vector<unsigned char> image; 
	    unsigned width, height;

	    // load the image
	    auto error = lodepng::decode(image, width, height, image_filename);
	    if( error ) {
		std::cerr << "decoder error " << error << 
		    ": " << lodepng_error_text(error) << std::endl;
		throw std::runtime_error("unable to read Maze from image file");
	    } 
            size.x = width;
            size.y = height;
            traversable.resize(width*height);
            // std::cerr << "loaded " << image_filename << " read " << size.x << "x" << size.y << " image with " << traversable.size() << " pixels" << std::endl;

	    // convert to a boolean vector of traversable cells
	    for ( size_t y=0; y<size.y; y++ ) {
		for ( size_t x=0; x<size.x; x++ ) {
		    size_t i = (x + y*size.x)*4;
		    double r = static_cast<double>(image[i+0])/255;
		    double g = static_cast<double>(image[i+1])/255;
		    double b = static_cast<double>(image[i+2])/255;
		    double a = static_cast<double>(image[i+3])/255;

		    double greyscale = (0.30 * r +  0.59 * g + 0.11 * b) * a;
		    traversable[x + y*size.x] = (greyscale < 0.2);
		}
	    }
        // TODO sanity check start and goal against size
    }

    friend std::ostream& operator <<(std::ostream& out, const Maze& maze) {
        // print the maze
        for ( size_t y=0; y<maze.size.y; y++ ) {
            for ( size_t x=0; x<maze.size.x; x++ ) {
                const Point p = Point{x,y};
		if ( p == maze.start ) {
		    out << "@";
		} else if ( p == maze.goal ) {
		    out << "X";
		} else {
                    out << (maze.traversable[x+y*maze.size.x] ? ' ' : '#');
		}
            }
            out << std::endl;
        }
        return out;
    }

    
private:
    std::vector<bool> traversable;
    Point size;
    Point start; 
    Point goal;
    
};

int main(int argc, char **argv) {
    if ( argc != 2 ) {
        std::cerr << "Usage: img PNG" << std::endl;
        return 1;
    }

    auto maze = Maze(argv[1], Point{0,0}, Point{31, 5});
    std::cout << maze << std::endl;


    return 0;
}

