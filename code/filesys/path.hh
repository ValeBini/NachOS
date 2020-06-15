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

        string fromPathToStr();
        
        // Path * fromStrToPath();

        list<string> path;
    private:
};