#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <raylib.h>

#define WIDTH 80
#define HEIGHT 60
#define SIZE 10

#define BUTTON_WIDTH (800 / 7)
#define BUTTON_HEIGHT 50
#define BUTTON_TEXT_SIZE 20

typedef enum
{
    AIR,
    SAND,
    SOIL,
    WATER,
    WOOD,
    LEAVES,
    FIRE,
    SEEDS
} WorldMaterial;

const char *button_labels[] = {"SAND", "SOIL", "WATER", "FIRE", "WOOD", "LEAVES", "SEEDS"};
const int button_materials[] = {SAND, SOIL, WATER, FIRE, WOOD, LEAVES, SEEDS};
const Color button_colors[] = {GOLD, DARKBROWN, BLUE, RED, BROWN, DARKGREEN, GREEN};

int num_buttons = sizeof(button_labels) / sizeof(button_labels[0]);

WorldMaterial world[WIDTH][HEIGHT];
int selected_material = SAND;

void draw_world()
{
    for (size_t x = 0; x < WIDTH; x++)
    {
        for (size_t y = 0; y < HEIGHT; y++)
        {
            switch (world[x][y])
            {
            case AIR:
                DrawRectangle(x * SIZE, y * SIZE, SIZE, SIZE, (Color){20, 20, 20, 255});
                break;
            case SAND:
                DrawRectangle(x * SIZE, y * SIZE, SIZE, SIZE, GOLD);
                break;
            case SOIL:
                DrawRectangle(x * SIZE, y * SIZE, SIZE, SIZE, DARKBROWN);
                break;
            case WATER:
                DrawRectangle(x * SIZE, y * SIZE, SIZE, SIZE, BLUE);
                break;
            case WOOD:
                DrawRectangle(x * SIZE, y * SIZE, SIZE, SIZE, BROWN);
                break;
            case LEAVES:
                DrawRectangle(x * SIZE, y * SIZE, SIZE, SIZE, DARKGREEN);
                break;
            case FIRE:
                DrawRectangle(x * SIZE, y * SIZE, SIZE, SIZE, RED);
                break;
            case SEEDS:
                DrawRectangle(x * SIZE, y * SIZE, SIZE, SIZE, GREEN);
                break;
            }
        }
    }
}

bool is_surrounded_by(int x, int y, int material)
{
    for (int dx = -1; dx <= 1; dx++)
    {
        for (int dy = -1; dy <= 1; dy++)
        {
            if (dx == 0 && dy == 0)
            {
                continue;
            }

            int nx = x + dx;
            int ny = y + dy;

            if (nx < 0 || nx >= WIDTH || ny < 0 || ny >= HEIGHT)
            {
                continue;
            }

            if (world[nx][ny] == material)
            {
                return true;
            }
        }
    }

    return false;
}

void update_plants()
{
    for (size_t x = 0; x < WIDTH; x++)
    {
        for (int y = HEIGHT - 1; y >= 0; y--)
        {
            if (world[x][y] == SEEDS && is_surrounded_by(x, y, SOIL) && !is_surrounded_by(x, y, WATER))
            {
                world[x][y] = WOOD;
                if (y > 0 && world[x][y - 1] == AIR)
                {
                    world[x][y - 1] = SEEDS;
                }
            }

            else if (world[x][y] == SEEDS && world[x][y + 1] == WOOD && !is_surrounded_by(x, y, WATER))
            {
                int probability = GetRandomValue(0, 100);

                if (probability < 20)
                {
                    world[x][y] = WOOD;
                    world[x][y - 1] = SEEDS;
                }
                else
                {
                    world[x][y] = LEAVES;
                }
            }

            else if (world[x][y] == SEEDS)
            {
                int probability = GetRandomValue(0, 100);

                if (probability < 10)
                {
                    world[x][y] = AIR;
                }
            }
        }
    }
}

void update_flamable()
{
    for (size_t x = 0; x < WIDTH; x++)
    {
        for (int y = HEIGHT - 1; y >= 0; y--)
        {
            if (world[x][y] == FIRE)
            {
                if (is_surrounded_by(x, y, WATER) || !is_surrounded_by(x, y, AIR))
                {
                    world[x][y] = AIR;

                    break;
                }

                if (!is_surrounded_by(x, y, WOOD) && !is_surrounded_by(x, y, LEAVES))
                {
                    world[x][y] = AIR;

                    break;
                }

                for (int dx = -1; dx <= 1; dx++)
                {
                    for (int dy = -1; dy <= 1; dy++)
                    {
                        if (dx == 0 && dy == 0)
                        {
                            continue;
                        }

                        int nx = x + dx;
                        int ny = y + dy;

                        if (nx < 0 || nx >= WIDTH || ny < 0 || ny >= HEIGHT)
                        {
                            continue;
                        }

                        if (world[nx][ny] == WOOD)
                        {
                            world[nx][ny] = FIRE;
                        }

                        if (world[nx][ny] == LEAVES)
                        {
                            world[nx][ny] = FIRE;
                        }
                    }
                }
            }
        }
    }
}

