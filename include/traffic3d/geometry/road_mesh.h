/*
* road_mesh.h — Factory for the road, sidewalks, and crosswalk.
 *
 * Commit: feat(geometry): add road mesh factory header
 *
 * Depends on: render/mesh.h, config.h
 * Consumed by: main.
 */
#ifndef TRAFFIC3D_GEOMETRY_ROAD_MESH_H
#define TRAFFIC3D_GEOMETRY_ROAD_MESH_H

#include "traffic3d/render/mesh.h"
#include "traffic3d/config.h"

/* Creates a single mesh containing the asphalt, sidewalks, lane lines,
 * grass, and a zebra crosswalk. */
mesh_t* road_mesh_create(const config_t* cfg);

#endif