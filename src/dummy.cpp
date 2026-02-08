#include "GatorBST.h"

GatorBST::GatorBST() {
    root = nullptr;
}

GatorBST::~GatorBST() {
}

int GatorBST::Height() {
    return 0;
}

bool GatorBST::Insert(const int ufid, const string &name) {
    return false;
}

optional<string_view> GatorBST::SearchID(const int ufid) {
    return nullopt;
}

vector<int> GatorBST::SearchName(const string &name) {
    return {};
}

bool GatorBST::Remove(int ufid) {
    return false;
}

vector<Node *> GatorBST::TraversePreorder() {
    return {};
}

vector<Node *> GatorBST::TraverseInorder() {
    return {};
}

vector<Node *> GatorBST::TraversePostorder() {
    return {};
}
