#ifndef PQ4_GATORBST_H
#define PQ4_GATORBST_H

#include <string>
#include <utility>
#include <vector>
#include <optional>
#include <string_view>

using namespace std;

struct Node {
    int ufid;
    string name;
    int height;
    Node *left, *right;

    Node(int ufid, string name) : ufid(ufid), name(std::move(name)), height(1), left(nullptr), right(nullptr) {
    };
};


class GatorBST {
private:
    Node *root;

public:
    // constructor and destructor
    GatorBST();

    ~GatorBST();

    // Class Methods


    // Returns 1-based height of tree. In other words, if it is empty, the height is 0, otherwise this returns the length
    //  of the longest path in nodes.
    int Height();

    // Inserts into BST. If successful, returns true, otherwise false.
    // Will fail if UFID already exists in the tree.
    // We assume the name and ID are properly formatted at this point, so no validation happens within this function.
    bool Insert(int ufid, const string &name);

    // Searches for node with given UFID. Returns an optional string view - if not found, returns std::nullopt and will
    // fail .has_value check. If found, returns a string_view, which is essentially a fancier form of a pointer for a
    // string that does not allocate new memory or take "ownership". See more details for these modern C++ features below:
    // https://builtin.com/articles/stdoptional
    // https://en.cppreference.com/w/cpp/string/basic_string_view.html
    optional<string_view> SearchID(int ufid);

    // Searches for name. Since multiple UFID's can share the same name, returns a vector with all matching ID's in ascending order.
    // If none are found, the returned vector will be empty.
    vector<int> SearchName(const string &name);

    // Deletes node with provided UFID. If found (and removed), return true. If node was not present, return false.
    // If there are two children, replaces with inorder SUCCESSOR.
    bool Remove(int ufid);

    // Performs a preorder traversal and returns list of node pointers if there were nodes in the tree.
    vector<Node *> TraversePreorder();

    // Performs an inorder traversal and returns list of node pointers if there were nodes in the tree.
    vector<Node *> TraverseInorder();

    // Performs a postorder traversal and returns list of node pointers if there were nodes in the tree.
    vector<Node *> TraversePostorder();
};

#endif //PQ4_GATORBST_H
