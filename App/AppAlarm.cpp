#include "Initializer.h"


using namespace std;


int main(int argc, char *argv[])
{
    Initializer::getInstance()->init(argc, argv);
    Initializer::getInstance()->begin();
    return 0;
}
