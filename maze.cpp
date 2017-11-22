#include "lib/lodepng.h"
#include <iostream>
#include <vector>
#include <math.h>
#include <algorithm>

struct Point {
public:
    int x;
    int y;

    bool operator==(const Point& rhs) const {
        return x == rhs.x and y == rhs.y;
    }
    bool operator!=(const Point& rhs) const {
        return not (*this == rhs);
    }

    double distanceTo(const Point& rhs) const {
        double dx = static_cast<double>(x) - static_cast<double>(rhs.x);
        double dy = static_cast<double>(y) - static_cast<double>(rhs.y);
        return sqrt(dx*dx + dy*dy);
    }

    friend std::ostream& operator<<(std::ostream& out, const Point& p) {
        out << "(" << p.x << "," << p.y << ")";
        return out;
    }

    std::vector<Point> neighborhood() const {
        std::vector<Point> ret = {
            Point{x, y+1},
            //Point{x+1, y+1},
            Point{x+1, y},
            //Point{x+1, y-1},
            Point{x, y-1},
            //Point{x-1, y-1},
            Point{x-1, y}
            //Point{x-1, y+1}
        };
        return ret;
    }
};

class Maze {
public:
    
    Maze(const char* image_filename):
        start{-1, -1},
        goal{-1, -1}
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
            visited.resize(width*height, false);
            breadcrumbs.resize(width*height, 0);

            // std::cerr << "loaded " << image_filename << " read " << size.x << "x" << size.y << " image with " << traversable.size() << " pixels" << std::endl;

        // convert to a boolean vector of traversable cells
        for ( int y=0; y<size.y; y++ ) {
            for ( int x=0; x<size.x; x++ ) {
                size_t i = (x + y*size.x)*4;
                double r = static_cast<double>(image[i+0])/255;
                double g = static_cast<double>(image[i+1])/255;
                double b = static_cast<double>(image[i+2])/255;
                double a = static_cast<double>(image[i+3])/255;
                // std::cerr << Point{x,y} << " rgba(" << r << "," << g << "," << b << "," << a << ")" << std::endl;

                if ( r > 0.7 and (g + b) < 0.5 ) {
                    // red goal point
                    goal = Point{x,y};
                    // std::cerr << "goal: " << goal << std::endl;
                    traversable[x + y*size.x] = true;
                } else if ( g > 0.7 and (r + b) < 0.5 ) {
                    // green starting point
                    start = Point{x,y};
                    // std::cerr << "start: " << start << std::endl;
                    traversable[x + y*size.x] = true;
                } else {
                    double greyscale = (0.30 * r +  0.59 * g + 0.11 * b) * a;
                    traversable[x + y*size.x] = (greyscale > 0.5);
                }
            }
        }

        // sanity check start and goal
        if ( not inBounds(start) ) {
            throw std::runtime_error("no (green) start point found");
        }
        if ( not inBounds(goal) ) {
            throw std::runtime_error("no (red) goal point found");
        }
    }

    bool inBounds(const Point& p) const {
        return p.x >= 0 and p.y >= 0 and p.x < size.x and p.y < size.y;
    }

    bool isTraversable(const Point& p) const {
        return inBounds(p) and traversable[p.x + p.y*size.x] and not visited[p.x + p.y*size.x];
    }

    virtual double heuristic(const Point& p) const {
        return p.distanceTo(goal);
    }

    bool isGoal(const Point& p ) const {
        return p == goal;
    }

    std::vector<Point> neighborNodes(const Point& p) const {
        auto neighbors = p.neighborhood();
        auto eraser = std::remove_if(neighbors.begin(), neighbors.end(), [this](const Point& p) {
            return not isTraversable(p);
        });
        neighbors.erase(eraser, neighbors.end());
        return neighbors;
    }


    friend std::ostream& operator <<(std::ostream& out, const Maze& maze) {
        // print the maze
        for ( int y=0; y<maze.size.y; y++ ) {
            for ( int x=0; x<maze.size.x; x++ ) {
                const Point p = Point{x,y};
                if ( p == maze.start ) {
                    out << "@";
                } else if ( p == maze.goal ) {
                    out << "X";
                } else if ( maze.breadcrumbs[x+y*maze.size.x] > 0 ) {
                    out << maze.breadcrumbs[x+y*maze.size.x];
                } else if ( maze.visited[x+y*maze.size.x] ) {
                    out << ".";
                } else {
                    out << (maze.traversable[x+y*maze.size.x] ? ' ' : '#');
                }
            }
            out << std::endl;
        }
        return out;
    }

    // the worst solution
    std::vector<Point> naive_depth_first() { return naive_depth_first(start); }
    std::vector<Point> naive_depth_first(const Point&p) {
        if ( isGoal(p) ) {
            return { p };
        } else {
            // flag this node as non-traversable to prevent cycles
            visited[p.x +p.y*size.x] = true;

            // the neighborhood of all possible moves from this node
            auto neighbors = neighborNodes(p);

            // investigate the most promising node first
            std::sort(neighbors.begin(), neighbors.end(), [this](const Point& lhs, const Point& rhs) {
                return lhs.distanceTo(goal) < rhs.distanceTo(goal);
            });

            // recurse depth-first
            for ( auto p2 : neighbors ) {
                if ( p != p2 ) {
                    auto path = naive_depth_first(p2);
                    if ( not path.empty() ) {
                        path.push_back(p);
                        return path;
                    }
                }
            }
        } 
        // return an empty vector as a sentinel indicating no solution was found.
        return {};
    }

    void paint_path(const std::vector<Point> path) {
        std::fill(breadcrumbs.begin(), breadcrumbs.end(), 0);
        int i = 1;
        for ( auto p : path ) {
            breadcrumbs[p.x + p.y*size.x] = i;
            i++;
            if ( i >= 10 ) i = 1;
        }
    }
    
private:
    std::vector<bool> traversable;
    std::vector<bool> visited;
    std::vector<int> breadcrumbs;
    Point size;
    Point start;
    Point goal;
    
};


int main(int argc, char **argv) {
    if ( argc != 2 ) {
        std::cerr << "Usage: img PNG" << std::endl;
        return 1;
    }

    auto maze = Maze(argv[1]);

    auto path = maze.naive_depth_first();
    std::reverse(path.begin(), path.end());
    maze.paint_path(path);
    std::cout << maze << std::endl;

    return 0;
}

