#ifndef DLX_H
#define DLX_H

class DLX {
public:
private:
    struct ColumnNode;
    struct DataNode {
        DataNode *L, *R, *U, *D;
        ColumnNode *C;

        DataNode() : L(this), R(this), U(this), D(this), C(nullptr) {}
        DataNode(ColumnNode *c) : L(this), R(this), U(this), D(this), C(c) {}

        void linkDown(DataNode *node);
        void linkRight(DataNode *node);        
        void unlinkLR();
        void relinkLR();
        void unlinkUD();
        void relinkUD();
    };

    struct ColumnNode : public DataNode {
        int size;
        int name;

        ColumnNode(int n) : DataNode(), size(0), name(n) { C = this; }

        void cover();
        void uncover();
    };
};

#endif
