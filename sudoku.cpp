#include "sudoku.h"
#include <cassert>

Solver::Solver(Sudoku sudoku) {
    this->sudoku = sudoku;
}

void Solver::bruteforce_step() {
    // random
}

void Sudoku::empty_cell(u32 x, u32 y) {
    assert(x >= 0 && x < width);
    assert(y >= 0 && y < width);
    u32 index = INDEX_9x9(x, y); 
    Cell* cell = &cells[index];
    u32 prev = cell->digit;
    if (prev == 0) return;
    cell->digit = 0;
    u32 groups[GROUP_MAX][width] = {0};
    get_row_indeces(y, groups[ROW]);
    get_col_indeces(x, groups[COL]);
    get_block_indeces(x / block_size, y / block_size, groups[BLOCK]);
    for (int i = 0; i < GROUP_MAX; ++i) {
        for (int j = 0; j < width; ++j) {
            recompute_candidates(groups[i][j], prev);
        }
    }
}
// tries to set the cell value if input digit is a candidate
// updates candidates for all the affected cells (rows, cols, blocks)
bool Sudoku::set_cell(u32 x, u32 y, u32 digit) {
    assert(x >= 0 && x < width);
    assert(y >= 0 && y < width);
    assert(digit > 0 && digit <= 9);
    u32 index = INDEX_9x9(x, y);
    Cell* cell = &cells[index];
    if (!cell->is_candidate(digit)) return false;
    empty_cell(x, y);
    cell->digit = digit;
    // delete digit from the candidates of cells in the same groups 
    set_candidate_all_groups(x, y, digit, 0);
    return true;
}


void Sudoku::set_candidate_group(u32* group_indeces, u32 new_entry, u32 value) {
    assert(new_entry > 0 && new_entry <= 9);
    for (int i = 0; i < width; ++i) {
        cells[group_indeces[i]].candidates[new_entry - 1] = value;
    }
}

void Sudoku::set_candidate_all_groups(u32 x, u32 y, u32 new_entry, u32 value) {
    assert(new_entry > 0 && new_entry <= 9);
    // row, col, block
    u32 groups[GROUP_MAX][width] = {0};
    get_row_indeces(y, groups[ROW]);
    get_col_indeces(x, groups[COL]);
    get_block_indeces(x / block_size, y / block_size, groups[BLOCK]);
    for (int i = 0; i < GROUP_MAX; ++i) {
        for (int j = 0; j < width; ++j) {
            cells[groups[i][j]].candidates[new_entry - 1] = value;
        }
    }
}

void Sudoku::recompute_candidates(u32 index, u32 deleted_digit) {
    assert(deleted_digit > 0 && deleted_digit <= 9);
    u32 x = index % width;
    u32 y = index / width;
    u32 groups[GROUP_MAX][width] = {0};
    get_row_indeces(y, groups[ROW]);
    get_col_indeces(x, groups[COL]);
    get_block_indeces(x / block_size, y / block_size, groups[BLOCK]);
    for (int i = 0; i < GROUP_MAX; ++i) {
        for (int j = 0; j < width; ++j) {
            u32 digit = cells[groups[i][j]].digit;
            if (digit == deleted_digit) {
                cells[index].candidates[digit - 1] = 0;
                return;
            }
        }
    }
    cells[index].candidates[deleted_digit - 1] = 1;
}

Cell Sudoku::get_cell(u32 x, u32 y) {
    return cells[INDEX_9x9(x, y)];
}

void Sudoku::get_row_indeces(u32 row_index, u32* row) {
    for (int i = 0; i < width; ++i) {
        row[i] = INDEX_9x9(i, row_index);
    }
}

void Sudoku::get_col_indeces(u32 col_index, u32* col) {
    for (int i = 0; i < width; ++i) {
        col[i] = INDEX_9x9(col_index, i);
    }
}

void Sudoku::get_block_indeces_cell(u32 x, u32 y, u32* block) {
    int start_y = y / block_size;
    int start_x = x / block_size;
    int i = 0;
    for (int y = 0; y < block_size; ++y) {
        for (int x = 0; x < block_size ; ++x) {
            block[i++] = INDEX_9x9(x + start_x, y + start_y);
        }
    }
}

void Sudoku::get_block_indeces(u32 x_block, u32 y_block, u32* block) {
    int start_y = y_block * block_size;
    int start_x = x_block * block_size;
    int i = 0;
    for (int y = 0; y < block_size; ++y) {
        for (int x = 0; x < block_size ; ++x) {
            block[i++] = INDEX_9x9(x + start_x, y + start_y);
        }
    }
}

void Sudoku::get_row_digits(u32 row_index, u32* row) {
    for (int i = 0; i < width; ++i) {
        row[i] = cells[INDEX_9x9(i, row_index)].digit;
    }
}

void Sudoku::get_col_digits(u32 col_index, u32* col) {
    for (int i = 0; i < width; ++i) {
        col[i] = cells[INDEX_9x9(col_index, i)].digit;
    }
}

void Sudoku::get_block_digits(u32 x_block, u32 y_block, u32* block) {
    u32 indeces[width] = {0};
    get_block_indeces(x_block, y_block, indeces);
    int i = 0;
    for (u32 index : indeces) {
        block[i++] = cells[index].digit;
    }
}

bool Cell::is_candidate(u32 candidate) {
    assert(candidate > 0 && candidate <= 9);
    return candidates[candidate - 1] > 0;
}

void Cell::fill_candidates() {
    for (u32& c : candidates) {
        c = 1;
    }
}
void Cell::clear_candidates() {
    for (u32& c : candidates) {
        c = 0;
    }
}

bool Solver::no_candidates() {
    u32 sum = 0;
    for (const Cell& cell : sudoku.cells) {
        for (u32 candidate : cell.candidates) {
            sum += candidate; 
        }
    }
    return sum == 0;
}

bool Sover::is_solved {
    return is_valid() && no_candidates();
}

bool Solver::is_valid() {
    //lines
    for (int i = 0; i < sudoku.width; ++i) {
        u32 line[sudoku.width] = {0};
        sudoku.get_row_digits(i, line);
        if (!is_group_valid(line)) return false;
    }
    for (int i = 0; i < sudoku.width; ++i) {
        u32 line[sudoku.width] = {0};
        sudoku.get_col_digits(i, line);
        if (!is_group_valid(line)) return false;
    }
    //blocks
    for (int y_block = 0; y_block < sudoku.block_size; ++y_block) {
        for (int x_block = 0; x_block < sudoku.block_size; ++x_block) {
            u32 block[sudoku.width] = {0};
            sudoku.get_block_digits(x_block, y_block, block);
            if (!is_group_valid(block)) return false;
        }
    }
    return true;
}  

bool Solver::is_group_valid(u32* group) {
    for (int i = 0; i < sudoku.width; ++i) {
        int elem = group[i];
        for (int j = 0; j < sudoku.width; ++j) {
            if (i != j && elem == group[j] || elem == 0) {
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
    for (int i = 0; i < 9; ++i) {
        u32 candidate = cells[index].candidates[i];
        if (candidate) std::cout << i + 1 << ", ";
    }
    std::cout << "]\n";
}
