#ifndef WINDOW_BUILDING_DEPOT_HELPER_H
#define WINDOW_BUILDING_DEPOT_HELPER_H

#include "game/resource.h"

// Helpers for the Cart Depot order UI: given a resource, find which granary/warehouse
// buildings in the city are valid targets to set as a depot's source or destination
// for that resource.

// Fills available_building_ids[0..limit) with up to `limit` eligible building ids,
// starting at `offset` into the full match list (for pagination/scrolling).
// Any unused trailing slots (fewer than `limit` matches remaining) are set to 0
// "eligible" buildings are active (non-mothballed, non-rubble) granaries or warehouses
// currently configured to accept `resource` (granaries only for food resources).
// Returns the total number of matches across the whole city, independent of offset/limit,
// so callers can determine how many results exist beyond the current page.
int building_depot_get_available_storages(
    resource_type resource,
    int offset, int limit,
    int *available_building_ids // out: buffer of at least `limit` ints, zero-filled past the match count
);

// Fills unavailable_building_ids[0..limit) with up to `limit` "unavailable" building ids,
// starting at `offset` into the full match list (for pagination/scrolling).
// Any unused trailing slots (fewer than `limit` matches remaining) are set to 0
// "unavailable" buildings are granaries or warehouses that are structurally capable of
// storing `resource` but are currently mothballed, full, or configured not to accept it.
// Buildings in rubble, and granaries asked about a non-food resource, never appear in
// either the available or unavailable lists.
// Returns the total number of matches across the whole city, independent of offset/limit,
// so callers can determine how many results exist beyond the current page.
int building_depot_get_unavailable_storages(
    resource_type resource,
    int offset, int limit,
    int *unavailable_building_ids // out: buffer of at least `limit` ints, zero-filled past the match count
);

#endif // WINDOW_BUILDING_DEPOT_HELPER_H
