//
// Created by alexgavr on 14.03.17.
//

#ifndef REPO_CLOSE_H
#define REPO_CLOSE_H
#include <unordered_map>
#include "../global/globalTypes.h"

class Close {
private:
    std::unordered_map<unsigned, Node> hash_table;

public:
    void push(Node node);
    std::unordered_map<unsigned, Node>::const_iterator find(Node node);
    std::unordered_map<unsigned, Node>::const_iterator end();
    unsigned size() const;
    Node operator[](Node node) const;
    Node operator[](unsigned nodeKey) const;
};


#endif //REPO_CLOSE_H
