/*
* light_mesh.h — Factory for the traffic light pole and symbols.
 *
 * Commit: feat(geometry): update light mesh to single pole with dual symbols
 *
 * Depends on: render/mesh.h, common.h, config.h
 * Consumed by: main.
 */
#ifndef TRAFFIC3D_GEOMETRY_LIGHT_MESH_H
#define TRAFFIC3D_GEOMETRY_LIGHT_MESH_H

#include "traffic3d/render/mesh.h"
#include "traffic3d/common.h"
#include "traffic3d/config.h"

/*
 * Creates a single traffic light pole containing BOTH symbols.
 * - Top: A horizontal car symbol.
 * - Bottom: A vertical pedestrian symbol.
 * The colors are determined by the respective states.
 */
mesh_t* light_mesh_create(light_state_t vehicle_state,
                          light_state_t pedestrian_state,
                          const config_t* cfg);

#endif