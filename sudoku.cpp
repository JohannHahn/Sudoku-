#include "sudoku.h"
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <string>

int random(int max) {
    return rand() % max;
}

template <typename T>
void remove_elem(std::vector<T>& data, T elem) {
    for (int i = 0; i < data.size(); ++i) {
        if (data[i] == elem) {
            data[i] = data[data.size() - 1];
            data.pop_back();
            return;
        }
    }
}


void Sudoku::clear_cells() {
    *this = Sudoku();
}

void Sudoku::empty_cell(u32 index) {
    empty_cell(index % width, index / width);
}
void Sudoku::empty_cell(u32 x, u32 y) {
    assert(x >= 0 && x < width);
    assert(y >= 0 && y < width);
    u32 index = INDEX_9x9(x, y); 
    Cell* cell = &cells[index];
    u32 prev = cell->digit;
    if (prev == 0) return;
    cell->digit = 0;

    u32 x_block_start = x / block_size;
    u32 y_block_start = y / block_size;
    u32 block_index = INDEX(x_block_start, y_block_start, 3);
    for (int y_block = 0; y_block < block_size; ++y_block) {
        for (int x_block = 0; x_block < block_size; ++x_block) {
            recompute_candidates(x_block_start + x_block, y_block_start + y_block, prev);  
        }
    }
    for (int i = 0; i < width; ++i) {
        recompute_candidates(i, y, prev);
        recompute_candidates(x, i, prev);
    }
}
// tries to set the cell value if input digit is a candidate
// updates candidates for all the affected cells (rows, cols, blocks)
bool Sudoku::set_cell(u32 index, u32 digit) {
    assert(index < width * width);
    assert(digit > 0 && digit <= 9);
    u32 x = index % width;
    u32 y = index / width;
    return set_cell(x, y, digit);
}
bool Sudoku::set_cell(u32 x, u32 y, u32 digit) {
    assert(x >= 0 && x < width);
    assert(y >= 0 && y < width);
    assert(digit > 0 && digit <= 9);
    u32 index = INDEX_9x9(x, y);
    Cell* cell = &cells[index];
    if (!cell->is_candidate(digit)) {
        //std::cout << "candidate " << digit << " is not a candidate for cell " << x << ", " << y << "\n";
        return false;
    }
    empty_cell(x, y);
    cell->digit = digit;
    // delete digit from the candidates of cells in the same groups 
    set_candidate_all_groups(x, y, digit, 0);
    return true;
}

