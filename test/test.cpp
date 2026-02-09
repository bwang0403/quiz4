#include <catch2/catch_test_macros.hpp>
#include "GatorBST.h"
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

// 辅助函数：将节点指针向量转换为 UFID 向量
vector<int> get_ids(const vector<Node*>& nodes) {
    vector<int> ids;
    for (auto n : nodes) if (n) ids.push_back(n->ufid);
    return ids;
}

TEST_CASE("BST Ultimate Verification", "[bst]") {
    GatorBST tree;

    SECTION("1. Empty State & All Traversals") {
        REQUIRE(tree.Height() == 0);
        // 关键补充：测试所有遍历在空树下的表现，防止空指针崩溃
        REQUIRE(tree.TraverseInorder().empty());
        REQUIRE(tree.TraversePreorder().empty());   // 新增
        REQUIRE(tree.TraversePostorder().empty());  // 新增
        REQUIRE(tree.SearchName("Missing").empty());
        REQUIRE(tree.SearchID(999) == std::nullopt);
    }

    SECTION("2. Single Node & Height 1") {
        // 关键补充：明确验证高度为 1 的情况
        REQUIRE(tree.Insert(10, "Root") == true);
        REQUIRE(tree.Height() == 1); 
        REQUIRE(tree.SearchID(10).value() == "Root");
    }

    SECTION("3. Basic Insert & Duplicate") {
        tree.Insert(50, "Root");
        tree.Insert(30, "Left");
        tree.Insert(70, "Right");
        REQUIRE(tree.Height() == 2);
        REQUIRE(tree.Insert(50, "Duplicate") == false);
    }

    SECTION("4. SearchName Sorting") {
        tree.Insert(300, "Alice");
        tree.Insert(100, "Alice");
        tree.Insert(200, "Alice");
        vector<int> expected = {100, 200, 300};
        REQUIRE(tree.SearchName("Alice") == expected);
    }

    SECTION("5. Complex Removal (Successor is Deep)") {
        // 场景：后继节点在右子树的左分支深处 (例如 60 是 80 的左孩子)
        tree.Insert(50, "Root");
        tree.Insert(30, "L");
        tree.Insert(80, "R");
        tree.Insert(60, "Successor"); // 50 的后继
        tree.Insert(70, "SuccRight"); // 后继的右孩子
        
        // 删除 50
        REQUIRE(tree.Remove(50) == true);
        REQUIRE(tree.SearchID(50) == std::nullopt);
        
        // 验证 60 上位
        REQUIRE(tree.SearchID(60).value() == "Successor");
        // 验证中序完整性
        vector<int> expected_in = {30, 60, 70, 80};
        REQUIRE(get_ids(tree.TraverseInorder()) == expected_in);
    }

    SECTION("6. Mutant Killer: Successor is Immediate Right Child") {
        // 关键新增场景：待删节点 10 的右孩子 20 没有左孩子
        // 这时 20 就是后继，应该直接上位
        GatorBST t2;
        t2.Insert(10, "Root");
        t2.Insert(20, "Right");
        t2.Insert(30, "RightRight");
        
        REQUIRE(t2.Remove(10) == true);
        // 验证 20 成为根
        REQUIRE(t2.TraversePreorder()[0]->ufid == 20);
        // 验证结构保持
        vector<int> expected = {20, 30};
        REQUIRE(get_ids(t2.TraverseInorder()) == expected);
    }

    SECTION("7. Mutant Killer: Remove Root with Left Child Only") {
        // 关键新增场景：删除只有左孩子的根
        GatorBST t3;
        t3.Insert(20, "Root");
        t3.Insert(10, "Left");
        
        REQUIRE(t3.Remove(20) == true);
        // 验证 10 成为新根
        REQUIRE(t3.Height() == 1);
        REQUIRE(t3.TraversePreorder()[0]->ufid == 10);
        REQUIRE(t3.SearchID(20) == std::nullopt);
    }

    SECTION("8. Postorder Fix") {
        // 300(Root) -> 100(L) -> 50(L-L), 200(L-R)
        tree.Insert(300, "R");
        tree.Insert(100, "L");
        tree.Insert(200, "L-R");
        tree.Insert(50, "L-L");
        
        // Postorder: 50, 200, 100, 300
        vector<int> expected_post = {50, 200, 100, 300};
        REQUIRE(get_ids(tree.TraversePostorder()) == expected_post);
    }

    SECTION("9. Full Clearance") {
        GatorBST t4;
        t4.Insert(1, "A");
        t4.Insert(2, "B");
        t4.Remove(1);
        t4.Remove(2);
        REQUIRE(t4.Height() == 0);
        REQUIRE(t4.TraverseInorder().empty());
    }
}
