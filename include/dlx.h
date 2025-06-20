#ifndef DLX_H
#define DLX_H

#include <array>
#include <vector>

template<int SubGridSize>

class DLX {
public:
    static const int N = SubGridSize * SubGridSize;
    static const int A = N * N;
    static const int CONSTRAINT_NUM = 4;
    static const int COL_NUM = A * CONSTRAINT_NUM;

    using Board = std::array<std::array<int, N>, N>;

    static Board solve(const Board& puzzle) {

    }

private:
    struct ColumnNode;
    struct DataNode {
        DataNode *L, *R, *U, *D;
        ColumnNode *C;

        DataNode() : L(this), R(this), U(this), D(this), C(nullptr) {}
        DataNode(ColumnNode *c) : L(this), R(this), U(this), D(this), C(c) {}

        void linkDown(DataNode *node) { 
            node->D = D;
            node->D->U = node;
            node->U = this;
            D = node;
        };
        void linkRight(DataNode *node) {
            node->R = R;
            node->R->L = node;
            node->L = this;
            R = node;
        };        
        void unlinkLR() { L->R = R; R->L = L; };
        void relinkLR() { L->R = this; R->L = this; };
        void unlinkUD() { U->D = D; D->U = U; };
        void relinkUD() { U->D = this; D->U = this; };
    };

    struct ColumnNode : public DataNode {
        int size;
        int name;

        ColumnNode(int n = -1) : DataNode(), size(0), name(n) { this->C = this; }

        // Covers this column and all associated rows from the DLX matrix
        void cover() {
            this->unlinkLR();
            for (DataNode *i = this->D; i != this; i = i->D) {
                for (DataNode *j = i->R; j != i; j = j->R) {
                    j->unlinkUD();
                    j->C->size--;
                }
            }
        };

        // Reinserts all nodes that were removed using cover()
        void uncover() {
            for (DataNode *i = this->U; i != this; i = i->U) {
                for (DataNode *j = i->L; j != i; j = j->L) {
                    j->C->size++;
                    j->relinkUD();
                }
            }
            this->relinkLR();
        };
    };

    // templated static member var
    inline static std::array<ColumnNode, COL_NUM> columns;
    inline static ColumnNode header;
    inline static std::vector<DataNode*> answer;
    inline static std::vector<DataNode*> node_pool;
    inline static size_t pool_idx = 0;

    static void initDLX() {
        header = ColumnNode(-1);
        ColumnNode *prev = header;
        for (int i = 0; i < COL_NUM; i++) {
            columns[i] = ColumnNode(i);
            prev->linkRight(&columns[i]);
            prev = &columns[i];
        }
    }
};

#endif
