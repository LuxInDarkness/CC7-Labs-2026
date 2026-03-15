#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#include "../ts_objects/ui_bridge.h"
#include "../program/admin.h"

// ─── Constants ───────────────────────────────────────────────────────────────

#define SCREEN_W   1000
#define SCREEN_H   700
#define MAX_CARS   50
#define MAX_SPACES 30

// ─── Types ────────────────────────────────────────────────────────────────────

typedef enum { SCREEN_INPUT, SCREEN_SIM } Screen;

typedef struct {
    bool  occupied;
    int   car_number; // which car is parked here, 0 = empty
    Color color;
} ParkingSpace;

typedef struct {
    int   number;
    Color color;
    bool  active;
} Car;

// ─── Car color palette ────────────────────────────────────────────────────────

static const Color CAR_PALETTE[] = {
    { 255,  80,  80, 255 },
    { 255, 160,  40, 255 },
    { 255, 220,  50, 255 },
    {  80, 220, 120, 255 },
    {  60, 180, 255, 255 },
    { 180,  80, 255, 255 },
    { 255, 100, 180, 255 },
    {  80, 230, 210, 255 },
};
#define PALETTE_SIZE 8

// ─── Globals ──────────────────────────────────────────────────────────────────

static Screen       current_screen  = SCREEN_INPUT;
static int          num_cars        = 10;
static int          num_spaces      = 5;
static ParkingSpace spaces[MAX_SPACES];
static Car          cars[MAX_CARS];
static int          queue[MAX_CARS]; // indices of cars still waiting
static int          queue_size      = 0;
static bool         sim_done        = false;

// Input screen
static int          editing_field   = 0; // 0 = cars, 1 = spaces
static char         input_cars[8]   = "10";
static char         input_spaces[8] = "5";

// Bridge — declared extern so admin.c owns the instance
extern UIEventQueue ui_queue;

// ── UI event processing ───────────────────────────────────────────────────────

static void process_ui_events(void) {
    UIEvent event;
    while (ui_queue_pop(&ui_queue, &event)) {
        int cn = event.car_number; // 1-based

        switch (event.type) {

            case UI_EVENT_ARRIVED:
                // Activate car and add to queue
                if (cn >= 1 && cn <= num_cars) {
                    cars[cn - 1].active = true;
                    queue[queue_size++] = cn;
                }
                break;

            case UI_EVENT_PARKED:
                // Remove from queue, put in first free space
                for (int i = 0; i < queue_size; i++) {
                    if (queue[i] == cn) {
                        for (int j = i; j < queue_size - 1; j++) queue[j] = queue[j + 1];
                        queue_size--;
                        break;
                    }
                }
                for (int i = 0; i < num_spaces; i++) {
                    if (!spaces[i].occupied) {
                        spaces[i].occupied   = true;
                        spaces[i].car_number = cn;
                        spaces[i].color      = cars[cn - 1].color;
                        break;
                    }
                }
                break;

            case UI_EVENT_LEFT:
                // Free the space this car is in
                for (int i = 0; i < num_spaces; i++) {
                    if (spaces[i].occupied && spaces[i].car_number == cn) {
                        spaces[i].occupied   = false;
                        spaces[i].car_number = 0;
                        break;
                    }
                }
                break;

            case UI_EVENT_DONE:
                sim_done = true;
                break;
        }
    }
}

// ── Simulation init ───────────────────────────────────────────────────────────

// Named thread entry for admin
static void *run_admin(void *arg) {
    int *args = (int *)arg;
    admin(args[0], args[1]);
    free(args);
    return NULL;
}

static void start_simulation(void) {
    for (int i = 0; i < num_spaces; i++) {
        spaces[i].occupied   = false;
        spaces[i].car_number = 0;
    }
    for (int i = 0; i < num_cars; i++) {
        cars[i].number = i + 1;
        cars[i].color  = CAR_PALETTE[i % PALETTE_SIZE];
        cars[i].active = false;
    }
    queue_size = 0;
    sim_done   = false;

    ui_queue_init(&ui_queue);

    int *args = malloc(2 * sizeof(int));
    args[0] = num_cars;
    args[1] = num_spaces;

    pthread_t sim_thread;
    pthread_create(&sim_thread, NULL, run_admin, args);
    pthread_detach(sim_thread); // fire and forget, UI polls the bridge
}

// ─── Input handling ───────────────────────────────────────────────────────────

static void handle_number_input(char *buf, int max_len) {
    int key = GetCharPressed();
    while (key > 0) {
        int len = (int)strlen(buf);
        if (key >= '0' && key <= '9' && len < max_len - 1) {
            buf[len]     = (char)key;
            buf[len + 1] = '\0';
        }
        key = GetCharPressed();
    }
    if (IsKeyPressed(KEY_BACKSPACE)) {
        int len = (int)strlen(buf);
        if (len > 0) buf[len - 1] = '\0';
    }
}

// ─── Input screen ─────────────────────────────────────────────────────────────

