#include "gear_selector.h"
#include <stdio.h>

/**
 * @brief Display closest gear ratio without going over
 *
 * @param config Drivetrain config to display gears from
 * @param desired_ratio Desired gear ratio
 */
void display_closest_gear_combo(const drivetrain_config_t *config,
                                float desired_ratio)
{
    gear_selection_t gears;
    errors_t err =
        gear_selector_get_closest_under(config, desired_ratio, &gears);

    if (err)
    {
        printf("ERROR getting ratio: %d\n", err);
    }
    else
    {
        printf("f: %d r:%d ratio:%.3f\n",
               config->front_cogs[gears.front_cog_idx],
               config->rear_cogs[gears.rear_cog_idx],
               config->front_cogs[gears.front_cog_idx] /
                   (float)config->rear_cogs[gears.rear_cog_idx]);
    }
}

/**
 * @brief Display steps to get from one gear ratio to another (without going
 * over desired)
 *
 * @param config Drivetrain config to display gears from
 * @param desired_ratio Ratio to get close to without going over
 * @param current_gears starting gear ratio -- This will be over written with
 * final gear selected
 */
void display_gear_steps(const drivetrain_config_t *config, float desired_ratio,
                        gear_selection_t *current_gears)
{
    errors_t err;
    gear_selection_t out_gears;

    printf("1 - f: %d r:%d ratio:%.3f\n",
           config->front_cogs[current_gears->front_cog_idx],
           config->rear_cogs[current_gears->rear_cog_idx],
           config->front_cogs[current_gears->front_cog_idx] /
               (float)config->rear_cogs[current_gears->rear_cog_idx]);

    // cap it at max possible number of gear combos so we don't get stuck in
    // here if something goes wrong
    for (int i = 0; i < config->front_cogs_len * config->rear_cogs_len; i++)
    {
        err = gear_selector_get_next_gear(config, desired_ratio, current_gears,
                                          &out_gears);
        if (err)
        {
            printf("ERROR getting next gear: %d\n", err);
            return;
        }
        // see if the next gear is the same as our current gear
        if (memcmp(current_gears, &out_gears, sizeof(out_gears)) == 0)
        {
            // If so, no more gear changes needed
            break;
        }
        printf("%d - f: %d r:%d ratio:%.3f\n", i + 2,
               config->front_cogs[out_gears.front_cog_idx],
               config->rear_cogs[out_gears.rear_cog_idx],
               config->front_cogs[out_gears.front_cog_idx] /
                   (float)config->rear_cogs[out_gears.rear_cog_idx]);

        // make new gear our current gear and find next gear
        memcpy(current_gears, &out_gears, sizeof(out_gears));
    }
}

int main()
{
    // setup our drivetrain
    const drivetrain_config_t config = {.front_cogs     = {30, 38, 44},
                                        .front_cogs_len = 3,
                                        .rear_cogs      = {16, 19, 23, 28},
                                        .rear_cogs_len  = 4};

    printf("****Display Closest Gear****\n\n");
    // print closest possible ratio, without going over, for various ratios
    printf("Desired ratio: 1.6\n");
    display_closest_gear_combo(&config, 1.6);
    printf("\nDesired ratio: 5\n");
    display_closest_gear_combo(&config, 5);
    printf("\nDesired ratio: 1.1\n");
    display_closest_gear_combo(&config, 1.1);
    printf("\nDesired ratio: 2.0\n");
    display_closest_gear_combo(&config, 2.0);
    printf("\nDesired ratio: 1.0\n");
    display_closest_gear_combo(&config, 1.0);

    // Print steps to get to closest ratio without going over, from a specified
    // starting gear combo

    printf("\n\n****Display Steps****\n");

    gear_selection_t current_gears = {.front_cog_idx = 1, .rear_cog_idx = 3};
    printf("\nSteps to get to 1.6:\n");
    display_gear_steps(&config, 1.6, &current_gears);

    current_gears.front_cog_idx = 0;
    current_gears.rear_cog_idx  = 3;
    printf("\nSteps to get to 5:\n");
    display_gear_steps(&config, 5, &current_gears);

    current_gears.front_cog_idx = 1;
    current_gears.rear_cog_idx  = 3;
    printf("\nSteps to get to 2.1:\n");
    display_gear_steps(&config, 2.1, &current_gears);

    current_gears.front_cog_idx = 1;
    current_gears.rear_cog_idx  = 3;
    printf("\nSteps to get to 1.1:\n");
    display_gear_steps(&config, 1.1, &current_gears);

    // This should error out as we can't get a ratio this low
    current_gears.front_cog_idx = 1;
    current_gears.rear_cog_idx  = 3;
    printf("\nSteps to get to 1:\n");
    display_gear_steps(&config, 1, &current_gears);
}