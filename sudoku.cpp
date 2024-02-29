#include "sudoku.h"
#include <cassert>

Solver::Solver(Sudoku sudoku) {
    this->sudoku = sudoku;
}

void Solver::bruteforce_step() {
    // random
}
// tries to set the cell value if input digit is a candidate
// updates candidates for all the affected cells (rows, cols, blocks)
bool Solver::set_cell(u32 x, u32 y, u32 digit) {
    u32 index = INDEX_9x9(x, y);
    Cell* cell = &sudoku.cells[index];
    if (!cell->is_candidate(digit)) return false;
    std::cout << digit << " is candidate!\n";
    cell->digit = digit;
    update_candidates(x, y, digit);
    return true;
}


void Solver::update_group(u32* group_indeces, u32 new_entry) {
    std::cout << "updating group with new entry = " << new_entry << "\n";
    assert(new_entry > 0 && new_entry <= 9);
    for (int i = 0; i < sudoku.width; ++i) {
        if (new_entry == 8) std::cout << "removing 8\n";
        sudoku.cells[group_indeces[i]].candidates[new_entry - 1] = 0;
    }
}

void Solver::update_candidates(u32 x, u32 y, u32 new_entry) {
    // row, col, block
    u32 groups[GROUP_MAX][sudoku.width] = {0};
    get_row_indeces(y, groups[ROW]);
    get_col_indeces(x, groups[COL]);
    get_block_indeces(x / sudoku.block_size, y / sudoku.block_size, groups[BLOCK]);
    for (int i = 0; i < GROUP_MAX; ++i) {
        update_group(groups[i], new_entry);
    }
    std::cout << "updating row, col, block:\n";
    
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

void Solver::get_row_digits(u32 row_index, u32* row) {
    for (int i = 0; i < sudoku.width; ++i) {
        row[i] = sudoku.cells[INDEX_9x9(i, row_index)].digit;
    }
}

void Solver::get_col_digits(u32 col_index, u32* col) {
    for (int i = 0; i < sudoku.width; ++i) {
        col[i] = sudoku.cells[INDEX_9x9(col_index, i)].digit;
    }
}

void Solver::get_block_digits(u32 x_block, u32 y_block, u32* block) {
    u32 indeces[sudoku.width] = {0};
    get_block_indeces(x_block, y_block, indeces);
    int i = 0;
    for (u32 index : indeces) {
        block[i++] = sudoku.cells[index].digit;
    }
}

bool Cell::is_candidate(u32 candidate) {
    assert(candidate > 0 && candidate <= 9);
    return candidates[candidate - 1] > 0;
}

bool Solver::is_solved() {
    //lines
    for (int i = 0; i < sudoku.width; ++i) {
        u32 line[sudoku.width] = {0};
        get_row_digits(i, line);
        std::cout << "row = [";
        for (u32 d : line) {
            std::cout << d << ", ";
        }
        std::cout << "]\n";
        if (!is_group_valid(line)) return false;
    }
    for (int i = 0; i < sudoku.width; ++i) {
        u32 line[sudoku.width] = {0};
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
            u32 block[sudoku.width] = {0};
            get_block_digits(x_block, y_block, block);
            std::cout << "block = [";
            for (u32 d : block) {
                std::cout << d << ", ";
            }
            std::cout << "]\n";
            if (!is_group_valid(block)) return false;
        }
    }
    return true;
}  

bool Solver::is_group_valid(u32* group) {
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
    std::cout << "x = " << x << ", y = " << y << "\n";
    print_candidates(index);
}

void Sudoku::print_candidates(u32 index) {
    std::cout << "candidates for cell at index = " << index << "\n";
    std::cout << "[";
    for (int i = 0; i < 8; ++i) {
        u32 candidate = cells[index].candidates[i];
        if (candidate) std::cout << i + 1 << ", ";
    }
    std::cout << "]\n";
}
