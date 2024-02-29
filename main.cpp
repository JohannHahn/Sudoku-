#include "raylib/src/raylib.h"
#include "sudoku.h"

float window_width = 900;
float window_height = 900;
const char* window_title = "Sudoku";

Solver sudoku_solver = Solver(Sudoku());
float cell_width = window_width / sudoku_solver.sudoku.width;

void draw_cell(Rectangle boundary, const Cell& cell) {
    DrawRectangleLinesEx(boundary, 1.f, BLACK);
    float font_size = boundary.height * 0.9f;
    Vector2 digit_pos = {.x = boundary.x + boundary.width / 2.f - font_size / 4.f,
                        .y = boundary.y + boundary.height / 2.f - font_size / 2.f};
    if (cell.digit > 0) {
        DrawText(TextFormat("%d", (u32)cell.digit), digit_pos.x, digit_pos.y, font_size, BLACK);
    }
    else {
        Vector2 candidate_pos = {boundary.x, boundary.y};
        int candidate_width = cell_width / 3.f;
        for (int i = 0; i < 9; ++i) {
            if (i > 0 && i % 3 == 0) {
                candidate_pos.y += candidate_width;
                candidate_pos.x = boundary.x;
            }
            if (cell.candidates[i]) {
                DrawText(TextFormat("%d", i + 1), candidate_pos.x, candidate_pos.y, font_size/3.f, DARKGRAY);
            }
            candidate_pos.x += candidate_width;
        }
    }
}

int main() {
    InitWindow(window_width, window_height, window_title);
    for (int i = 0; i < 9*9; ++i) {
        sudoku_solver.set_cell(i, 0, 1);
    }
    for (int i = 9; i < 18; ++i) {
        sudoku_solver.sudoku.print_candidates(i);
    }
    std::cout << (sudoku_solver.is_solved() ? "sudoku is solved\n" : "sudoku is not solved\n");
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        Rectangle cell_boundary = {0, 0, cell_width, cell_width};
        int i = 0;
        for (const Cell& cell : sudoku_solver.sudoku.cells) {
            draw_cell(cell_boundary, cell);
            cell_boundary.x += cell_width;
            if (i > 0 && i % sudoku_solver.sudoku.width == 0) {
                cell_boundary.x = 0;
                cell_boundary.y += cell_width;
            }
            i++;
        }
        EndDrawing();
    }
    return 0;
}
