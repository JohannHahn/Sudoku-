#include "sudoku.h"
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <ctime>

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

Solver::Solver() {
    srand(std::time(NULL));
}

void Solver::bruteforce_step(Sudoku& sudoku) {
    // random
}

bool Solver::one_candidate(Sudoku& sudoku) {
    if (sudoku.is_solved()) return true;
    //std::cout << "not solved yet: \n";
    //sudoku.print();

    std::vector<u32> one_candidate_cells = sudoku.find_cells_by_candidates(1);
    if (one_candidate_cells.size() == 0) {
        std::cout << "sudoku became unsolvable\n";
        return false;
    }
    else {
        std::cout << "\n";
        for (u32 index : one_candidate_cells) {
            assert(sudoku.cells[index].digit == 0);
            u32 candidate = 0;
            for (u32 c = 1; candidate != c && c <= 9; ++c) {
                if (sudoku.cells[index].is_candidate(c)) {
                    candidate = c;
                    break;
                }
            } 
            assert(candidate != 0);
            if (!sudoku.set_cell(index, candidate)) {
                std::cout << "could not set cell\n";
                u32 x = index % sudoku.width;
                u32 y = index / sudoku.width;
                std::cout << "x = " << x << ", y = " << y << "\n";
                std::cout << "index = " << index << ", candidate = " << candidate << "\n";
                std::cout << "cell at index = " << sudoku.cells[index].digit << "\n";
                std::cout << sudoku.cells[index].is_candidate(candidate) << "\n";
                assert (0);
            } 
            Sudoku s = sudoku;
            if (one_candidate(s)) {
                return true;
            }
            sudoku.empty_cell(index);
        } 
        return false;
    }
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

    u32 x_block = x / block_size;
    u32 y_block = y / block_size - 1;
    for (int i = 0; i < width; ++i) {
        recompute_candidates(i, y, prev);
        recompute_candidates(x, i, prev);
        x_block += i;
        if (i % block_size == 0) {
            x_block = 0;
            y_block++;
        }
        recompute_candidates(x_block, y_block, prev);
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
    // emtpy
    *this = Sudoku(); 
    std::vector<u32> indeces = {0};
    u32 size = width * width;
    for (int i = 0; i < size; ++i) {
        indeces.push_back(i);
    }
    int i = 0;
    while (i < num_cells) {
        int index = random(indeces.size() - 1);
        assert(index >= 0 && index < indeces.size());
        remove_elem<u32>(indeces, index);
        int digit = random(9) + 1;
        assert (digit > 0 && digit <= 9);
        set_cell(index, digit);
        i++;
    }
}

std::vector<u32> Sudoku::find_cells_by_candidates(u32 num_candidates) {
    std::vector<u32> result = {};

    for (u32 i = 0; i < width * width; ++i) {
        u32 local_num_c = 0;
        if (cells[i].digit != 0) continue;
        for (u32 j = 0; j < width; ++j) {
            if (cells[i].candidates[j] > 0) local_num_c++;
        }
        if (local_num_c == num_candidates) result.push_back(i);
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
    return sum == 0 && is_valid();
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