void Sudoku::set_candidate_group(u32* group_indeces, u32 new_entry, u32 value) {
    assert(new_entry > 0 && new_entry <= 9);
    for (int i = 0; i < width; ++i) {
        Cell* cell = &cells[group_indeces[i]];
        cell->candidates[new_entry - 1] = value;
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

void Sudoku::recompute_candidates(u32 x, u32 y,  u32 deleted_digit) {
    assert(deleted_digit > 0 && deleted_digit <= 9);
    u32 index = INDEX_9x9(x, y);
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
    int start_x = x / block_size;
    int start_y = y / block_size;
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

bool Cell::no_candidates() {
    for (u32 candidate_bit : candidates) {
        if (candidate_bit) return false;
    }
    return true;
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

void Sudoku::fill_upto(u32 num_cells) {
    clear_cells();
    u32 set_cells = 0;
    while (set_cells < num_cells) {
        u32 i = random(size);
        if (cells[i].digit == 0) {
            std::vector<u32> digits = {1, 2, 3, 4, 5, 6, 7, 8, 9};
            while (digits.size()) {
                u32 index = random(digits.size() - 1);
                u32 candidate = digits[index];
                digits[index] = digits[digits.size() - 1];
                digits.pop_back();
                if (set_cell(i, candidate)) { 
                    set_cells++;
                    break;
                }
            }
        }
    }
}

std::vector<std::pair<u32, u32>> Sudoku::find_cells_one_candidate() {
    std::vector<std::pair<u32, u32>> result;
    for (u32 i = 0; i < width * width; ++i) { 
        if (cells[i].digit != 0) continue;
        u32 count = 0;
        u32 set_candidate = 0;
        u32 c_i = 0;
        for (u32 candidate : cells[i].candidates) {
            if (candidate > 0) {
                count++;
                set_candidate = c_i + 1;
            }
            c_i++;
        }
        if (count == 1) {
            result.push_back({i, set_candidate});
        }
    }
    return result;
}

std::vector<u32> Sudoku::find_cells_by_candidates(u32 num_candidates, std::string& info) {
    std::vector<u32> result = {};

    for (u32 i = 0; i < width * width; ++i) {
        if (cells[i].digit != 0) continue;
        u32 local_num_c = 0;
        for (u32 candidate : cells[i].candidates) {
            if (candidate > 0) local_num_c++;
        }
        if (local_num_c == num_candidates) {
            result.push_back(i);
            
        }
    }
    for (u32 i : result) {
        u32 nc = 0;
        info += "found index = "; info += std::to_string(i); info += " with "; info += std::to_string(num_candidates); info += " candidates\n";
        for (u32 candidate : cells[i].candidates) {
            info += std::to_string(candidate) + " ";
            if (candidate > 0) nc++;
        }
        info += "\n";
        if (nc != num_candidates) {
            info += "nc = " + std::to_string(nc) + ", num_candidates = " + std::to_string(num_candidates) + "\n";
        }
        assert(nc == num_candidates);
    }
    return result;
}


u32 Sudoku::get_lowest_candidates() {
    return lowest_candidates;
}

bool Sudoku::no_candidates() {
    u32 sum = 0;
    for (const Cell& cell : cells) {
        for (u32 candidate : cell.candidates) {
            sum += candidate; 
        }
    }
    return sum == 0;
}

bool Sudoku::is_solved() {
    return is_valid() && no_candidates();
}

bool Sudoku::is_valid() {
    //lines
    for (int i = 0; i < width; ++i) {
        u32 line[width] = {0};
        get_row_digits(i, line);
        if (!is_group_valid(line)) return false;
    }
    for (int i = 0; i < width; ++i) {
        u32 line[width] = {0};
        get_col_digits(i, line);
        if (!is_group_valid(line)) return false;
    }
    //blocks
    for (int y_block = 0; y_block < block_size; ++y_block) {
        for (int x_block = 0; x_block < block_size; ++x_block) {
            u32 block[width] = {0};
            get_block_digits(x_block, y_block, block);
            if (!is_group_valid(block)) return false;
        }
    }
    return true;
}  

bool Sudoku::is_group_valid(u32* group) {
    for (int i = 0; i < width; ++i) {
        int elem = group[i];
        for (int j = 0; j < width; ++j) {
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
    print_candidates(index);
}

void Sudoku::print_candidates(u32 index) {
    std::cout << "candidates for cell at index = " << index << "\n";
    u32 x = index % width;
    u32 y = index / width;
    std::cout << "x = " << x << ", y = " << y << "\n";
    std::cout << "[";
    int i = 0;
    for (u32 candidate : cells[index].candidates) {
        if (candidate) std::cout << i + 1 << ", ";
        i++;
    }
    std::cout << "]\n";
}

Solver::Solver() {
    srand(std::time(NULL));
}

void Solver::bruteforce_step(Sudoku& sudoku) {
    // random
}

u32 Solver::backtrack(Sudoku& sudoku, u32 start) {
    for (Cell& cell : sudoku.cells) {
        if (cell.no_candidates()) {
            return false;
        }
    }
    if (start == sudoku.size) { 
        for (Cell& cell : sudoku.cells) {
            if (cell.digit == 0) {
                return false;
            }
        }
        std::cout << "grid is completely filled\n";
        return true;
    }
    // find empty squares
    for (int i = start; i < sudoku.size; ++i) {
        if (sudoku.cells[i].digit == 0) {
            u32 candidate = 1;
            for (u32 candidate_bit : sudoku.cells[i].candidates) {
                if (candidate_bit) {
                    if (!sudoku.set_cell(i, candidate)) assert(0);
                    if (start == 0) std::cout << "set cell at i = " << i << "\n";
                    if (backtrack(sudoku, i + 1)) return true;
                    sudoku.empty_cell(i);
                }
                candidate++;
            }
        }
    }
    return false;
}

u32 Solver::one_candidate(Sudoku& sudoku) {
    if (sudoku.is_solved()) return true;
    std::vector<std::pair<u32, u32>> one_candidate_cells = sudoku.find_cells_one_candidate();
    if (one_candidate_cells.size() == 0) return false;

    u32 index = one_candidate_cells[0].first;
    u32 digit = one_candidate_cells[0].second;
    if (!sudoku.set_cell(index, digit)) {
        assert(0 && "could not set cell after calculating that you could set the cell :/\n");
    }
    return one_candidate(sudoku);
}
Sudoku Solver::get_full_grid() {
    Sudoku sudoku;
    u32 set_count = 0;
    u32 index = 0;
    u32 candidate = 0;
    while (set_count < sudoku.size) {
        if (sudoku.set_cell(index, candidate)) {
            set_count++;
        }
    }
    return sudoku;
}
