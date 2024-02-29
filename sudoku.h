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
    //std::vector<u32> candidates;
    bool is_candidate(u32 candidate);
};

struct Sudoku {
    static constexpr u32 width = 9;
    static constexpr u32 block_size = width / 3;
    static constexpr u32 block_count = width;
    Cell cells[width * width];
    void print();
    void print_candidates(u32 x, u32 y);
    void print_candidates(u32 index);
private:
};

class Solver {
public:
    Solver(Sudoku sudoku);
    Sudoku sudoku;
    bool is_solved();
    bool is_group_valid(u32* group);
    // changes the candidates for all affected cells after setting!
    bool set_cell(u32 x, u32 y, u32 digit);
    Cell get_cell(u32 x, u32 y);

    void get_col_indeces(u32 col_index, u32* col);
    void get_row_indeces(u32 row_index, u32* row);
    void get_block_indeces(u32 x_block, u32 y_block, u32* block);
    void get_block_indeces_cell(u32 x, u32 y, u32* block);

    void get_col_digits(u32 col_index, u32* col);
    void get_row_digits(u32 row_index, u32* row);
    void get_block_digits(u32 x_block, u32 y_block, u32* block);

    void bruteforce_step();

private:
    void update_candidates(u32 x, u32 y, u32 new_entry);
    void update_group(u32* group_indeces, u32 new_entry);
};
