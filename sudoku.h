#include <iostream>
#include <cinttypes>
#include <vector>

typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

#define INDEX(x, y, width) (x) + ((y) * (width))
#define INDEX_9x9(x, y) INDEX(x, y, 9)

enum group {
    ROW, COL, BLOCK, GROUP_MAX
};

struct Cell {	
    u32 digit = 0;
    u32 candidates[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
    bool is_candidate(u32 candidate);
    bool no_candidates();
    void fill_candidates();
    void clear_candidates();
};

struct Sudoku {
    static constexpr u32 width = 9;
    static constexpr u32 size = width * width;
    static constexpr u32 block_size = width / 3;
    static constexpr u32 block_count = width;
    Cell cells[width * width];
    void print();
    void print_candidates(u32 x, u32 y);
    void print_candidates(u32 index);
    // value = 0 to empty 
    void set_candidate_all_groups(u32 x, u32 y, u32 new_entry, u32 value);
    // value = 0 to empty 
    void set_candidate_group(u32* group_indeces, u32 new_entry, u32 value);
    void recompute_candidates(u32 x, u32 y, u32 deleted_digit);
    // checks if other cells in the same groups 
    // can allow the digit to be deleted as a candidate and sets it if so
    void empty_cell(u32 x, u32 y);
    void empty_cell(u32 index);
    void clear_cells();
    // removes the digit as candidate for all affected cells after setting!
    bool set_cell(u32 x, u32 y, u32 digit);
    bool set_cell(u32 index, u32 digit);
    Cell get_cell(u32 x, u32 y);

    void get_col_indeces(u32 col_index, u32* col);
    void get_row_indeces(u32 row_index, u32* row);
    void get_block_indeces(u32 x_block, u32 y_block, u32* block);
    void get_block_indeces_cell(u32 x, u32 y, u32* block);

    void get_col_digits(u32 col_index, u32* col);
    void get_row_digits(u32 row_index, u32* row);
    void get_block_digits(u32 x_block, u32 y_block, u32* block);
    std::vector<u32> find_cells_by_candidates(u32 num_candidates, std::string& info);
    std::vector<std::pair<u32, u32>> find_cells_one_candidate();
    bool no_candidates();
    bool is_solved();
    bool is_valid();
    bool is_group_valid(u32* group);
    void fill_upto(u32 num_cells);
    u32 get_lowest_candidates();

private:
    u32 lowest_candidates = width;
};

class Solver {
public:
    Solver();
    void bruteforce_step(Sudoku& sudoku);
    u32 backtrack(Sudoku& sudoku, u32 start = 0);
    // return number of sulutions
    u32 one_candidate(Sudoku& sudoku);
    Sudoku get_full_grid();
private:

};
