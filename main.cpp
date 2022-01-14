#include <iostream>

class LinkNode {
public:
    int value;
    LinkNode* nextPtr;
};

class Link {
    LinkNode startNode = 0;
    LinkNode getLastNode();
};

int main() {
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
