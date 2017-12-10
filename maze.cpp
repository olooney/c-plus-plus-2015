#include "lib/lodepng/lodepng.h"
#include "lib/gif-h/gif.h"

#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <cmath> // need sqrt
#include <algorithm>
#include <functional>
#include <cfloat> // need the DBL_MAX constant
#include <utility> // need std::make_pair

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
            Point{x+1, y},
            Point{x, y-1},
            Point{x-1, y}

            //Point{x+1, y+1},
            //Point{x+1, y-1},
            //Point{x-1, y-1},
            //Point{x-1, y+1}
        };
        return ret;
    }

};

// need a strict ordering to use as an std::map key.
bool operator<(const Point& lhs, const Point& rhs ) {
    if ( lhs.x == rhs.x ) {
        return ( lhs.y < rhs.y );
    } else {
        return ( lhs.x < rhs.x );
    }
}

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

    // best solution
    std::vector<Point> a_star() {
        std::vector< std::pair<double, Point> > frontier;
        std::set<Point> closed_set;
        std::map<Point, double> g_map;
        std::map<Point, Point> prev_map;

        // heuristic - estimated distance to goal
        auto h = [&](Point p) { return p.distanceTo(goal); };

        // shortest known distance from start to p (discovered so far)
        auto g = [&](Point p) -> double {
            if ( g_map.find(p) != g_map.end() ) {
                return g_map[p];
            } else {
                return INFINITY;
            }
        };

        // counter-intuitively, we need to use operator>() (greater than) to
        // sort our heap so that we can pop the least element.
        auto frontier_priority = [](const std::pair<double, Point>& lhs, const std::pair<double, Point>& rhs) {
            return lhs.first > rhs.first;
        };

        // push a point onto the frontier.
        auto push = [&](Point p) {
            double f = g(p) + h(p);
            frontier.emplace_back( std::make_pair(f, p) );
            std::push_heap(frontier.begin(), frontier.end(), frontier_priority);
        };

        // get the next, best point off the frontier.
        auto pop = [&]() -> Point {
            std::pop_heap(frontier.begin(), frontier.end(), frontier_priority);
            std::pair<double, Point> pair = frontier.back();
            frontier.pop_back();
            closed_set.insert(pair.second);
            return pair.second;
        };

        // sometimes we need to update a frontier node when a better path is found.
        auto update_f = [&](Point p) {
            for ( auto pair : frontier ) {
                if  ( pair.second == p ) {
                    double f = g(p) + h(p);
                    pair.first = f;
                    std::make_heap(frontier.begin(), frontier.end(), frontier_priority);
                    return;
                }
            }
        };

        // recurses backwards through the prev_map to reconstruct the optimal
        // path from start to goal.
        std::function<std::vector<Point> (Point)> unwind_path = [&](Point p) -> std::vector<Point> {
            if ( p == start ) {
                return {start};
            } else {
                auto prev = prev_map[p];
                std::vector<Point> path = unwind_path(prev);
                path.push_back(p);
                return path;
            }
        };

        // setup
        push(start);
        g_map[start] = 0;

        // a_star algorithm 
        while ( not frontier.empty() ) {
            // take the most promising point from the frontier
            auto point = pop();
            // std::cerr << "from " << point << ":" << std::endl;

            // we're done! Not only that, but because point was a the top of the frontier heap,
            // we've proven that the path via this point is the *optimal* path.
            if ( point == goal ) {
                return unwind_path(point);
            }

            // search all reachable neighbors
            auto neighbors = neighborNodes(point);
            for ( neighbor : neighbors ) {
                // never search a node twice
                if ( closed_set.find(neighbor) == closed_set.end() ) {
                    // std::cerr << "\tto:" << neighbor; 

                    // total travel cost from start to new neighbor
                    double new_g = g(point) + point.distanceTo(neighbor);
                    double old_g = g(neighbor);
                    // std::cerr << " new_g=" << new_g << " h=" << h(neighbor) << " f=" << new_g + h(neighbor) << " old_g=" << old_g;

                    // if we've found a new/better path...
                    if ( new_g < old_g ) {
                        // in both cases we update g and prev.
                        g_map[neighbor] = new_g;
                        prev_map[neighbor] = point;

                        if ( old_g >= INFINITY ) { 
                            // never visited
                            push(neighbor);
                            // std::cerr << " added to frontier!";
                        } else { 
                            // already visited but this is a shorter path
                            update_f(neighbor);
                            // std::cerr << " updated!";
                       }
                    }
                    // std::cerr << std::endl;
                }
            }
        }

        // empty vector indicates no path found.
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

    void write_gif(const std::string& filename) {
        const uint32_t delay = 8;
        bool ok = true;
        GifWriter writer;
        ok = GifBegin(&writer, filename.c_str(), size.x, size.y, delay);
        if ( not ok ) throw std::runtime_error("unable to open gif file for writing");
            
        uint8_t* frame_data = new uint8_t[size.x * size.y * 4];

        for ( int y=0; y<size.y; y++ ) {
            for ( int x=0; x<size.x; x++ ) {
                const Point p = Point{x,y};
                frame_data[(x + y*size.x)*4 + 3]=0;

                if ( p == start ) {
                    frame_data[(x + y*size.x)*4 + 0]=255;
                    frame_data[(x + y*size.x)*4 + 1]=0;
                    frame_data[(x + y*size.x)*4 + 2]=0;
                } else if ( p == goal ) {
                    frame_data[(x + y*size.x)*4 + 0]=255;
                    frame_data[(x + y*size.x)*4 + 1]=0;
                    frame_data[(x + y*size.x)*4 + 2]=0;
                } else if ( breadcrumbs[x+y*size.x] > 0 ) {
                    frame_data[(x + y*size.x)*4 + 0]=255;
                    frame_data[(x + y*size.x)*4 + 1]=0;
                    frame_data[(x + y*size.x)*4 + 2]=0;
                /*
                } else if ( visited[x+y*size.x] ) {
                    frame_data[(x + y*size.x)*4 + 0]=200;
                    frame_data[(x + y*size.x)*4 + 1]=200;
                    frame_data[(x + y*size.x)*4 + 2]=255;
                */
                } else if (traversable[x+y*size.x] ) {
                    frame_data[(x + y*size.x)*4 + 0]=255;
                    frame_data[(x + y*size.x)*4 + 1]=255;
                    frame_data[(x + y*size.x)*4 + 2]=255;
                } else {
                    frame_data[(x + y*size.x)*4 + 0]=0;
                    frame_data[(x + y*size.x)*4 + 1]=0;
                    frame_data[(x + y*size.x)*4 + 2]=0;
                }
            }
        }

        ok = GifWriteFrame(&writer, frame_data, size.x, size.y, delay); 
        if ( not ok ) throw std::runtime_error("unable to write frame to gif file");

        delete[] frame_data;

        ok = GifEnd(&writer);
        if ( not ok ) throw std::runtime_error("unable to finalize writing to gif file.");
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

    auto path = maze.a_star();
    std::cout << path.size() << std::endl;
    //std::reverse(path.begin(), path.end());
    maze.paint_path(path);
    // std::cout << maze << std::endl;
    if ( not path.empty() ) {
        maze.write_gif(std::string(argv[1]) + ".solution.gif");
    }

    return 0;
}

