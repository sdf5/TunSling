#include <iostream>
#include "node.h"

using namespace std;

int main()
{
    auto my_node = node::node_factory();
    my_node->run();
    return 0;
}
