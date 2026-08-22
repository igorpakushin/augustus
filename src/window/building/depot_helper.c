#include "depot_helper.h"

#include "building/building.h"
#include "building/storage.h"
#include "game/resource.h"

#include <string.h>

typedef int (*storage_predicate)(const data_storage *storage, building *store_building, resource_type resource);

static int is_available_storage(const data_storage *storage, building *store_building, resource_type resource)
{
    if (!storage->in_use || !storage->building_id ||
        store_building->state == BUILDING_STATE_MOTHBALLED ||
        store_building->state == BUILDING_STATE_RUBBLE ||
        (!resource_is_food(resource) && store_building->type == BUILDING_GRANARY)) {
        return 0;
    }

    return building_storage_resource_max_storable(store_building, resource) > 0;
}

static int is_unavailable_storage(const data_storage *storage, building *store_building, resource_type resource)
{
    if (!storage->in_use || !storage->building_id ||
        (!resource_is_food(resource) && store_building->type == BUILDING_GRANARY)) {
        return 0;
    }

    int max_storable = building_storage_resource_max_storable(store_building, resource);
    return (max_storable == 0 || store_building->state == BUILDING_STATE_MOTHBALLED) &&
        store_building->storage_id > 0 && store_building->state != BUILDING_STATE_RUBBLE;
}

// Shared pagination skeleton: walks every storage in the city once, counts every match
// against `predicate`, and writes building ids for matches falling within
// [offset, offset + limit) into out_building_ids. Returns the total match count
// across the whole city, independent of offset/limit.
static int collect_storages(storage_predicate predicate, resource_type resource,
    int offset, int limit, int *out_building_ids)
{
    if (limit > 0) {
        memset(out_building_ids, 0, sizeof(int) * limit);
    }

    int total_buildings = 0;
    int output_buildings = 0;

    int storage_array_size = building_storage_get_array_size();
    for (int i = 0; i < storage_array_size; i++) {
        const data_storage *storage = building_storage_get_array_entry(i);
        building *store_building = building_get(storage->building_id);

        if (!predicate(storage, store_building, resource)) {
            continue;
        }

        total_buildings++;

        if (total_buildings <= offset || output_buildings >= limit) {
            continue;
        }

        out_building_ids[output_buildings++] = storage->building_id;
    }

    return total_buildings;
}

int building_depot_get_available_storages(
    resource_type resource,
    int offset, int limit,
    int *available_building_ids // out: buffer of at least `limit` ints, zero-filled past the match count
) {
    return collect_storages(is_available_storage, resource, offset, limit, available_building_ids);
}

int building_depot_get_unavailable_storages(
    resource_type resource,
    int offset, int limit,
    int *unavailable_building_ids // out: buffer of at least `limit` ints, zero-filled past the match count
) {
    return collect_storages(is_unavailable_storage, resource, offset, limit, unavailable_building_ids);
}
