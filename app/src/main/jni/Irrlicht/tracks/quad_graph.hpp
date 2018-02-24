//  $Id: quad_graph.hpp 6256 2010-10-11 21:49:35Z hikerstk $
//
//  SuperTuxKart - a fun racing game with go-kart
//  Copyright (C) 2009 Joerg Henrichs
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 3
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, B

#ifndef HEADER_QUAD_GRAPH_HPP
#define HEADER_QUAD_GRAPH_HPP

#include <vector>
#include <string>

#include "tracks/graph_node.hpp"
#include "tracks/quad_set.hpp"

class CheckLine;

/**
  * \brief This class stores a graph of quads.
  * \ingroup tracks
  */
class QuadGraph
{
    
private:
    /** The actual graph data structure. */
    std::vector<GraphNode*>  m_all_nodes;
    /** The set of all quads. */
    QuadSet                 *m_all_quads;
    /** For debug mode only: the node of the debug mesh. */
    scene::ISceneNode       *m_node;
    /** For debug only: the mesh of the debug mesh. */
    scene::IMesh            *m_mesh;
    /** For debug only: the actual mesh buffer storing the quads. */
    scene::IMeshBuffer      *m_mesh_buffer;

    /** The length of the first loop. */
    float                    m_lap_length;

    /** The minimum coordinates of the quad graph. */
    Vec3                     m_min_coord;

    /** Scaling for mini map. */
    float                    m_scaling;

    /** Stores the filename - just used for error messages. */
    std::string              m_quad_filename;

    void setDefaultSuccessors();
    void load         (const std::string &filename);
    void createMesh(bool show_invisible=true);
    int  getPredecessor(unsigned int target_node) const;
public:
    static const int UNKNOWN_SECTOR;

                 QuadGraph     (const std::string &quad_file_name, 
                                const std::string graph_file_name);
                ~QuadGraph     ();
    void         createDebugMesh();
    void         cleanupDebugMesh();
    void         getSuccessors(int node_number, 
                               std::vector<unsigned int>& succ) const;
    void         spatialToTrack(Vec3 *dst, const Vec3& xyz, 
                                const int sector)               const;
    void         findRoadSector(const Vec3& XYZ, int *sector,
                            std::vector<int> *all_sectors=NULL) const;
    int          findOutOfRoadSector(const Vec3& xyz,
                                     const int curr_sector=UNKNOWN_SECTOR,
                                     std::vector<int> *all_sectors=NULL
                                     ) const;
    void         setDefaultStartPositions(std::vector<btTransform> 
                                                       *start_transforms,
                                         unsigned int karts_per_row,
                                         float forwards_distance=1.5f,
                                         float sidewards_distance=1.5f,
                                         float upwards_distance=0.0f) const;
    video::ITexture *makeMiniMap(const core::dimension2du &where,
                                 const std::string &name, 
                                 const video::SColor &fill_color
                                        =video::SColor(127, 255, 255, 255) );
    void         mapPoint2MiniMap(const Vec3 &xyz, Vec3 *out) const;

    /** Returns the number of nodes in the graph. */
    unsigned int getNumNodes() const { return m_all_nodes.size();         } 
    // ----------------------------------------------------------------------
    /** Return the distance to the j-th successor of node n. */
    float        getDistanceToNext(int n, int j) const
                         { return m_all_nodes[n]->getDistanceToSuccessor(j);}
    // ----------------------------------------------------------------------
    /** Returns the angle of the line between node n and its j-th. 
     *  successor. */
    float        getAngleToNext(int n, int j) const
                         { return m_all_nodes[n]->getAngleToSuccessor(j);   }
    // ----------------------------------------------------------------------
    /** Returns the number of successors of a node n. */
    int          getNumberOfSuccessors(int n) const 
                         { return m_all_nodes[n]->getNumberOfSuccessors();  }
    // ----------------------------------------------------------------------
    /** Returns the quad that belongs to a graph node. */
    const Quad&  getQuad(unsigned int j) const
                 { return m_all_quads->getQuad(m_all_nodes[j]->getIndex()); }
    // ----------------------------------------------------------------------
    /** Returns the quad that belongs to a graph node. */
    GraphNode&   getNode(unsigned int j) const{ return *m_all_nodes[j]; }
    // ----------------------------------------------------------------------
    /** Returns the distance from the start to the beginning of a quad. */
    float        getDistanceFromStart(int j) const
                           { return m_all_nodes[j]->getDistanceFromStart(); }
    // ----------------------------------------------------------------------
    /** Returns the length of the main driveline. */
    float        getLapLength() const {return m_lap_length; }
    // ----------------------------------------------------------------------
};   // QuadGraph

#endif
