#include "gear_selector.h"
#include "stdio.h"

// Used as placeholder for a ratio that is not possible
#define INVALID_RATIO (-1)

/**
 * @brief Validate a drivetrain_config_t object.
 *
 * @param config drivetrain_config_t to validate
 */
static errors_t validate_config(const drivetrain_config_t *config)
{
    // make sure len is less than MAX_COGS_F_OR_R
    if (config->front_cogs_len > MAX_COGS_F_OR_R ||
        config->rear_cogs_len > MAX_COGS_F_OR_R)
    {
        return ERR_INVALID_COG_LEN;
    }

    // make sure cogs are sorted from smallest to largest
    for (int i = 1; i < config->front_cogs_len; i++)
    {
        if (config->front_cogs[i] < config->front_cogs[i - 1])
        {
            return ERR_COGS_NOT_SORTED;
        }
    }
    // make sure cogs sorted from smallest to largest
    for (int i = 1; i < config->rear_cogs_len; i++)
    {
        if (config->rear_cogs[i] < config->rear_cogs[i - 1])
        {
            return ERR_COGS_NOT_SORTED;
        }
    }

    return SUCCESS;
}

errors_t gear_selector_get_closest_under(const drivetrain_config_t *config,
                                         float desired_ratio,
                                         gear_selection_t *output_gears)
{
    errors_t ret_val = validate_config(config);
    if (ret_val != SUCCESS)
    {
        return ret_val;
    }

    // we can't do anything if the desired ratio is lower than our lowest
    // possible ratio
    if (desired_ratio < (config->front_cogs[0] /
                         (float)config->rear_cogs[config->rear_cogs_len - 1]))
    {
        return ERR_INVALID_RATIO;
    }

    // used to track the closest gear combo that is under our desired ratio
    float closest_ratio = INVALID_RATIO;

    // loop over our front and rear cogs and find the gear combo that is closest
    for (int i = 0; i < config->front_cogs_len; i++)
    {
        for (int j = 0; j < config->rear_cogs_len; j++)
        {
            // get ratio
            float ratio = config->front_cogs[i] / (float)config->rear_cogs[j];
            // check if it's closer than our previous
            if ((desired_ratio - ratio) < (desired_ratio - closest_ratio))
            {
                // make sure it's not over our desired ratio
                if (ratio < desired_ratio)
                {
                    // we're closer to our desired ratio without going over,
                    // lets save it
                    closest_ratio               = ratio;
                    output_gears->front_cog_idx = i;
                    output_gears->rear_cog_idx  = j;
                }
            }
        }
    }

    return SUCCESS;
}

errors_t gear_selector_get_next_gear(const drivetrain_config_t *config,
                                     float desired_ratio,
                                     const gear_selection_t *current_gears,
                                     gear_selection_t *output_gears)
{
    // check if there is a gear combo closer than we currently are
    errors_t ret_val =
        gear_selector_get_closest_under(config, desired_ratio, output_gears);
    if (ret_val)
    {
        return ret_val;
    }

    if (desired_ratio <= 0)
    {
        return ERR_INVALID_RATIO;
    }

    // check the specified gear combo is in our drivetrain config
    if (current_gears->front_cog_idx >= config->front_cogs_len ||
        current_gears->rear_cog_idx >= config->rear_cogs_len)
    {
        return ERR_INVALID_GEAR;
    }

    // if current gears differ from our desired gear
    if (memcmp(output_gears, current_gears, sizeof(*output_gears)))
    {
        // let's shift cogs in the right direction
        // shift front first
        if (output_gears->front_cog_idx != current_gears->front_cog_idx)
        {
            // copy rear over
            output_gears->rear_cog_idx = current_gears->rear_cog_idx;
            // move front appropriate way
            output_gears->front_cog_idx =
                output_gears->front_cog_idx < current_gears->front_cog_idx
                    ? current_gears->front_cog_idx - 1
                    : current_gears->front_cog_idx + 1;
        }
        else
        {
            // copy front over
            output_gears->front_cog_idx = current_gears->front_cog_idx;
            // Move rear appropriate way
            output_gears->rear_cog_idx =
                output_gears->rear_cog_idx < current_gears->rear_cog_idx
                    ? current_gears->rear_cog_idx - 1
                    : current_gears->rear_cog_idx + 1;
        }
    }

    return SUCCESS;
}