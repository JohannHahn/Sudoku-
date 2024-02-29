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
        int x_dif = cell_width;
        int y_dif = 0;
        for (int i = 0; i < 1; ++i) {
            DrawText(TextFormat("%d", (u32)cell.candidates.size()), candidate_pos.x, candidate_pos.y, font_size/3.f, DARKGRAY);
            if (i % 3 == 0) {
                x_dif = -y_dif; 
                y_dif = x_dif;
            }
            candidate_pos.x += x_dif;
            candidate_pos.y += y_dif;
        }
    }
}

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
        ClearBackground(RAYWHITE);
        Rectangle cell_boundary = {0, 0, cell_width, cell_width};
        int i = 0;
        for (const Cell& cell : sudoku_solver.sudoku.cells) {
            draw_cell(cell_boundary, cell);
            i++;
            cell_boundary.x += cell_width;
            if (cell_boundary.x > window_width) {
                cell_boundary.x = 0.f;
            }
            if (i % (sudoku_solver.sudoku.width) == 0) {
                cell_boundary.y += cell_width;
            }
        }
        EndDrawing();
    }
    return 0;
}
