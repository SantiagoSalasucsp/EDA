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

#define MAXNODES 2 // Valor original del usuario.
#define MINNODES 1 // Valor original del usuario.

using namespace std;

#define ASSERT assert // RTree uses ASSERT( condition )
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

    int m_min[2];                      ///< Min dimensions of bounding box
    int m_max[2];                      ///< Max dimensions of bounding box
};

struct  Node;

struct Branch
{
    Rect m_rect;                                  ///< Bounds
    Node* m_child;                                ///< Child node
    vector<pair<int, int>> m_data;                              ///< Data Id (El objeto, un polígono de puntos)
};

struct Node
{
    bool IsInternalNode() { return (m_level > 0); } // Not a leaf, but a internal node
    bool IsLeaf() { return (m_level == 0); } // A leaf, contains data

    int m_count;                                  ///< Count
    int m_level;                                  ///< Leaf is zero, others positive
    Branch m_branch[MAXNODES+1];                    ///< Branch
};

/// A link list of nodes for reinsertion after a delete operation
struct ListNode
{
    ListNode* m_next;                             ///< Next in list
    Node* m_node;                                 ///< Node
};

/// Variables for finding a split partition
struct PartitionVars
{
    enum { NOT_TAKEN = -1 }; // indicates that position

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
  vector<vector<pair<int, int>>> mObjs; // Almacena todos los objetos de datos

    void Insert(const int a_min[2], const int a_max[2], vector<pair<int, int>>& a_dataId);
    void Remove(const int a_min[2], const int a_max[2], const vector<pair<int, int>>& a_dataId);
    void RemoveAll();

    // NUEVA FUNCIÓN: Búsqueda (Algoritmo SEARCH - 3.1)
    bool Search(const Rect& a_rect, vector<vector<pair<int, int>>>& a_results);
    
    // NUEVA FUNCIÓN: Retorna los objetos para la visualización
    vector<vector<pair<int, int>>> getObjects() const;

    /// Count the data elements in this container.  This is slow as no internal counter is maintained.
    int Count();

    /// get the MBRs;
    bool getMBRs(vector<vector<vector<pair<int, int>>>>& mbrs_n);
    Rect MBR(vector<pair<int, int>> pol);


protected:

    Node* AllocNode();
    void FreeNode(Node* a_node);
    void InitNode(Node* a_node);
    void InitRect(Rect* a_rect);
    
    // Funciones renombradas para coincidir con Guttman (Ver tabla en la respuesta)
    bool InsertRec(const Branch& a_branch, Node* a_node, Node** a_newNode, int a_level); // Antes: InsertRectRec
    bool InsertRect(const Branch& a_branch, Node** a_root, int a_level);
    Rect NodeCover(Node* a_node);
    bool AddBranch(const Branch* a_branch, Node* a_node, Node** a_newNode);
    void DisconnectBranch(Node* a_node, int a_index);
    int ChooseLeaf(const Rect* a_rect, Node* a_node); // Antes: PickBranch
    Rect CombineRect(const Rect* a_rectA, const Rect* a_rectB);
    void SplitNode(Node* a_node, const Branch* a_branch, Node** a_newNode); // Mismo nombre en el paper
    float CalcRectArea(Rect* a_rect);
    void GetBranches(Node* a_node, const Branch* a_branch, PartitionVars* a_parVars);
    void QuadraticSplit(PartitionVars* a_parVars, int a_minFill); // Antes: ChoosePartition
    void LoadNodes(Node* a_nodeA, Node* a_nodeB, PartitionVars* a_parVars);
    void InitParVars(PartitionVars* a_parVars, int a_maxRects, int a_minFill);
    void PickSeeds(PartitionVars* a_parVars); // Mismo nombre en el paper
    void Classify(int a_index, int a_group, PartitionVars* a_parVars);
    bool RemoveRect(Rect* a_rect, const vector<pair<int, int>>& a_id, Node** a_root);
    bool DeleteRec(Rect* a_rect, const vector<pair<int, int>>& a_id, Node* a_node, ListNode** a_listNode); // Antes: RemoveRectRec
    ListNode* AllocListNode();
    void FreeListNode(ListNode* a_listNode);
    
    bool Overlap(const Rect* a_rectA, const Rect* a_rectB) const;
        bool Overlap2(Rect* a_rectA, Rect* a_rectB) const; // Deja Overlap2 como estaba si solo se usa internamente

        void ReInsert(Node* a_node, ListNode** a_listNode);
        void RemoveAllRec(Node* a_node);
        void Reset();
        void CountRec(Node* a_node, int& a_count);
    
    void CopyRec(Node* current, Node* other);
    
    // Función auxiliar para Search (3.1)
    void SearchRec(Node* a_node, const Rect& a_rect, vector<vector<pair<int, int>>>& a_results);

    Node* m_root;                                    ///< Root of tree
    float m_unitSphereVolume;                 ///< Unit sphere constant for required number of dimensions
};

#endif
