#ifndef RTREE_H
#define RTREE_H

#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <stdlib.h>

#include <algorithm>
#include <functional>
#include <vector>
#include <limits>
#include <iostream>

#define MAXNODES 2
#define MINNODES 1

using namespace std;

#define ASSERT assert
#define Min min
#define Max max


struct Rect
{
    Rect() {}

    Rect(int a_minX, int a_minY, int a_maxX, int a_maxY)
    {
        m_min[0] = a_minX;
        m_min[1] = a_minY;

        m_max[0] = a_maxX;
        m_max[1] = a_maxY;
    }

    int m_min[2];
    int m_max[2];
};

struct Node;

struct Branch
{
    Rect m_rect;
    Node* m_child;
    vector<pair<int, int>> m_data;
};

struct Node
{
    bool IsInternalNode() { return (m_level > 0); }
    bool IsLeaf() { return (m_level == 0); }

    int m_count;
    int m_level;
    Branch m_branch[MAXNODES+1];
};

struct ListNode
{
    ListNode* m_next;
    Node* m_node;
};

struct PartitionVars
{
    enum { NOT_TAKEN = -1 };

    int m_partition[MAXNODES + 1];
    int m_total;
    int m_minFill;
    int m_count[2];
    Rect m_cover[2];
    float m_area[2];

    Branch m_branchBuf[MAXNODES + 1];
    int m_branchCount;
    Rect m_coverSplit;
    float m_coverSplitArea;
};


class RTree
{
public:

    RTree();
    RTree(const RTree& other);
    virtual ~RTree();
    vector<vector<pair<int, int>>> mObjs;

    void Insert(const int a_min[2], const int a_max[2], vector<pair<int, int>>& a_dataId);
    void Remove(const int a_min[2], const int a_max[2], const vector<pair<int, int>>& a_dataId);
    void RemoveAll();

    bool Search(const Rect& a_rect, vector<vector<pair<int, int>>>& a_results);

    vector<vector<pair<int, int>>> getObjects() const;

    int Count();

    bool getMBRs(vector<vector<vector<pair<int, int>>>>& mbrs_n);
    Rect MBR(vector<pair<int, int>> pol);


protected:

    Node* AllocNode();
    void FreeNode(Node* a_node);
    void InitNode(Node* a_node);
    void InitRect(Rect* a_rect);

    bool InsertRec(const Branch& a_branch, Node* a_node, Node** a_newNode, int a_level);
    bool InsertRect(const Branch& a_branch, Node** a_root, int a_level);
    Rect NodeCover(Node* a_node);
    bool AddBranch(const Branch* a_branch, Node* a_node, Node** a_newNode);
    void DisconnectBranch(Node* a_node, int a_index);
    int ChooseLeaf(const Rect* a_rect, Node* a_node);
    Rect CombineRect(const Rect* a_rectA, const Rect* a_rectB);
    void SplitNode(Node* a_node, const Branch* a_branch, Node** a_newNode);
    float CalcRectArea(Rect* a_rect);
    void GetBranches(Node* a_node, const Branch* a_branch, PartitionVars* a_parVars);
    void QuadraticSplit(PartitionVars* a_parVars, int a_minFill);
    void LoadNodes(Node* a_nodeA, Node* a_nodeB, PartitionVars* a_parVars);
    void InitParVars(PartitionVars* a_parVars, int a_maxRects, int a_minFill);
    void PickSeeds(PartitionVars* a_parVars);
    void Classify(int a_index, int a_group, PartitionVars* a_parVars);
    bool RemoveRect(Rect* a_rect, const vector<pair<int, int>>& a_id, Node** a_root);
    bool DeleteRec(Rect* a_rect, const vector<pair<int, int>>& a_id, Node* a_node, ListNode** a_listNode);
    ListNode* AllocListNode();
    void FreeListNode(ListNode* a_listNode);

    bool Overlap(const Rect* a_rectA, const Rect* a_rectB) const;
    bool Overlap2(Rect* a_rectA, Rect* a_rectB) const;

    void ReInsert(Node* a_node, ListNode** a_listNode);
    void RemoveAllRec(Node* a_node);
    void Reset();
    void CountRec(Node* a_node, int& a_count);

    void CopyRec(Node* current, Node* other);

    void SearchRec(Node* a_node, const Rect& a_rect, vector<vector<pair<int, int>>>& a_results);

    Node* m_root;
    float m_unitSphereVolume;
};

#endif