void update_fluids()
{
    for (size_t x = 0; x < WIDTH; x++)
    {
        for (int y = HEIGHT - 1; y >= 0; y--)
        {
            if (world[x][y] == WATER)
            {
                if (y + 1 < HEIGHT && world[x][y + 1] == AIR)
                {
                    world[x][y + 1] = WATER;
                    world[x][y] = AIR;
                }
                else
                {
                    int direction = GetRandomValue(-1, 1);
                    if (x + direction >= 0 && x + direction < WIDTH && world[x + direction][y] == AIR)
                    {
                        world[x + direction][y] = WATER;
                        world[x][y] = AIR;
                    }
                }
            }
        }
    }
}

void update_falling()
{
    for (size_t x = 0; x < WIDTH; x++)
    {
        for (int y = HEIGHT - 1; y >= 0; y--)
        {
            if (world[x][y] == SAND || world[x][y] == SOIL || world[x][y] == SEEDS)
            {
                int material = world[x][y];
                if (y + 1 < HEIGHT && world[x][y + 1] == AIR)
                {
                    world[x][y + 1] = material;
                    world[x][y] = AIR;
                }
                else if (y + 1 < HEIGHT && world[x][y + 1] == WATER && GetRandomValue(0, 100) < 50)
                {
                    world[x][y + 1] = material;
                    world[x][y] = WATER;
                }
                else if (y + 1 < HEIGHT && x + 1 < WIDTH && world[x + 1][y + 1] == AIR)
                {
                    world[x + 1][y + 1] = material;
                    world[x][y] = AIR;
                }
                else if (y + 1 < HEIGHT && x - 1 >= 0 && world[x - 1][y + 1] == AIR)
                {
                    world[x - 1][y + 1] = material;
                    world[x][y] = AIR;
                }
            }
        }
    }
}

void clear_world()
{
    for (size_t x = 0; x < WIDTH; x++)
    {
        for (size_t y = 0; y < HEIGHT; y++)
        {
            world[x][y] = AIR;
        }
    }
}

void save_world()
{
    FILE *file = fopen("world.sav", "wb");
    if (file != NULL)
    {
        fwrite(world, sizeof(world), 1, file);
        fclose(file);
    }

    printf("Saved world\n");
}

void load_world()
{
    FILE *file = fopen("world.sav", "rb");
    if (file != NULL)
    {
        fread(world, sizeof(world), 1, file);
        fclose(file);
    }

    printf("Loaded world\n");
}

void update_buttons()
{
    for (int i = 0; i < num_buttons; i++)
    {
        DrawRectangle(i * BUTTON_WIDTH, 600, BUTTON_WIDTH, BUTTON_HEIGHT, button_colors[i]);

        int text_width = MeasureText(button_labels[i], BUTTON_TEXT_SIZE);
        int text_x = i * BUTTON_WIDTH + (BUTTON_WIDTH - text_width) / 2;
        DrawText(button_labels[i], text_x, 615, BUTTON_TEXT_SIZE, WHITE);
    }

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        Vector2 mouse = GetMousePosition();
        if (mouse.y > 600)
        {
            int button_index = mouse.x / BUTTON_WIDTH;
            if (button_index < num_buttons)
            {
                selected_material = button_materials[button_index];
            }
        }
    }

    if (IsKeyPressed(KEY_C))
    {
        clear_world();
    }

    if (IsKeyPressed(KEY_S))
    {
        save_world();
    }

    if (IsKeyPressed(KEY_L))
    {
        load_world();
    }
}

int main(void)
{
    InitWindow(800, 650, "SandBox");
    SetTargetFPS(150);

    clear_world();

    float fluid_update_time = 0.0f;
    float fluid_update_interval = 1.0 / 150.0f;

    float solid_update_time = 0.0f;
    float solid_update_interval = 1.0 / 60.0f;

    float flamable_update_time = 0.0f;
    float flamable_update_interval = 1.0 / 20.0f;

    float plants_update_time = 0.0f;
    float plants_update_interval = 1.0 / 5.0f;

    while (!WindowShouldClose())
    {
        float frame_time = GetFrameTime();
        fluid_update_time += frame_time;

        if (fluid_update_time >= fluid_update_interval)
        {
            update_fluids();
            fluid_update_time = 0.0f;
        }

        solid_update_time += frame_time;

        if (solid_update_time >= solid_update_interval)
        {
            update_falling();
            solid_update_time = 0.0f;
        }

        flamable_update_time += frame_time;

        if (flamable_update_time >= flamable_update_interval)
        {
            update_flamable();
            flamable_update_time = 0.0f;
        }

        plants_update_time += frame_time;

        if (plants_update_time >= plants_update_interval)
        {
            update_plants();
            plants_update_time = 0.0f;
        }

        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
        {
            Vector2 mouse = GetMousePosition();
            int x = mouse.x / SIZE;
            int y = mouse.y / SIZE;

            if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT)
            {
                world[x][y] = selected_material;
            }
        }

        if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON))
        {
            Vector2 mouse = GetMousePosition();
            int x = mouse.x / SIZE;
            int y = mouse.y / SIZE;

            if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT)
            {
                world[x][y] = AIR;
            }
        }

        BeginDrawing();

        draw_world();
        update_buttons();

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
