#include <catch2/catch_test_macros.hpp>
#include "GatorBST.h"
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

// 辅助函数：提取节点 ID 列表
vector<int> get_ids(const vector<Node*>& nodes) {
    vector<int> ids;
    for (auto n : nodes) if (n) ids.push_back(n->ufid);
    return ids;
}

TEST_CASE("GatorBST Part A & B Final Fix", "[bst]") {
    GatorBST tree;

    SECTION("Initial State and Empty Checks") {
        REQUIRE(tree.Height() == 0);
        REQUIRE(tree.TraverseInorder().empty());
        REQUIRE(tree.SearchName("None").empty());
        REQUIRE(tree.SearchID(999) == std::nullopt);
    }

    SECTION("Successor Replacement and Property Check") {
        tree.Insert(50, "A");
        tree.Insert(30, "B");
        tree.Insert(70, "C");
        tree.Insert(60, "F"); // 50 的后继
        
        // 删除 50，后继 60 应该上位
        REQUIRE(tree.Remove(50) == true);
        REQUIRE(tree.SearchID(50) == std::nullopt);
        
        auto res = tree.SearchID(60);
        REQUIRE(res.has_value());
        REQUIRE(res.value() == "F");
        
        // 验证结构：60 应该是新的根
        REQUIRE(tree.TraversePreorder()[0]->ufid == 60);
    }

    SECTION("Mutant Killer: Successor is Immediate Right Child") {
        // 针对最后一个 Mutant：后继节点没有左子树且就是待删节点的右孩子
        GatorBST t2;
        t2.Insert(10, "Root");
        t2.Insert(20, "Successor");
        t2.Insert(25, "SuccessorRightChild");
        
        REQUIRE(t2.Remove(10) == true);
        REQUIRE(t2.TraversePreorder()[0]->ufid == 20);
        REQUIRE(get_ids(t2.TraverseInorder()) == vector<int>{20, 25});
        REQUIRE(t2.Height() == 2);
    }

    SECTION("Correct Postorder and SearchName Sorting") {
        GatorBST t3;
        t3.Insert(300, "Alice");
        t3.Insert(100, "Alice");
        t3.Insert(200, "Alice");
        t3.Insert(50, "Bob");

        // 1. 验证 SearchName 升序排列
        vector<int> expected_name = {100, 200, 300};
        REQUIRE(t3.SearchName("Alice") == expected_name);

        // 2. 修正后的后序遍历验证
        // 结构：300(根) -> 100(左) -> 50(100左), 200(100右)
        // 后序：50, 200, 100, 300
        vector<int> expected_post = {50, 200, 100, 300};
        REQUIRE(get_ids(t3.TraversePostorder()) == expected_post);
    }

    SECTION("Full Clearance Height Verification") {
        // 重新创建一个树确保完全清空
        GatorBST t4;
        t4.Insert(10, "A");
        t4.Insert(20, "B");
        t4.Remove(10);
        t4.Remove(20);
        // 这次高度必须是 0
        REQUIRE(t4.Height() == 0);
        REQUIRE(t4.TraverseInorder().empty());
    }
}
