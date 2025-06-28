#ifndef DLX_H
#define DLX_H

#include <array>
#include <climits>
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
        allocateNodes(puzzle);
        initDLX();

        for (int r = 0; r < N; ++r) {
            for (int c = 0; c < N; ++c) {
                if (puzzle[r][c] != 0) {
                    addKnownConstraint(r, c, puzzle[r][c] - 1);
                }
            }
        }
        for (int r = 0; r < N; ++r) {
            for (int c = 0; c < N; ++c) {
                if (puzzle[r][c] == 0) {
                    addUnknownConstraints(r, c);
                }
            }
        }

        answer.clear();
        if (search(0)) {
            auto solution = convertSolutionToGrid();
            cleanup();
            return solution;
        }

        cleanup();
        Board empty_board = {};
        return empty_board;
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
        header.L = header.R = header.U = header.D = &header;
        header.C = &header;
        header.size = 0;
        header.name = -1;

        ColumnNode *prev = &header;
        for (int i = 0; i < COL_NUM; i++) {
            columns[i].L = columns[i].R = columns[i].U = columns[i].D = &columns[i];
            columns[i].C = &columns[i];
            columns[i].size = 0;
            columns[i].name = i;

            prev->linkRight(&columns[i]);
            prev = &columns[i];
        }
    }

    static void addKnownConstraint(int row, int col, int num) {
        // box index 0-8
        int box = (row/SubGridSize) * SubGridSize + (col/SubGridSize);

        // indices for the 4 constraints
        int cell_constraint = row*N + col;
        int row_constraint = A + row*N + num;
        int col_constraint = A*2 + col*N + num;
        int box_constraint = A*3 + box*N + num;

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
            answer.push_back(r);

            for (DataNode* j = r->R; j != r; j = j->R) {
                j->C->cover();
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
        int min_size = INT_MAX;

        for (ColumnNode* col = (ColumnNode*)header.R; col != &header; col = (ColumnNode*)col->R) {
            if (col->size < min_size) {
                min_size = col->size;
                selected = col;

                if (min_size == 0) {
                    break;
                }
            }
        }

        return selected;
    }

    static Board convertSolutionToGrid() {
    Board solution = {};
    for (const auto& start_node : answer) {
        DataNode* cell_node = nullptr;
        DataNode* row_node = nullptr;

        DataNode* current_node = start_node;
        do {
            int name = current_node->C->name;

            if (name < A) {
                cell_node = current_node;
            }
            else if (name >= A && name < 2 * A) {
                row_node = current_node;
            }
            
            current_node = current_node->R;
        } while (current_node != start_node);

        if (cell_node == nullptr || row_node == nullptr) {
            Board empty_board = {};
            return empty_board;
        }

        int cell_constraint = cell_node->C->name;
        int row = cell_constraint / N;
        int col = cell_constraint % N;

        int num = (row_node->C->name - A) % N;

        solution[row][col] = num + 1;
    }
    return solution;
    }
    
    // memory management w/ area allocation
    // https://en.wikipedia.org/wiki/Region-based_memory_management
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
        if (pool_idx >= node_pool.size()) {
            return nullptr;
        }
        DataNode* node = node_pool[pool_idx++];
        node->L = node->R = node->U = node->D = node;
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
