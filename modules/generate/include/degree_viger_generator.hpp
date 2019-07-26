/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef DEGREE_VIGER_GENERATOR_HPP
#define DEGREE_VIGER_GENERATOR_HPP

#include "spatial_graph.hpp"
#include <vector>

namespace SG {

class degree_viger_generator {
  public:
    degree_viger_generator(const std::vector<int> &degree_sequence) {
        alloc(degree_sequence);
    }

    ~degree_viger_generator() { dealloc(); }
    /**
     * Bind the graph avoiding multiple edges or self-edges (return
     * false if fail)
     *
     * From Wikipedia: The Havel–Hakimi algorithm is an algorithm in
     * graph theory solving the graph realization problem. That is, it
     * answers the following question: Given a finite list of
     * nonnegative integers, is there a simple graph such that its
     * degree sequence is exactly this list. Here, the "degree sequence"
     * is a list of numbers that for each vertex of the graph states how
     * many neighbors it has. For a positive answer the list of integers
     * is called graphic. The algorithm constructs a special solution if
     * one exists or proves that one cannot find a positive answer. This
     * construction is based on a recursive algorithm. The algorithm was
     * published by Havel (1955), and later by Hakimi (1962).
     *
     * @return
     */
    bool havel_hakimi();
    int max_degree() const;
    void compute_neigh();
    bool make_connected();
    enum class ShuffleType {
        FINAL_HEURISTICS = 0,
        GKAN_HEURISTICS = 1,
        FAB_HEURISTICS = 2,
        OPTIMAL_HEURISTICS = 3,
        BRUTE_FORCE_HEURISTICS = 4
    };
    /// Connected Shuffle
    unsigned long shuffle(unsigned long times,
                          unsigned long max_times,
                          ShuffleType shuffle_type,
                          const bool verbose);
    using edge = struct {
        int from;
        int to;
    };
    /** Number of vertices of graph */
    int num_vertices_;
    /** Number of arcs = 2*edges */
    int arcs_;
    /** Degree sequence of graph */
    std::vector<int> deg_;
    /** The array containing all links */
    int *links_;
    /** The array containing pointers to adjacency list of every vertices */
    int **neigh_;

  private:
    // Swap edges. The swap MUST be valid !!!
    inline void swap_edges(int from1, int to1, int from2, int to2);
    /// Fast replace
    inline int *fast_rpl(int *m, const int a, const int b);
    void alloc(const std::vector<int> &degree_sequence);
    void dealloc();

    /**
     * For debug purposes
     *
     * @param mode
     *
     * @return
     */
    bool verify(int mode);

    // Backup graph [sizeof(int) bytes per edge]
    int *backup();
    // Restore from backup
    void restore(int *back);
    /// Optimal window for the gkantsidis heuristics
    int optimal_window(const bool verbose = false);
    /// Average unitary cost per post-validated edge swap, for some window
    double average_cost(int T, int *back, double min_cost);
    /** Try to shuffle T times. Return true if at the end, the graph was still
     * connected. */
    bool try_shuffle(int T, int K, int *backup_graph);

    // Random edge swap ATTEMPT. Return 1 if attempt was a succes, 0 otherwise
    int random_edge_swap(int K, int *Kbuff, bool *visited);
    // Pick random edge, and gives a corresponding vertex
    inline int pick_random_vertex() const;
    // Pick random neighbour
    inline int *random_neighbour(const int v) const;
    // is edge ?
    inline bool is_edge(int a, int b) const;
    // Test if graph is connected
    bool is_connected() const;
    // Test if vertex is in an isolated component of size<K
    bool isolated(int v, int K, int *Kbuff, bool *visited) const;
    // Depth-first search.
    int depth_search(bool *visited, int *buff, int v0 = 0) const;

    // Add edge (a,b). Return FALSE if vertex a is already full.
    // WARNING : only to be used by havelhakimi(), restore() or constructors
    inline bool add_edge(int a, int b, const std::vector<int> &realdeg);
};

GraphType
create_graph_from_degree_sequence(const std::vector<int> &degree_sequence);

GraphType
convert_degree_viger_generator_to_graph_type(const degree_viger_generator &gen);

} // namespace SG
#endif
