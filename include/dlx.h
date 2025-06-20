#ifndef DLX_H
#define DLX_H

template<int SubGridSize>
class DLX {
public:
    static const int N = SubGridSize * SubGridSize;
    static const int A = N * N;
    static const int CONSTRAINT_NUM = 4;
    static const int COL_NUM = A * CONSTRAINT_NUM;

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

        void cover() {
            this->unlinkLR();
        };
        void uncover() {

        };
    };
};

#endif
