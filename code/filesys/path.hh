#ifndef PATH
#define PATH

#include <algorithm>
#include <iostream>
#include <iterator>
#include <list>
#include <string>
#include <sstream>


using namespace std;

class Path{
    public:

        Path(string path);

        ~Path();

        string FromPathToStr();
        
        // Path * fromStrToPath();

        void Merge(string cdPath);
        void Set(string newPath);

        list<string> path;
    private:
};

#endif