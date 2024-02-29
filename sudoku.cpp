#include "sudoku.h"

void remove_elem(u8 elem, std::vector<u8>& vec) {
    for (int i = 0; i < vec.size(); ++i) {
        if (vec[i] == elem) {
            for (int j = i; j < vec.size() - 1; ++j) {
                vec[j] = vec[j + 1]; 
            }
            vec.pop_back();
            return;
        }
    }
}

Sudoku::Sudoku() {
    for (Cell& cell : cells) {
        cell.digit = 0;
        cell.candidates = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    }
}


Solver::Solver(Sudoku sudoku) {
    this->sudoku = sudoku;
}

void Solver::bruteforce_step() {
    // random
}
// tries to set the cell value if input digit is a candidate
// updates candidates for all the affected cells (rows, cols, blocks, diags)
bool Solver::set_cell(u32 x, u32 y, u8 digit) {
    u32 index = INDEX_9x9(x, y);
    Cell* cell = &sudoku.cells[index];
    if (!cell->is_candidate(digit)) return false;
    cell->digit = digit;
    update_candidates(x, y, digit);
    return true;
}


void Solver::update_group(u32* group_indeces, u8 new_entry) {
    std::cout << "updating group: \n";
    for (int i = 0; i < sudoku.width; ++i) {
        remove_elem(new_entry, sudoku.cells[group_indeces[i]].candidates);
        std::cout << "removed candidate " << (int)new_entry << " from cell at index " << group_indeces[i] << "\n";
    }
}

void Solver::update_candidates(u32 x, u32 y, u8 new_entry) {
    // row, col, block, diag1, diag2
    u32 groups[GROUP_MAX][sudoku.width] = {0};
    get_row_indeces(y, groups[ROW]);
    get_col_indeces(x, groups[COL]);
    get_block_indeces(x / sudoku.block_size, y / sudoku.block_size, groups[BLOCK]);
    bool diag_left, diag_right; 
    std::cout << "updating row, col, block:\n";
    for (int i = 0; i < DIAG_LEFT_RIGHT; ++i) {
        update_group(groups[i], new_entry);
    }
    if (is_on_diag(x, y, &diag_left, &diag_right)) {
        get_diags_indeces(groups[DIAG_LEFT_RIGHT], groups[DIAG_RIGHT_LEFT]);
        if (diag_left) {
            std::cout << "updating left right diag\n";
            update_group(groups[DIAG_LEFT_RIGHT], new_entry);
        }
        if (diag_right) {
            std::cout << "updating right left diag\n";
            update_group(groups[DIAG_RIGHT_LEFT], new_entry);
        }
    }
    
}

Cell Solver::get_cell(u32 x, u32 y) {
    return sudoku.cells[INDEX_9x9(x, y)];
}

void Solver::get_row_indeces(u32 row_index, u32* row) {
    for (int i = 0; i < sudoku.width; ++i) {
        row[i] = INDEX_9x9(i, row_index);
    }
}

void Solver::get_col_indeces(u32 col_index, u32* col) {
    for (int i = 0; i < sudoku.width; ++i) {
        col[i] = INDEX_9x9(col_index, i);
    }
}

void Solver::get_block_indeces_cell(u32 x, u32 y, u32* block) {
    int start_y = y / sudoku.block_size;
    int start_x = x / sudoku.block_size;
    int i = 0;
    for (int y = 0; y < sudoku.block_size; ++y) {
        for (int x = 0; x < sudoku.block_size ; ++x) {
            block[i++] = INDEX_9x9(x + start_x, y + start_y);
        }
    }
}

void Solver::get_block_indeces(u32 x_block, u32 y_block, u32* block) {
    int start_y = y_block * sudoku.block_size;
    int start_x = x_block * sudoku.block_size;
    int i = 0;
    for (int y = 0; y < sudoku.block_size; ++y) {
        for (int x = 0; x < sudoku.block_size ; ++x) {
            block[i++] = INDEX_9x9(x + start_x, y + start_y);
        }
    }
}

void Solver::get_diags_indeces(u32* left_right, u32* right_left) {
    for (int i = 0; i < 2; ++i) {
        int x = i == 0 ? 0 : sudoku.width - 1; 
        int y = 0;
        int x_dif = i == 0 ? 1 : -1;
        int line_index = 0;
        while (x >= 0 && x < sudoku.width && y < sudoku.width) {
            if (i == 0) left_right[line_index] = INDEX_9x9(x, y);
            else right_left[line_index] = INDEX_9x9(x, y);
            x += x_dif;
            y += 1;
            line_index++;
        }
    }
}

void Solver::get_row_digits(u32 row_index, u8* row) {
    for (int i = 0; i < sudoku.width; ++i) {
        row[i] = sudoku.cells[INDEX_9x9(i, row_index)].digit;
    }
}

