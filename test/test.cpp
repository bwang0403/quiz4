#include <catch2/catch_test_macros.hpp>
#include "GatorBST.h"
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

// 辅助函数：将遍历得到的 Node* 向量转换为 ufid 向量，方便进行全序列对比
vector<int> get_ids(const vector<Node*>& nodes) {
    vector<int> ids;
    for (auto n : nodes) {
        if (n) ids.push_back(n->ufid);
    }
    return ids;
}

TEST_CASE("GatorBST Basic Functionality", "[basic]") {
    GatorBST tree;

    SECTION("Initial State") {
        REQUIRE(tree.Height() == 0);
        REQUIRE(tree.TraverseInorder().empty());
        REQUIRE(tree.TraversePreorder().empty());
        REQUIRE(tree.TraversePostorder().empty());
        REQUIRE(tree.SearchID(111) == std::nullopt);
    }

    SECTION("Insertion and Property Search") {
        REQUIRE(tree.Insert(50, "RootNode") == true);
        REQUIRE(tree.Insert(30, "LeftNode") == true);
        REQUIRE(tree.Insert(70, "RightNode") == true);
        
        auto result = tree.SearchID(30);
        REQUIRE(result.has_value() == true);
        REQUIRE(result.value() == "LeftNode");

        // 重复插入必须失败
        REQUIRE(tree.Insert(50, "Duplicate") == false);
        REQUIRE(tree.Height() == 2); 
    }
}

TEST_CASE("GatorBST Complex Deletion Scenarios", "[remove]") {
    GatorBST tree;
    // 构建标准 BST 结构
    tree.Insert(50, "A");
    tree.Insert(30, "B");
    tree.Insert(70, "C");
    tree.Insert(20, "D");
    tree.Insert(40, "E");
    tree.Insert(60, "F");
    tree.Insert(80, "G");

    SECTION("Remove Node with Two Children - Successor Property Check") {
        // 这是为了抓住那个“只改 ID 不改 Name”或者“指针丢失”的 Mutant
        // 50 的后继是 60，60 应该带着它原本的 Name "F" 替换上去
        REQUIRE(tree.Remove(50) == true);
        
        // 1. 验证原来的 50 彻底消失
        REQUIRE(tree.SearchID(50) == std::nullopt);
        
        // 2. 验证后继节点 60 的属性依然正确
        auto res = tree.SearchID(60);
        REQUIRE(res.has_value());
        REQUIRE(res.value() == "F"); 
        
        // 3. 验证完整的中序序列，确保没有任何节点丢失或位置错误
        vector<int> expected = {20, 30, 40, 60, 70, 80};
        REQUIRE(get_ids(tree.TraverseInorder()) == expected);
        
        // 4. 验证前序遍历，确保 60 确实变成了新的根节点
        REQUIRE(tree.TraversePreorder()[0]->ufid == 60);
    }

    SECTION("Successor with Right Child Removal") {
        // 专门测试：后继节点自己还有一个右孩子的情况
        GatorBST t2;
        t2.Insert(50, "Root");
        t2.Insert(80, "R");
        t2.Insert(60, "Successor");
        t2.Insert(70, "SuccessorRightChild");
        
        // 删除 50，后继 60 应该上位，70 应该挂到 80 的左边
        REQUIRE(t2.Remove(50) == true);
        vector<int> expected_in = {60, 70, 80};
        REQUIRE(get_ids(t2.TraverseInorder()) == expected_in);
        REQUIRE(t2.Height() == 3); // 确保高度正确更新
    }

    SECTION("Remove non-existent and empty") {
        REQUIRE(tree.Remove(999) == false);
        // 清空树
        tree.Remove(30); tree.Remove(70); tree.Remove(20);
        tree.Remove(40); tree.Remove(60); tree.Remove(80);
        REQUIRE(tree.Height() == 0);
    }
}

TEST_CASE("GatorBST SearchName and Order Verification", "[search]") {
    GatorBST tree;
    tree.Insert(300, "Alice");
    tree.Insert(100, "Alice");
    tree.Insert(200, "Alice");
    tree.Insert(50, "Bob");

    SECTION("SearchName Strict Sorting") {
        // 必须按 ID 升序：100, 200, 300
        vector<int> expected = {100, 200, 300};
        REQUIRE(tree.SearchName("Alice") == expected);
        
        // 删除中间一个后再查
        tree.Remove(200);
        vector<int> expected_after = {100, 300};
        REQUIRE(tree.SearchName("Alice") == expected_after);
    }

    SECTION("Postorder Sequence Check") {
        // 验证后序遍历的精确顺序
        // 结构：50(Bob) -> 300(Alice) -> 100(Alice) -> 200(Alice)
        // 这个结构的后序应该是：200, 100, 300, 50
        auto post_ids = get_ids(tree.TraversePostorder());
        vector<int> expected_post = {200, 100, 300, 50};
        REQUIRE(post_ids == expected_post);
    }
}

TEST_CASE("GatorBST Edge Cases and Height", "[edge]") {
    GatorBST tree;

    SECTION("Left Leaning vs Right Leaning Height") {
        // 右斜树
        for(int i = 1; i <= 5; i++) tree.Insert(i, "R");
        REQUIRE(tree.Height() == 5);
        
        // 左斜树
        GatorBST treeL;
        for(int i = 5; i >= 1; i--) treeL.Insert(i, "L");
        REQUIRE(treeL.Height() == 5);
    }

    SECTION("Root Deletion Scenarios") {
        // 只有一个左孩子的根
        tree.Insert(100, "Root");
        tree.Insert(50, "L");
        REQUIRE(tree.Remove(100) == true);
        REQUIRE(tree.Height() == 1);
        REQUIRE(tree.SearchID(50).has_value());
    }
}
