#include <stdio.h>
#include <stddef.h>

#define DEBUG_EACH_MONTH 0

struct scenario_cost_t {
    float dev_cost;
    float increase_cost_multi;
    float increase_cost_ratio;
    float building_cost;
    float tech_cost;
};

struct dev_effect_t {
    float ducat_per_dev;
};

struct tech_effect_t {
    float increase_tax_modifier;
};

struct scenario_t {
    size_t current_province_dev;
    float national_ducat_monthly;
    size_t admin_point_monthly;
    struct scenario_cost_t cost;
    struct dev_effect_t dev_effect;
    struct tech_effect_t tech_effect;
    size_t max_month;
};

static float cal_total_ducat_without_tech(struct scenario_t *scene);

static float cal_total_ducat_wait_for_tech_start_from_month(
    struct scenario_t *scene, size_t month_start_wait_for_tech);



int main() {
    struct scenario_t scenario = {
        .current_province_dev = 4,
        .national_ducat_monthly = 0.6,
        .admin_point_monthly = 7,
        .cost = {
            .dev_cost = 70,
            .increase_cost_multi = 10,
            .increase_cost_ratio = 0.03,
            .building_cost = 100,
            .tech_cost = 420,
        },
        .dev_effect = {
            .ducat_per_dev = 0.08,
        },
        .tech_effect = {
            .increase_tax_modifier = 0.4
        },
        .max_month = 120,
    };

    // float get_ducat_without_tech = cal_total_ducat_without_tech(&scenario);
    float max_cucat = 0;
    size_t optimized_month_to_wait_for_tech = 0;
    for (size_t month_to_wait_tech = 0;
        month_to_wait_tech < scenario.max_month;
        month_to_wait_tech++)
    {
        float get_ducat_withtech = cal_total_ducat_wait_for_tech_start_from_month(
            &scenario, month_to_wait_tech);
        if (max_cucat < get_ducat_withtech)
        {
            printf("New update to max: %u months and result is %8.4f ducat\n", 
                month_to_wait_tech, get_ducat_withtech);
            max_cucat = get_ducat_withtech;
            optimized_month_to_wait_for_tech = month_to_wait_tech;
        }

    }

    // printf("Ducat without tech %8.4f", get_ducat_without_tech);
    return 0;
}

static float cal_total_ducat_without_tech(struct scenario_t *scene)
{
    printf("%s!!", __func__);

    float current_ducat = 0;
    size_t current_admin_point = 0;
    for (size_t m = 0; m < scene->max_month; m++)
    {
        size_t num_of_multitude_to_dev_cost = 
            scene->current_province_dev / scene->cost.increase_cost_multi;
        size_t current_admin_point_for_new_dev = scene->cost.dev_cost + (size_t)(
            (float) num_of_multitude_to_dev_cost * scene->cost.increase_cost_ratio);
        current_admin_point += scene->admin_point_monthly;
        
        float current_ducat_of_the_month = scene->national_ducat_monthly + 
            scene->current_province_dev * scene->dev_effect.ducat_per_dev;
#if DEBUG_EACH_MONTH
        printf("month #%03u..curr: ap %03u, %8.4f ducat this month, ap cost %03u, %8.4f ducat total\n",
            m,
            current_admin_point,
            current_ducat_of_the_month,
            current_admin_point_for_new_dev,
            current_ducat);
#endif
        current_ducat += current_ducat_of_the_month;
        
        if (current_admin_point >= current_admin_point_for_new_dev)
        {
            scene->current_province_dev++;
            current_admin_point -= current_admin_point_for_new_dev;
        }
    }
    return current_ducat;
}

static float cal_total_ducat_wait_for_tech_start_from_month(
    struct scenario_t *scene, size_t month_start_wait_for_tech)
{
    printf("%s!!", __func__);
    float current_ducat = 0;
    size_t current_admin_point = 0;
    int is_tech_owned = 0;
    int is_building_owned = 0;
    for (size_t m = 0; m < scene->max_month; m++)
    {
        size_t num_of_multitude_to_dev_cost = 
            scene->current_province_dev / scene->cost.increase_cost_multi;
        size_t current_admin_point_for_new_dev = scene->cost.dev_cost + (size_t)(
            (float) num_of_multitude_to_dev_cost * scene->cost.increase_cost_ratio);
        current_admin_point += scene->admin_point_monthly;
        if ((is_tech_owned) && (!is_building_owned) && 
            (current_ducat > scene->cost.building_cost))
        {
            is_building_owned = 1;
            current_ducat -= scene->cost.building_cost;
        }
        float current_provincal_ducat_of_the_month = 
            scene->current_province_dev * scene->dev_effect.ducat_per_dev *
            (1 + is_building_owned * scene->tech_effect.increase_tax_modifier);
        float current_ducat_of_the_month = 
            scene->national_ducat_monthly + current_provincal_ducat_of_the_month;
        int is_wait_for_tech = (m > month_start_wait_for_tech) && (!is_tech_owned);

#if DEBUG_EACH_MONTH
        printf("month #%03u..curr: ap %03u, %8.4f province ducat this month, %8.4f ducat this month, ap cost %03u, %8.4f ducat total. Tech is wait %d, is owned %d. Is building owned  %d\n",
            m,
            current_admin_point,
            current_provincal_ducat_of_the_month,
            current_ducat_of_the_month,
            current_admin_point_for_new_dev,
            current_ducat,
            is_wait_for_tech,
            is_tech_owned,
            is_building_owned);
#endif
        current_ducat += current_ducat_of_the_month;
        
        if (is_wait_for_tech)
        {
            if (current_admin_point >= scene->cost.tech_cost)
            {
                is_tech_owned = 1;
                current_admin_point -= scene->cost.tech_cost;
            }
            continue;
        }
        if (current_admin_point >= current_admin_point_for_new_dev)
        {
            scene->current_province_dev++;
            current_admin_point -= current_admin_point_for_new_dev;
        }
    }
    return current_ducat;
}
