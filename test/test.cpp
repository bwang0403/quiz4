#include <catch2/catch_test_macros.hpp>
#include "GatorBST.h"
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

// 辅助函数：将 Node* 向量转换为 ufid 向量
vector<int> get_ids(const vector<Node*>& nodes) {
    vector<int> ids;
    for (auto n : nodes) {
        if (n) ids.push_back(n->ufid);
    }
    return ids;
}

TEST_CASE("GatorBST Basic Functionality", "[basic]") {
    GatorBST tree;

    SECTION("Initial State and Empty Checks") {
        REQUIRE(tree.Height() == 0);
        REQUIRE(tree.TraverseInorder().empty());
        REQUIRE(tree.SearchName("Any").empty()); // 捕捉空树搜索Bug
        REQUIRE(tree.SearchID(111) == std::nullopt);
    }

    SECTION("Insertion and Basic Search") {
        REQUIRE(tree.Insert(50, "Root") == true);
        REQUIRE(tree.Insert(30, "Left") == true);
        REQUIRE(tree.Insert(70, "Right") == true);
        
        auto result = tree.SearchID(30);
        REQUIRE(result.has_value() == true);
        REQUIRE(result.value() == "Left");

        REQUIRE(tree.Insert(50, "Duplicate") == false);
        REQUIRE(tree.Height() == 2); 
    }
}

TEST_CASE("GatorBST Deletion Scenarios", "[remove]") {
    GatorBST tree;
    tree.Insert(50, "A");
    tree.Insert(30, "B");
    tree.Insert(70, "C");
    tree.Insert(20, "D");
    tree.Insert(40, "E");
    tree.Insert(60, "F");
    tree.Insert(80, "G");

    SECTION("Successor Replacement and Property Preservation") {
        // 删除 50，后继应该是 60
        REQUIRE(tree.Remove(50) == true);
        REQUIRE(tree.SearchID(50) == std::nullopt);
        
        auto res = tree.SearchID(60);
        REQUIRE(res.has_value());
        REQUIRE(res.value() == "F"); // 确保名字也正确转移或保留
        
        // 验证结构
        vector<int> expected = {20, 30, 40, 60, 70, 80};
        REQUIRE(get_ids(tree.TraverseInorder()) == expected);
    }

    SECTION("Mutant Killer: Successor is Direct Right Child") {
        GatorBST t2;
        t2.Insert(10, "Root");
        t2.Insert(20, "DirectSuccessor");
        t2.Insert(25, "SuccessorRight");
        
        // 删除 10，20 应该是后继且是直系孩子
        REQUIRE(t2.Remove(10) == true);
        REQUIRE(t2.TraversePreorder()[0]->ufid == 20);
        REQUIRE(get_ids(t2.TraverseInorder()) == vector<int>{20, 25});
    }

    SECTION("Full Clearance Height Check") {
        // 修复之前漏掉根节点 50 的问题
        tree.Remove(50); tree.Remove(30); tree.Remove(70);
        tree.Remove(20); tree.Remove(40); tree.Remove(60); tree.Remove(80);
        REQUIRE(tree.Height() == 0); // 确保所有节点移除后高度为0
        REQUIRE(tree.Remove(50) == false); // 二次删除应返回false
    }
}

TEST_CASE("GatorBST Advanced Search and Traversals", "[search]") {
    GatorBST tree;
    tree.Insert(300, "Alice");
    tree.Insert(100, "Alice");
    tree.Insert(200, "Alice");
    tree.Insert(50, "Bob");

    SECTION("SearchName Sorting and Postorder") {
        // 升序：100, 200, 300
        vector<int> expected = {100, 200, 300};
        REQUIRE(tree.SearchName("Alice") == expected);
        
        // 修复后序遍历预期值
        // 结构: 300是根, 100是左, 100有左50和右200
        // 后序应该是: 50, 200, 100, 300
        vector<int> expected_post = {50, 200, 100, 300};
        REQUIRE(get_ids(tree.TraversePostorder()) == expected_post);
    }
}

TEST_CASE("GatorBST Height and Edge Cases", "[edge]") {
    GatorBST tree;
    SECTION("Complex Structure Height") {
        tree.Insert(10, "A");
        tree.Insert(5, "B");
        tree.Insert(15, "C");
        tree.Insert(12, "D");
        tree.Insert(13, "E");
        // 高度应为 4 (10-15-12-13)
        REQUIRE(tree.Height() == 4);
        tree.Remove(13);
        REQUIRE(tree.Height() == 3);
    }
}
