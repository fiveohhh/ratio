#ifndef GEAR_SELECTOR_H
#define GEAR_SELECTOR_H

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

// Max allowed cogs on the front or rear
#define MAX_COGS_F_OR_R 13

typedef enum
{
    SUCCESS             = 0, /** SUCCESS */
    ERR_INVALID_COG_LEN = 1, /** Specified cog len was greater than MAX_COGS_F_OR_R */
    ERR_COGS_NOT_SORTED = 2, /** Cogs were not sorted smallest to largest */
    ERR_INVALID_RATIO   = 3, /** Unable to find a gear combo that can achieve desired ratio */
    ERR_INVALID_GEAR    = 4, /** Gear combo doesn't exist in specified drivetrain_config_t */
    ERR_INVALID_COG     = 5, /** Cog cannot be `0` in size */
} errors_t;

typedef struct
{
    uint8_t front_cogs[MAX_COGS_F_OR_R]; /** Size of each front ring, sorted smallest to largest */
    uint8_t front_cogs_len;              /** Number of front rings */
    uint8_t rear_cogs[MAX_COGS_F_OR_R];  /** Size of cogs in cassette, sorted smallest to largest  */
    uint8_t rear_cogs_len;               /** Number of cogs in cassette */
} drivetrain_config_t;

// represents a physical gear combination
typedef struct gear_selector
{
    uint8_t front_cog_idx;
    uint8_t rear_cog_idx;
} gear_selection_t;

/**
 * @brief Retrieves the closest possible gear ratio without going over the desired
 * 
 * @param config Current drivetrain config
 * @param desired_ratio Desired gear ratio
 * @param[out] output_gears  If return is SUCCESS, output gears to achieve desired ratio
 * 
 * @return INVALID_RATIO if gear ratio unachievable, else SUCCESS
 */
errors_t gear_selector_get_closest_under(const drivetrain_config_t *config, float desired_ratio, gear_selection_t *output_gears);

/**
 * @brief Given the `current_gears`, Retrieves the next gear change to get closer to the desired ratio
 *          This prioritizes moving rear gears first.  We don't care about cross chaining
 * 
 * @param config Current drivetrain config
 * @param desired_ratio Desired gear ratio
 * @param current_gears Current gears that the drivetrain is in
 * @param[out] output_gears Gears that drivetrain needs to shift to
 * @return errors_t INVALID_GEAR if current_gears aren't in current drivetrain configuration
 */
errors_t gear_selector_get_next_gear(const drivetrain_config_t *config, float desired_ratio, const gear_selection_t *current_gears, gear_selection_t *output_gears);

#endif