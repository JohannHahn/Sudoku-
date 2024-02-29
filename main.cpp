#include "raylib/src/raylib.h"
#include "sudoku.h"

float window_width = 800;
float window_height = 600;
const char* window_title = "Sudoku";

Solver sudoku_solver = Solver(Sudoku());

int main() {
    InitWindow(window_width, window_height, window_title);
    for (int i = 0; i < 9; ++i) {
        sudoku_solver.set_cell(i, 0, i + 1);
    }
    sudoku_solver.sudoku.print();
    for (int i = 0; i < 9 * 9; ++i) {
        if (sudoku_solver.sudoku.cells[i].candidates.size() < 9) sudoku_solver.sudoku.print_candidates(i);
    }
    std::cout << (sudoku_solver.is_solved() ? "sudoku is solved\n" : "sudoku is not solved\n");
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(GREEN);
        EndDrawing();
    }
    return 0;
}
