#include <catch2/catch_test_macros.hpp>
#include "GatorBST.h"
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

// 辅助函数：将节点指针向量转换为 UFID 向量，方便进行全序列比对 [cite: 19, 20, 21]
vector<int> get_ids(const vector<Node*>& nodes) {
    vector<int> ids;
    for (auto n : nodes) if (n) ids.push_back(n->ufid);
    return ids;
}

TEST_CASE("BST Comprehensive Verification", "[bst]") {
    GatorBST tree;

    SECTION("1. Initial and Empty State") {
        REQUIRE(tree.Height() == 0); // 空树高度为0 [cite: 6, 25]
        REQUIRE(tree.TraverseInorder().empty());
        REQUIRE(tree.SearchName("Missing").empty()); // 无匹配名应返回空向量 [cite: 15, 34]
        REQUIRE(tree.SearchID(999) == std::nullopt); // 未找到返回 nullopt [cite: 11, 30]
    }

    SECTION("2. Basic Insert and Duplicate Check") {
        REQUIRE(tree.Insert(50, "Root") == true);
        REQUIRE(tree.Insert(30, "Left") == true);
        REQUIRE(tree.Insert(70, "Right") == true);
        REQUIRE(tree.Height() == 2); // 1-based height [cite: 5, 25]
        // 验证 UFID 已存在时插入失败 [cite: 8, 27]
        REQUIRE(tree.Insert(50, "Duplicate") == false);
    }

    SECTION("3. SearchName Sorting") {
        // GatorBST 要求 SearchName 返回按 UFID 升序排列的向量 [cite: 14, 33]
        tree.Insert(300, "Alice");
        tree.Insert(100, "Alice");
        tree.Insert(200, "Alice");
        
        vector<int> expected = {100, 200, 300};
        REQUIRE(tree.SearchName("Alice") == expected);
    }

    SECTION("4. Complex Removal - Successor Logic") {
        // 构建结构验证删除有两个孩子的节点使用后继节点替换 [cite: 17, 36]
        tree.Insert(50, "A");
        tree.Insert(30, "B");
        tree.Insert(80, "C");
        tree.Insert(60, "F"); // 50 的后继
        tree.Insert(70, "G"); // 后继节点的右孩子 (抓捕关键 Mutant)
        
        // 删除根节点 50，后继 60 应该上位，其右孩子 70 重新挂载
        REQUIRE(tree.Remove(50) == true);
        REQUIRE(tree.SearchID(50) == std::nullopt);
        
        // 验证 60 的属性正确继承
        auto res = tree.SearchID(60);
        REQUIRE(res.has_value());
        REQUIRE(res.value() == "F");
        
        // 验证整体中序结构
        vector<int> expected_in = {30, 60, 70, 80};
        REQUIRE(get_ids(tree.TraverseInorder()) == expected_in);
        // 验证前序，确保 60 成为根或正确移位
        REQUIRE(tree.TraversePreorder()[0]->ufid == 60);
    }

    SECTION("5. Postorder Traversal Fix") {
        // 基于 Gradescope 报错修正的结构逻辑
        // 插入顺序: 300, 100, 200, 50
        tree.Insert(300, "R");
        tree.Insert(100, "L");
        tree.Insert(200, "L-R");
        tree.Insert(50, "L-L");
        
        // 后序: 左, 右, 根 -> 50, 200, 100, 300
        vector<int> expected_post = {50, 200, 100, 300};
        REQUIRE(get_ids(tree.TraversePostorder()) == expected_post);
    }

    SECTION("6. Full Clearance Verification") {
        // 使用独立实例确保高度计算不受干扰
        GatorBST t_clear;
        t_clear.Insert(10, "X");
        t_clear.Insert(20, "Y");
        REQUIRE(t_clear.Remove(10) == true);
        REQUIRE(t_clear.Remove(20) == true);
        REQUIRE(t_clear.Height() == 0); // 彻底清空高度必为 0 [cite: 6, 25]
        REQUIRE(t_clear.TraverseInorder().empty());
    }
}