void Solver::get_col_digits(u32 col_index, u8* col) {
    for (int i = 0; i < sudoku.width; ++i) {
        col[i] = sudoku.cells[INDEX_9x9(col_index, i)].digit;
    }
}

void Solver::get_block_digits(u32 x_block, u32 y_block, u8* block) {
    u32 indeces[sudoku.width] = {0};
    get_block_indeces(x_block, y_block, indeces);
    int i = 0;
    for (u32 index : indeces) {
        block[i++] = sudoku.cells[index].digit;
    }
}

void Solver::get_diags_digits(u8* left_right, u8* right_left) {
    for (int i = 0; i < 2; ++i) {
        int x = i == 0 ? 0 : sudoku.width - 1; 
        int y = 0;
        int x_dif = i == 0 ? 1 : -1;
        int line_index = 0;
        while (x >= 0 && x < sudoku.width && y < sudoku.width) {
            if (i == 0) left_right[line_index] = sudoku.cells[INDEX_9x9(x, y)].digit;
            else right_left[line_index] = sudoku.cells[INDEX_9x9(x, y)].digit;
            x += x_dif;
            y += 1;
            line_index++;
        }
    }
}

bool Cell::is_candidate(u8 candidate) {
    for (u8 digit : candidates) {
        if (digit == candidate) return true;
    }
    return false;
}

bool Solver::is_solved() {
    //lines
    for (int i = 0; i < sudoku.width; ++i) {
        u8 line[sudoku.width] = {0};
        get_row_digits(i, line);
        std::cout << "row = [";
        for (u32 d : line) {
            std::cout << d << ", ";
        }
        std::cout << "]\n";
        if (!is_group_valid(line)) return false;
    }
    for (int i = 0; i < sudoku.width; ++i) {
        u8 line[sudoku.width] = {0};
        get_col_digits(i, line);
        std::cout << "col = [";
        for (u32 d : line) {
            std::cout << d << ", ";
        }
        std::cout << "]\n";
        if (!is_group_valid(line)) return false;
    }
    //blocks
    for (int y_block = 0; y_block < sudoku.block_size; ++y_block) {
        for (int x_block = 0; x_block < sudoku.block_size; ++x_block) {
            u8 block[sudoku.width] = {0};
            get_block_digits(x_block, y_block, block);
            std::cout << "block = [";
            for (u32 d : block) {
                std::cout << d << ", ";
            }
            std::cout << "]\n";
            if (!is_group_valid(block)) return false;
        }
    }
    //diags
    u8 diag1[sudoku.width];
    u8 diag2[sudoku.width];
    get_diags_digits(diag1, diag2);
    std::cout << "diag1 = [";
    for (u32 d : diag1) {
        std::cout << d << ", ";
    }
    std::cout << "]\n";
    std::cout << "diag2 = [";
    for (u32 d : diag2) {
        std::cout << d << ", ";
    }
    std::cout << "]\n";
    if (!is_group_valid(diag1) || !is_group_valid(diag2)) return false;
    return true;
}  

bool Solver::is_group_valid(u8* group) {
    for (int i = 0; i < sudoku.width; ++i) {
        int elem = group[i];
        for (int j = i; j < sudoku.width; ++j) {
            if (elem == group[j] || elem == 0) {
                return false;
            }
        }
    }
    return true;
} 

bool Solver::is_on_diag(u32 x, u32 y, bool* left, bool* right) {
    *left = x == y;
    *right = x == (sudoku.width - 1 - y);
    return *left || *right;
}

void Sudoku::print() {
    std::cout << "sudoku puzzle " << width << "x" << width << "\n";
    for (int y = 0; y < width; ++y) {
        if (y > 0 && y % block_size == 0) std::cout << "\n";
        for (int x = 0; x < width; ++x) {
            u32 index = INDEX_9x9(x, y);
            if (x > 0 && x % block_size == 0) std::cout << " ";
            std::cout << (int)cells[index].digit << " ";
        }
        std::cout << "\n";
    }
}

void Sudoku::print_candidates(u32 x, u32 y) {
    u32 index = INDEX_9x9(x, y);
    std::cout << "candidates for cell at x = " << x << ", y = " << y << "\nindex = " << index << "\n";
    std::cout << "[";
    for (u32 c : cells[index].candidates) {
        std::cout << c << ", ";
    }
    std::cout << "]\n";
}

void Sudoku::print_candidates(u32 index) {
    std::cout << "candidates for cell at index = " << index << "\n";
    std::cout << "[";
    for (u32 c : cells[index].candidates) {
        std::cout << c << ", ";
    }
    std::cout << "]\n";
}