static void draw_input_screen(void) {
    ClearBackground((Color){ 15, 17, 23, 255 });

    DrawText("PARKING LOT SIMULATOR", 240, 80, 30, WHITE);
    DrawLine(240, 118, 660, 118, DARKGRAY);

    // Cars field
    DrawText("Number of Cars:", 240, 185, 20, LIGHTGRAY);
    Color cars_border   = (editing_field == 0) ? SKYBLUE : DARKGRAY;
    DrawRectangleLines(460, 180, 160, 36, cars_border);
    DrawText(input_cars, 470, 188, 20, WHITE);
    if (editing_field == 0 && ((int)(GetTime() * 2) % 2 == 0))
        DrawText("|", 470 + MeasureText(input_cars, 20), 188, 20, WHITE);

    // Spaces field
    DrawText("Parking Spaces:", 240, 255, 20, LIGHTGRAY);
    Color spaces_border = (editing_field == 1) ? SKYBLUE : DARKGRAY;
    DrawRectangleLines(460, 250, 160, 36, spaces_border);
    DrawText(input_spaces, 470, 258, 20, WHITE);
    if (editing_field == 1 && ((int)(GetTime() * 2) % 2 == 0))
        DrawText("|", 470 + MeasureText(input_spaces, 20), 258, 20, WHITE);

    DrawText("Click a field to select it, then type", 240, 305, 15, DARKGRAY);

    // Start button
    Rectangle btn     = { 340, 380, 220, 52 };
    bool      hovered = CheckCollisionPointRec(GetMousePosition(), btn);
    DrawRectangleRec(btn, hovered ? SKYBLUE : (Color){ 30, 100, 180, 255 });
    DrawText("START SIMULATION", 358, 395, 20, WHITE);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mouse = GetMousePosition();

        if (CheckCollisionPointRec(mouse, (Rectangle){ 460, 180, 160, 36 }))
            editing_field = 0;
        else if (CheckCollisionPointRec(mouse, (Rectangle){ 460, 250, 160, 36 }))
            editing_field = 1;

        if (CheckCollisionPointRec(mouse, btn)) {
            int c = atoi(input_cars);
            int s = atoi(input_spaces);
            num_cars   = (c > 0 && c <= MAX_CARS)  ? c : 10;
            num_spaces = (s > 0 && s <= MAX_SPACES) ? s : 5;
            start_simulation();
            current_screen = SCREEN_SIM;
        }
    }

    if (editing_field == 0) handle_number_input(input_cars,   sizeof(input_cars));
    else                    handle_number_input(input_spaces,  sizeof(input_spaces));
}

// ─── Simulation screen ────────────────────────────────────────────────────────

static void draw_sim_screen(void) {
    ClearBackground((Color){ 15, 17, 23, 255 });

    process_ui_events(); // drain the bridge each frame

    // Parking spaces
    DrawText("PARKING SPACES", 40, 18, 18, LIGHTGRAY);
    int space_w = 90, space_h = 100, space_margin = 14;
    int per_row = (SCREEN_W - 80) / (space_w + space_margin);
    if (per_row < 1) per_row = 1;

    for (int i = 0; i < num_spaces; i++) {
        int x = 40 + (i % per_row) * (space_w + space_margin);
        int y = 45 + (i / per_row) * (space_h + space_margin);

        if (spaces[i].occupied) {
            DrawRectangle(x, y, space_w, space_h, spaces[i].color);
            DrawRectangleLines(x, y, space_w, space_h, WHITE);
            char label[8];
            snprintf(label, sizeof(label), "#%d", spaces[i].car_number);
            int tw = MeasureText(label, 22);
            DrawText(label, x + (space_w - tw) / 2, y + space_h / 2 - 11, 22, BLACK);
        } else {
            DrawRectangle(x, y, space_w, space_h, (Color){ 30, 35, 50, 255 });
            DrawRectangleLines(x, y, space_w, space_h, (Color){ 55, 65, 90, 255 });
            int tw = MeasureText("EMPTY", 15);
            DrawText("EMPTY", x + (space_w - tw) / 2, y + space_h / 2 - 8, 15, DARKGRAY);
        }
        char idx[4]; snprintf(idx, sizeof(idx), "%d", i + 1);
        DrawText(idx, x + 5, y + 4, 13, spaces[i].occupied ? BLACK : GRAY);
    }

    // Divider + queue
    int div_y = 500;
    DrawLine(40, div_y, SCREEN_W - 40, div_y, DARKGRAY);
    DrawText("WAITING QUEUE", 40, div_y + 10, 18, LIGHTGRAY);

    int car_w = 58, car_h = 68, car_margin = 10;
    for (int i = 0; i < queue_size; i++) {
        int cn = queue[i]; // 1-based car number
        int x  = 40 + i * (car_w + car_margin);
        int y  = div_y + 38;
        DrawRectangle(x, y, car_w, car_h, cars[cn - 1].color);
        DrawRectangleLines(x, y, car_w, car_h, WHITE);
        char label[8]; snprintf(label, sizeof(label), "#%d", cn);
        int tw = MeasureText(label, 18);
        DrawText(label, x + (car_w - tw) / 2, y + car_h / 2 - 9, 18, BLACK);
    }

    if (sim_done) DrawText("Simulation complete!", 40, div_y + 48, 20, GREEN);

    // Stats
    int parked = 0;
    for (int i = 0; i < num_spaces; i++) if (spaces[i].occupied) parked++;
    char stats[128];
    snprintf(stats, sizeof(stats), "Parked: %d    Waiting: %d    Spaces: %d",
             parked, queue_size, num_spaces);
    DrawText(stats, 40, SCREEN_H - 32, 17, LIGHTGRAY);

    // Back button
    Rectangle btn     = { SCREEN_W - 130, SCREEN_H - 42, 110, 30 };
    bool      hovered = CheckCollisionPointRec(GetMousePosition(), btn);
    DrawRectangleRec(btn, hovered ? DARKGRAY : (Color){ 30, 35, 50, 255 });
    DrawRectangleLinesEx(btn, 1, DARKGRAY);
    DrawText("< Back", SCREEN_W - 118, SCREEN_H - 35, 17, LIGHTGRAY);
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && hovered)
        current_screen = SCREEN_INPUT;
}

// ─── Main ─────────────────────────────────────────────────────────────────────

int main(void) {
    InitWindow(SCREEN_W, SCREEN_H, "Parking Lot Simulator");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        if (current_screen == SCREEN_INPUT) draw_input_screen();
        else                                draw_sim_screen();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
