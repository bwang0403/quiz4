#include <catch2/catch_test_macros.hpp>
#include "GatorBST.h"
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

// 工具函数：检查 ID 向量是否为升序
bool is_sorted_ids(const vector<int>& ids) {
    return std::is_sorted(ids.begin(), ids.end());
}

TEST_CASE("GatorBST Basic Functionality", "[basic]") {
    GatorBST tree;

    SECTION("Initial State") {
        REQUIRE(tree.Height() == 0);
        REQUIRE(tree.TraverseInorder().empty());
        REQUIRE(tree.SearchID(12345678) == std::nullopt);
    }

    SECTION("Insertion and Search") {
        REQUIRE(tree.Insert(50, "Root") == true);
        REQUIRE(tree.Insert(30, "Left") == true);
        REQUIRE(tree.Insert(70, "Right") == true);
        
        auto result = tree.SearchID(30);
        REQUIRE(result.has_value() == true);
        REQUIRE(result.value() == "Left");

        REQUIRE(tree.Insert(50, "Duplicate") == false);
        // 根据 .h，有节点时高度为路径上的节点数（1-based）
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

    SECTION("Remove leaf node") {
        REQUIRE(tree.Remove(20) == true);
        REQUIRE(tree.SearchID(20) == std::nullopt);
    }

    SECTION("Remove node with one child") {
        tree.Remove(20);
        REQUIRE(tree.Remove(30) == true);
        REQUIRE(tree.SearchID(40).has_value() == true);
    }

    SECTION("Mutant Killer: Successor has a right child") {
        // 这是最容易漏掉的 Mutant：删除有两个孩子的节点，且其后继节点本身有右孩子
        GatorBST t2;
        t2.Insert(50, "Root");
        t2.Insert(20, "L");
        t2.Insert(80, "R");
        t2.Insert(60, "R-L");
        t2.Insert(90, "R-R");
        t2.Insert(70, "R-L-R"); // 这是 80 的后继节点的右孩子
        
        // 删除 50，后继应该是 60
        REQUIRE(t2.Remove(50) == true);
        auto inorder = t2.TraverseInorder();
        REQUIRE(inorder.size() == 5);
        REQUIRE(inorder[2]->ufid == 70); // 确保 70 被正确移位
        
        auto preorder = t2.TraversePreorder();
        REQUIRE(preorder[0]->ufid == 60); // 验证 60 变成了根（或正确的子树顶端）
    }

    SECTION("Remove Root with only one child") {
        GatorBST t3;
        t3.Insert(100, "Root");
        t3.Insert(50, "LeftChild");
        REQUIRE(t3.Remove(100) == true);
        REQUIRE(t3.Height() == 1);
        REQUIRE(t3.SearchID(50).has_value());
    }
}

TEST_CASE("GatorBST Name Search and Traversals", "[advanced]") {
    GatorBST tree;
    // 故意乱序插入同名节点，验证强制排序 Bug
    tree.Insert(300, "Alice");
    tree.Insert(100, "Alice");
    tree.Insert(200, "Alice");
    tree.Insert(50, "Bob");

    SECTION("SearchName Strict Sorting Check") {
        // 关键：SearchName 必须返回升序 ID 向量 [cite: 30]
        vector<int> expected = {100, 200, 300};
        auto result = tree.SearchName("Alice");
        REQUIRE(result == expected);
        REQUIRE(is_sorted_ids(result)); 
        REQUIRE(tree.SearchName("Nobody").empty());
    }

    SECTION("Traversals Detail") {
        auto inorder = tree.TraverseInorder();
        REQUIRE(inorder.size() == 4);
        REQUIRE(inorder[0]->ufid == 50);
        
        auto postorder = tree.TraversePostorder();
        REQUIRE(postorder.size() == 4);
    }
}

TEST_CASE("GatorBST Edge Cases", "[edge]") {
    GatorBST tree;

    SECTION("Sequential Insertions and Height") {
        // 验证非平衡树在顺序插入时高度正确更新
        for(int i = 1; i <= 5; ++i) {
            tree.Insert(i, "Student");
        }
        REQUIRE(tree.Height() == 5);
        tree.Remove(5);
        REQUIRE(tree.Height() == 4);
    }

    SECTION("Removing Root until empty") {
        tree.Insert(10, "R");
        REQUIRE(tree.Height() == 1);
        REQUIRE(tree.Remove(10) == true);
        REQUIRE(tree.Height() == 0);
        REQUIRE(tree.Remove(10) == false);
        REQUIRE(tree.TraverseInorder().empty());
    }
}
