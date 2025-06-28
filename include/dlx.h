#ifndef DLX_H
#define DLX_H

#include <array>
#include <climits>
#include <cstddef>
#include <iostream>
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

    static void printBoard(const Board& board) {
        std::string h_line = "";
        for (int i = 0; i < SubGridSize; ++i) {
            h_line += std::string(SubGridSize * 3, '-');
            if (i < SubGridSize - 1) {
                h_line += "+";
            }
        }

        for (int i = 0; i < N; ++i) {
            if (i % SubGridSize == 0 && i != 0) {
                std::cout << h_line << "\n";
            }
            for (int j = 0; j < N; ++j) {
                if (j % SubGridSize == 0 && j != 0) {
                    std::cout << "|";
                }
                
                if (board[i][j] == 0) {
                    std::cout << " . ";
                } else {
                    if (board[i][j] < 10) {
                        std::cout << " " << board[i][j] << " ";
                    } else {
                        std::cout << board[i][j] << " ";
                    }
                }
            }
            std::cout << "\n";
        }
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

    static void addKnownConstraint(int row, int col, int num) {
        // box index 0-8
        int box = (row/3) * 3 + (col/3);

        // indices for the 4 constraints
        int cell_constraint = row*9 + col;
        int row_constraint = 81 + row*9 + num;
        int col_constraint = 162 + col*9 + num;
        int box_constraint = 243 + box*9 + num;

        std::array<ColumnNode*, CONSTRAINT_NUM> cols = {
            &columns[cell_constraint], 
            &columns[row_constraint], 
            &columns[col_constraint], 
            &columns[box_constraint]
        };

        DataNode* row_start = newNode(cols[0]);
        cols[0]->size++;
        cols[0]->U->linkDown(row_start);

        DataNode* curr = row_start;
        for (int i = 1; i < CONSTRAINT_NUM; i++) {
            DataNode* newNode_ptr = newNode(cols[i]);
            curr->linkRight(newNode_ptr);
            cols[i]->size++;
            cols[i]->U->linkDown(newNode_ptr);
            curr = newNode_ptr;
        }
    }

    static void addUnknownConstraints(int row, int col) {
        for (int num = 0; num < N; num++) {
            addKnownConstraint(row, col, num);
        }
    }

    static bool search(int k) {
        if (header.R == &header) {
            return true;
        }

        ColumnNode* col = selectColumn();
        col->cover();

        for (DataNode* r = col->D; r != col; r = r->D) {
            answer.add(r);

            for (DataNode* j = r->R; j != r; j = j->R) {
                j->C.cover();
            }

            if (search(k+1)) {
                return true;
            }
            answer.pop_back();
            for (DataNode* j = r->L; j != r; j = j->L) {
                j->C->uncover();
            }
        }

        col->uncover();
        return false;
    }

    static ColumnNode* selectColumn() {
        ColumnNode* selected = nullptr;
        int minSize = INT_MAX;

        for (ColumnNode* col = (ColumnNode*)header.R; col != &header; col = (ColumnNode*)col->R) {
            if (col->size < minSize) {
                minSize = col->size;
                selected = col;

                if (minSize == 0) {
                    break;
                }
            }
        }

        return selected;
    }

    static void allocateNodes(const Board& puzzle) {
        int empty_cells = 0;
        for (int r = 0; r < N; r++) {
            for (int c = 0; c < N; c++) {
                if (puzzle[r][c] == 0) empty_cells++;
            }
        }
        int filled_cells = A - empty_cells;
        int max_nodes = (filled_cells * CONSTRAINT_NUM) + (empty_cells * N * CONSTRAINT_NUM);

        node_pool.resize(max_nodes);
        for (size_t i = 0; i < max_nodes; i++) {
            node_pool[i] = new DataNode();
        }
        pool_idx = 0;
    }

    static DataNode* newNode(ColumnNode* c) {
        DataNode* node = node_pool[pool_idx++];
        node->C = c;
        return node;
    }

    static void cleanup() {
        for (auto ptr : node_pool) {
            delete ptr;
        }
        node_pool.clear();
        pool_idx = 0;
    }
};

#endif
