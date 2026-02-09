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
        // 测试所有遍历在空树下的表现
        REQUIRE(tree.TraverseInorder().empty());
        REQUIRE(tree.TraversePreorder().empty());
        REQUIRE(tree.TraversePostorder().empty());
        REQUIRE(tree.SearchName("Missing").empty());
        REQUIRE(tree.SearchID(999) == std::nullopt);
        // 尝试从空树删除
        REQUIRE(tree.Remove(10) == false);
    }

    SECTION("2. Single Node & Height 1") {
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
        // 场景：删除根节点，且后继在深处
        tree.Insert(50, "Root");
        tree.Insert(30, "L");
        tree.Insert(80, "R");
        tree.Insert(60, "Successor"); 
        tree.Insert(70, "SuccRight"); 
        
        REQUIRE(tree.Remove(50) == true);
        REQUIRE(tree.SearchID(50) == std::nullopt);
        REQUIRE(tree.SearchID(60).value() == "Successor");
        
        vector<int> expected_in = {30, 60, 70, 80};
        REQUIRE(get_ids(tree.TraverseInorder()) == expected_in);
        // 补充：检查前序以确认结构正确 (60应该是新根)
        REQUIRE(tree.TraversePreorder().front()->ufid == 60);
    }

    SECTION("6. Mutant Killer: Successor is Immediate Right Child") {
        GatorBST t2;
        t2.Insert(10, "Root");
        t2.Insert(20, "Right");
        t2.Insert(30, "RightRight");
        
        REQUIRE(t2.Remove(10) == true);
        REQUIRE(t2.TraversePreorder()[0]->ufid == 20);
        vector<int> expected = {20, 30};
        REQUIRE(get_ids(t2.TraverseInorder()) == expected);
    }

    SECTION("7. Mutant Killer: Remove Root with Left Child Only") {
        GatorBST t3;
        t3.Insert(20, "Root");
        t3.Insert(10, "Left");
        
        REQUIRE(t3.Remove(20) == true);
        REQUIRE(t3.Height() == 1);
        REQUIRE(t3.TraversePreorder()[0]->ufid == 10);
        REQUIRE(t3.SearchID(20) == std::nullopt);
    }

    SECTION("8. Postorder Fix") {
        tree.Insert(300, "R");
        tree.Insert(100, "L");
        tree.Insert(200, "L-R");
        tree.Insert(50, "L-L");
        
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

    // --- ⬇️ 这里是新增的必杀技 ⬇️ ---
    SECTION("10. Non-Root Removals (Critical Gap Fill)") {
        GatorBST t5;
        // 构建树: 50(根) -> 25(左), 75(右)
        // 25 -> 10(左), 30(右)
        t5.Insert(50, "Root");
        t5.Insert(25, "L");
        t5.Insert(75, "R");
        t5.Insert(10, "L-L");
        t5.Insert(30, "L-R");

        // 1. 删除左叶子节点 (非根)
        REQUIRE(t5.Remove(10) == true);
        REQUIRE(t5.SearchID(10) == std::nullopt);
        // 验证父节点(25)的左指针是否正确置空
        vector<int> check1 = {25, 30, 50, 75};
        REQUIRE(get_ids(t5.TraverseInorder()) == check1);

        // 2. 删除右叶子节点 (非根)
        REQUIRE(t5.Remove(75) == true);
        REQUIRE(t5.SearchID(75) == std::nullopt);
        // 验证父节点(50)的右指针
        vector<int> check2 = {25, 30, 50};
        REQUIRE(get_ids(t5.TraverseInorder()) == check2);

        // 3. 删除中间节点 (非根, 有孩子)
        // 此时树结构: 50 -> 25(L). 25 -> 30(R).
        // 删除 25，它有一个右孩子 30。30 应该接替 25 的位置成为 50 的左孩子。
        REQUIRE(t5.Remove(25) == true);
        REQUIRE(t5.SearchID(25) == std::nullopt);
        
        // 最终验证
        REQUIRE(t5.Height() == 2); // 50 -> 30
        REQUIRE(t5.TraversePreorder()[0]->ufid == 50); // 根还是50
        REQUIRE(t5.TraversePreorder()[1]->ufid == 30); // 30必须连在50下面
        REQUIRE(t5.SearchID(30).has_value());
    }
}
