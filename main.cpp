#include "raylib/src/raylib.h"
#include "sudoku.h"
#include <map>

float window_width = 900;
float window_height = 900;
const char* window_title = "Sudoku";
double start = 0.f;
double delta = 0.f;

Solver sudoku_solver = Solver();
Sudoku sudoku;
float cell_width = window_width / sudoku.width;
Vector2 selected_cell = {0, 0};

bool not_found = true;


typedef std::vector<std::pair<u32, u32>> Points;

void time_begin() {
    start = GetTime();
}
void time_end() {
    delta = GetTime() - start;
}
bool is_selected(u32 x, u32 y) {
    return x == selected_cell.x && y == selected_cell.y;
}

void draw_cell(Rectangle boundary, const Cell& cell) {
    Color border_col = BLACK;
    float border_thicc = 1.f;
    float font_size = boundary.height * 0.7f;
    const char digit = '0' + cell.digit;
    Vector2 text_size = MeasureTextEx(GetFontDefault(), &digit, font_size, 0.f);
    Vector2 digit_pos = {.x = boundary.x + boundary.width / 2.f - text_size.x / 2.f,
                        .y = boundary.y + boundary.height / 2.f - text_size.y / 2.f};

    if (is_selected(boundary.x / cell_width, boundary.y / cell_width)) {
        border_col = RED;
        border_thicc = 2.f;
        DrawRectangleRec(boundary, ColorAlpha(GREEN, 0.5f));
    }
    DrawRectangleLinesEx(boundary, border_thicc, border_col);
    if (cell.digit > 0) DrawText(TextFormat("%d", (u32)cell.digit), digit_pos.x, digit_pos.y, font_size, BLACK);

    int candidate_width = cell_width / 3.f;
    int c_x = boundary.x + candidate_width / 3.f; 
    int c_y = boundary.y;
    for (int y = 0; y < 3; ++y) {
        for (int x = 0; x < 3; ++x) {
            int index = INDEX(x, y, 3);
            if (cell.candidates[index]) {
                DrawText(TextFormat("%d", index + 1), 
                         c_x + x * candidate_width, c_y + y * candidate_width, 
                         font_size/3.f, DARKGRAY);
            }
        }
    }
}

void controls() {
    if (IsKeyReleased(KEY_O)) {
        sudoku.clear_cells();
    }
    if (IsKeyReleased(KEY_SPACE)) {
        Sudoku s = sudoku;
        u32 solutions = sudoku_solver.one_candidate(s);
        std::cout << "sudoku has " << solutions << " solutions\n";
    }
    if (IsKeyReleased(KEY_F) && not_found) {
        std::cout << "fast ";
        time_begin();
        sudoku = Sudoku();
        sudoku_solver.backtrack_fast(sudoku);
        time_end();
    }
    if (IsKeyReleased(KEY_S) && not_found) {
        std::cout << "slow ";
        time_begin();
        sudoku = Sudoku();
        sudoku_solver.backtrack_slow(sudoku);
        time_end();
    }
    Vector2 mouse_pos = GetMousePosition();
    if (CheckCollisionPointRec(mouse_pos, {0, 0, window_width, window_height}) && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        selected_cell.x = (int)(mouse_pos.x / cell_width);
        selected_cell.y = (int)(mouse_pos.y / cell_width); 
    } 
    for (int i = 1; i <= 9; ++i) {
        if (IsKeyReleased(KEY_ZERO + i) || IsKeyReleased(KEY_KP_0 + i)) {
            sudoku.set_cell(selected_cell.x, selected_cell.y, i);
        }
    }
    if (IsKeyReleased(KEY_ZERO) || IsKeyReleased(KEY_DELETE) || IsKeyReleased(KEY_KP_0)) {
        sudoku.empty_cell(selected_cell.x, selected_cell.y);
    }
    if (IsKeyPressed(KEY_LEFT)) {
        selected_cell.x -= 1.f;
    }
    if (IsKeyPressed(KEY_RIGHT)) {
        selected_cell.x += 1.f;
    }
    if (IsKeyPressed(KEY_UP)) {
        selected_cell.y -= 1.f;
    }
    if (IsKeyPressed(KEY_DOWN)) {
        selected_cell.y += 1.f;
    }
    if (selected_cell.x > 8.f) selected_cell.x = 0.f;
    if (selected_cell.x < 0.f) selected_cell.x = 8.f;
    if (selected_cell.y > 8.f) selected_cell.y = 0.f;
    if (selected_cell.y < 0.f) selected_cell.y = 8.f;
}

int main() {
    SetRandomSeed(GetTime());
    InitWindow(window_width, window_height, window_title);
    
    
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        controls();
        for (int y = 0; y < window_height; y += cell_width) {
            for (int x = 0; x < window_width; x += cell_width) {
                Rectangle cell_bounds = {(float)x, (float)y, cell_width, cell_width};
                draw_cell(cell_bounds, sudoku.cells[INDEX_9x9(x / (int)cell_width, y / (int)cell_width)]);
            }
        }
        EndDrawing();
    }
    return 0;
}
