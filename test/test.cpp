#include <catch2/catch_test_macros.hpp>
#include "GatorBST.h"
#include <vector>
#include <string>
#include <algorithm>

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
        
        // 检查搜索
        auto result = tree.SearchID(30);
        REQUIRE(result.has_value() == true);
        REQUIRE(result.value() == "Left");

        // 检查重复插入是否失败
        REQUIRE(tree.Insert(50, "Duplicate") == false);
        REQUIRE(tree.Height() == 2); // 50是根，30/70是孩子，高度应为2
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

    SECTION("Remove node with two children (Successor Test)") {
        // 根据题目要求：If there are two children, replaces with inorder SUCCESSOR.
        // 删除 50，后继应该是 60
        REQUIRE(tree.Remove(50) == true);
        auto inorder = tree.TraverseInorder();
        REQUIRE(inorder[0]->ufid == 20);
        REQUIRE(inorder[3]->ufid == 70); // 检查结构是否依然平衡
    }

    SECTION("Remove non-existent node") {
        REQUIRE(tree.Remove(999) == false);
    }
}

TEST_CASE("GatorBST Name Search and Traversals", "[advanced]") {
    GatorBST tree;
    tree.Insert(100, "Alice");
    tree.Insert(50, "Bob");
    tree.Insert(150, "Alice");
    tree.Insert(200, "Charlie");

    SECTION("SearchName multiple results") {
        // SearchName 应该返回升序排列的 ID
        vector<int> expected = {100, 150};
        REQUIRE(tree.SearchName("Alice") == expected);
        REQUIRE(tree.SearchName("Nobody").empty());
    }

    SECTION("Traversals") {
        // Inorder: 50, 100, 150, 200
        auto inorder = tree.TraverseInorder();
        REQUIRE(inorder.size() == 4);
        REQUIRE(inorder[0]->ufid == 50);
        REQUIRE(inorder[3]->ufid == 200);

        // Preorder: 100, 50, 150, 200
        auto preorder = tree.TraversePreorder();
        REQUIRE(preorder[0]->ufid == 100);
    }
}

TEST_CASE("GatorBST Edge Cases", "[edge]") {
    GatorBST tree;

    SECTION("Large number of insertions") {
        for(int i = 1; i <= 100; ++i) {
            tree.Insert(i, "Student");
        }
        REQUIRE(tree.Height() >= 7); // 如果是平衡的应该是7左右，如果不平衡可能是100
        REQUIRE(tree.SearchID(50).has_value());
    }

    SECTION("Removing Root until empty") {
        tree.Insert(10, "R");
        REQUIRE(tree.Remove(10) == true);
        REQUIRE(tree.Height() == 0);
        REQUIRE(tree.Remove(10) == false);
    }
}
