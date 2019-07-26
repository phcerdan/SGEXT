/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "degree_viger_generator.hpp"
#include "viger_generator_hash.hpp"

namespace SG {

GraphType
create_graph_from_degree_sequence(const std::vector<int> &degree_sequence) {
    // Check degree_sequence
    const size_t sum = std::accumulate(std::begin(degree_sequence),
                                       std::end(degree_sequence), 0);
    if (sum % 2 != 0) {
        throw std::runtime_error(
                "The sum of all degrees should be even. sum = " +
                std::to_string(sum) + ".");
    }

    auto generator = degree_viger_generator(degree_sequence);

    if (!generator.havel_hakimi()) {
        throw std::runtime_error("havel_hakimi algorithm not succesful. The "
                                 "given degree sequence is not "
                                 "an undirected, simple graph.");
    }

    bool verbose = false;
#if !defined(NDEBUG)
    verbose = true;
#endif
    auto &gh = generator;
    gh.shuffle(5 * gh.arcs_, 100 * gh.arcs_,
               degree_viger_generator::ShuffleType::OPTIMAL_HEURISTICS,
               verbose);

    const GraphType graph = convert_degree_viger_generator_to_graph_type(gh);
    return graph;
};

GraphType convert_degree_viger_generator_to_graph_type(
        const degree_viger_generator &gen) {
    GraphType graph(gen.num_vertices_);
    for (int i = 0; i < gen.num_vertices_; ++i) {
        for (int j = 0; j < gen.deg_[i]; ++j) {
            auto neigh = gen.neigh_[i][j];
            if (neigh != generator::HASH_NONE && neigh > i) {
                boost::add_edge(i, neigh, SpatialEdge(), graph);
            }
        }
    }
    return graph;
}

bool degree_viger_generator::havel_hakimi() {
    const int degree_max = this->max_degree() + 1;
    // Sort vertices using basket-sort, in descending degrees
    int *nb = new int[degree_max];
    int *sorted = new int[num_vertices_];
    // init basket
    for (int i = 0; i < degree_max; i++)
        nb[i] = 0;
    // count basket
    for (int i = 0; i < num_vertices_; i++)
        nb[deg_[i]]++;
    // cumul
    int c = 0;
    for (int i = degree_max - 1; i >= 0; i--) {
        c += nb[i];
        nb[i] = -nb[i] + c;
    }
    // sort
    for (int i = 0; i < num_vertices_; i++)
        sorted[nb[deg_[i]]++] = i;

    // Binding process starts
    int first = 0;          // vertex with biggest residual degree
    int d = degree_max - 1; // maximum residual degree available

    c = 0;
    for (c = arcs_ / 2; c > 0;) {
        // pick a vertex. we could pick any, but here we pick the one with
        // biggest degree
        int v = sorted[first];
        // look for current degree of v
        while (nb[d] <= first)
            d--;
        // store it in dv
        int dv = d;
        // bind it !
        c -= dv;
        int dc = d;       // residual degree of vertices we bind to
        int fc = ++first; // position of the first vertex with degree dc

        while (dv > 0 && dc > 0) {
            int lc = nb[dc];
            if (lc != fc) {
                while (dv > 0 && lc > fc) {
                    // binds v with sorted[--lc]
                    dv--;
                    int w = sorted[--lc];
                    *(neigh_[v]++) = w;
                    *(neigh_[w]++) = v;
                }
                fc = nb[dc];
                nb[dc] = lc;
            }
            dc--;
        }
        if (dv != 0) { // We couldn't bind entirely v
            delete[] nb;
            delete[] sorted;
            compute_neigh();
            throw std::runtime_error(
                    "Error in degree_viger_generator::havel_hakimi():"
                    " Couldn't bind vertex " +
                    std::to_string(v) + " entirely (" + std::to_string(dv) +
                    " edges remaining)");
            return false;
        }
    }
    assert(c == 0);
    compute_neigh();
    delete[] nb;
    delete[] sorted;
    return true;
}

int degree_viger_generator::max_degree() const {
    int max_degree = 0;
    for (int i = 0; i < num_vertices_; i++) {
        if (deg_[i] > max_degree) {
            max_degree = deg_[i];
        }
    }
    return max_degree;
}

void degree_viger_generator::alloc(const std::vector<int> &degree_sequence) {
    num_vertices_ = std::size(degree_sequence);
    arcs_ = std::accumulate(std::begin(degree_sequence),
                            std::end(degree_sequence), 0);
    assert(arcs_ % 2 == 0); // arcs must be even
    deg_ = degree_sequence;
    links_ = new int[arcs_];
    neigh_ = new int *[num_vertices_];
    compute_neigh();
}

void degree_viger_generator::dealloc() {
    delete[] links_;
    delete[] neigh_;
}

void degree_viger_generator::compute_neigh() {
    int *p = links_;
    for (int i = 0; i < num_vertices_; i++) {
        neigh_[i] = p;
        p += deg_[i];
    }
}

bool degree_viger_generator::make_connected() {
    // assert(verify());
    if (arcs_ / 2 < num_vertices_ - 1) {
        // fprintf(stderr,"\ngraph::make_connected() failed : #edges <
        // #vertices-1\num_vertices_");
        return false;
    }

    // Data struct for the visit :
    // - buff[] contains vertices to visit
    // - dist[V] is V's distance modulo 4 to the root of its comp, or -1 if it
    // hasn't been visited yet
    const int MC_BUFF_SIZE = num_vertices_ + 2;
    constexpr int NOT_VISITED = 255;
    constexpr int FORBIDDEN = 254;
    int *buff = new int[MC_BUFF_SIZE];
    unsigned char *dist = new unsigned char[num_vertices_];
    for (int i = num_vertices_; i > 0; dist[--i] = NOT_VISITED) {
    }

    // Data struct to store components : either surplus trees or surplus edges
    // are stored at buff[]'s end
    // - A Tree is coded by one of its vertices
    // - An edge (a,b) is coded by the TWO ints a and b
    int *ffub = buff + MC_BUFF_SIZE;
    edge *edges = (edge *)ffub;
    int *trees = ffub;
    int *min_ffub = buff + 1 + (MC_BUFF_SIZE % 2 ? 0 : 1);

    // There will be only one "fatty" component, and trees.
    edge fatty_edge = {-1, -1};
    bool enough_edges = false;

    // start main loop
    for (int v0 = 0; v0 < num_vertices_; v0++)
        if (dist[v0] == NOT_VISITED) {
            // is v0 an isolated vertex?
            if (deg_[v0] == 0) {
                delete[] dist;
                delete[] buff;
                std::cerr << "degree_viger_generator::make_connected() "
                             "returns FALSE : "
                             "vertex " +
                                     std::to_string(v0) + " has degree 0."
                          << std::endl;
                return false;
            }
            dist[v0] = 0; // root
            int *to_visit = buff;
            int *current = buff;
            *(to_visit++) = v0;

            // explore component connected to v0
            bool is_a_tree = true;
            while (current != to_visit) {
                int v = *(current++);
                unsigned char current_dist = dist[v];
                unsigned char next_dist = (current_dist + 1) & 0x03;
                // unsigned char prev_dist = (current_dist-1) & 0x03;
                int *ww = neigh_[v];
                int w;
                for (int k = deg_[v]; k--; ww++) {
                    if (dist[w = *ww] == NOT_VISITED) {
                        // we didn't visit *w yet
                        dist[w] = next_dist;
                        *(to_visit++) = w;
                        if (to_visit > min_ffub)
                            min_ffub += 2; // update limit of ffub's storage
                                           // assert(verify());
                    } else if (dist[w] == next_dist ||
                               (w >= v && dist[w] == current_dist)) {
                        // we found a removable edge
                        if (trees != ffub) {
                            // some trees still.. Let's merge with them!
                            assert(trees >= min_ffub);
                            assert(edges == (edge *)ffub);
                            swap_edges(v, w, *trees, neigh_[*trees][0]);
                            trees++;
                            // assert(verify());
                        } else if (is_a_tree) {
                            // we must merge with the fatty component
                            is_a_tree = false;
                            if (fatty_edge.from < 0) {
                                // we ARE the first component! fatty is us
                                fatty_edge.from = v;
                                fatty_edge.to = w;
                            } else {
                                // we connect to fatty
                                swap_edges(fatty_edge.from, fatty_edge.to, v,
                                           w);
                                fatty_edge.to = w;
                                // assert(verify());
                            }
                        } else if (!enough_edges) {
                            // Store the removable edge for future use
                            if (edges <= (edge *)min_ffub + 1)
                                enough_edges = true;
                            else {
                                edges--;
                                edges->from = v;
                                edges->to = w;
                            }
                        }
                    }
                }
            }
            // Mark component
            while (to_visit != buff)
                dist[*(--to_visit)] = FORBIDDEN;
            // Check if it is a tree
            if (is_a_tree) {
                assert(deg_[v0] != 0);
                if (edges != (edge *)ffub) {
                    // let's bind the tree we found with a removable edge in
                    // stock
                    assert(trees == ffub);
                    if (edges < (edge *)min_ffub)
                        edges = (edge *)min_ffub;
                    swap_edges(v0, neigh_[v0][0], edges->from, edges->to);
                    edges++;
                    // assert(verify());
                } else if (fatty_edge.from >= 0) {
                    // if there is a fatty component, let's merge with it ! and
                    // discard fatty :-/
                    assert(trees == ffub);
                    swap_edges(v0, neigh_[v0][0], fatty_edge.from,
                               fatty_edge.to);
                    fatty_edge.from = -1;
                    fatty_edge.to = -1;
                    // assert(verify());
                } else {
                    // add the tree to the list of trees
                    assert(trees > min_ffub);
                    *(--trees) = v0;
                    // assert(verify());
                }
            }
        }
    delete[] buff;
    delete[] dist;
    // Should ALWAYS return true : either we have no tree left, or we are a
    // unique, big tree
    return (trees == ffub || ((trees + 1) == ffub && fatty_edge.from < 0));
}

// Swap edges. The swap MUST be valid !!!
void degree_viger_generator::swap_edges(int from1,
                                        int to1,
                                        int from2,
                                        int to2) {
    fast_rpl(neigh_[from1], to1, to2);
    fast_rpl(neigh_[from2], to2, to1);
    fast_rpl(neigh_[to1], from1, from2);
    fast_rpl(neigh_[to2], from2, from1);
}

int *degree_viger_generator::fast_rpl(int *m, const int a, const int b) {
    while (*m != a)
        m++;
    *m = b;
    return m;
}

bool degree_viger_generator::verify(int mode) {

    static constexpr int VERIFY_NORMAL = 0;
    static constexpr int VERIFY_NONEIGH = 1;
    static constexpr int VERIFY_NOARCS = 2;
    assert(neigh_[0] == links_);
    // verify edges count
    if ((mode & VERIFY_NOARCS) == 0) {
        int sum = 0;
        for (int i = 0; i < num_vertices_; i++)
            sum += deg_[i];
        assert(sum == arcs_);
    }
    // verify neigh_[] and deg_[] compatibility
    if ((mode & VERIFY_NONEIGH) == 0)
        for (int i = 0; i < num_vertices_ - 1; i++)
            assert(neigh_[i] + deg_[i] == neigh_[i + 1]);
    // verify vertex range
    for (int i = 0; i < arcs_; i++)
        assert(links_[i] >= 0 && links_[i] < num_vertices_);
    // verify simplicity
    //  for(i=0; i<num_vertices_; i++) for(j=0; j<deg_[i]; j++) for(k=j+1;
    //  k<deg_[i]; k++)
    //    assert(neigh_[i][j]!=neigh_[i][k]);
    // verify symmetry
    for (int i = 0; i < num_vertices_; i++)
        for (int j = 0; j < deg_[i]; j++) {
            int v = neigh_[i][j];
            int nb = 0;
            for (int k = 0; k < deg_[v]; k++)
                if (neigh_[v][k] == i)
                    nb++;
            assert(nb > 0);
        }
    return true;
}

unsigned long degree_viger_generator::shuffle(unsigned long times,
                                              unsigned long maxtimes,
                                              ShuffleType type,
                                              const bool verbose) {
    if (verbose) {
        std::cout << "Starting to shuffle" << std::endl;
    }
    // assert(verify());
    // counters
    unsigned long nb_swaps = 0;
    unsigned long all_swaps = 0;
    unsigned long cost = 0;
    // window
    double T = double(std::min((unsigned long)(arcs_), times) / 10);
    if (type == ShuffleType::OPTIMAL_HEURISTICS)
        T = double(optimal_window());
    if (type == ShuffleType::BRUTE_FORCE_HEURISTICS)
        T = double(times * 2);
    // isolation test parameter, and buffers
    double K = 2.4;
    int *Kbuff = new int[int(K) + 1];
    bool *visited = new bool[num_vertices_];
    for (int i = 0; i < num_vertices_; i++)
        visited[i] = false;
    // Used for monitoring , active only if VERBOSE()
    int failures = 0;
    int successes = 0;
    double avg_K = 0;
    double avg_T = 0;
    unsigned long next = times;
    next = 0;

    // Shuffle: while #edge swap attempts validated by connectivity < times ...
    while (times > nb_swaps && maxtimes > all_swaps) {
        // Backup graph
        int *save = backup();
        // Prepare counters, K, T
        unsigned long swaps = 0;
        int K_int = 0;
        if (type == ShuffleType::FINAL_HEURISTICS ||
            type == ShuffleType::BRUTE_FORCE_HEURISTICS)
            K_int = int(K);
        unsigned long T_int = (unsigned long)(floor(T));
        if (T_int < 1)
            T_int = 1;
        // compute cost
        cost += T_int;
        if (K_int > 2)
            cost += (unsigned long)(K_int) * (unsigned long)(T_int);
        // Perform T edge swap attempts
        for (int i = T_int; i > 0; i--) {
            // try one swap
            swaps += (unsigned long)(random_edge_swap(K_int, Kbuff, visited));
            all_swaps++;
            // Verbose
            if (nb_swaps + swaps > next) {
                next = (nb_swaps + swaps) +
                       std::max((unsigned long)(100),
                                (unsigned long)(times / 1000));
                int progress = int(double(nb_swaps + swaps) / double(times));
                if (verbose) {
                    std::cout << "Shuffle progress: " << progress << std::endl;
                }
            }
        }
        // test connectivity
        cost += (unsigned long)(arcs_ / 2);
        bool ok = is_connected();
        // performance monitor
        {
            avg_T += double(T_int);
            avg_K += double(K_int);
            if (ok)
                successes++;
            else
                failures++;
        }
        // restore graph if needed, and count validated swaps
        if (ok)
            nb_swaps += swaps;
        else {
            restore(save);
            next = nb_swaps;
        }
        delete[] save;
        // Adjust K and T following the heuristics.
        switch (type) {
            int steps;
        case ShuffleType::GKAN_HEURISTICS:
            if (ok)
                T += 1.0;
            else
                T *= 0.5;
            break;
        case ShuffleType::FAB_HEURISTICS:
            steps = 50 / (8 + failures + successes);
            if (steps < 1)
                steps = 1;
            while (steps--)
                if (ok)
                    T *= 1.17182818;
                else
                    T *= 0.9;
            if (T > double(5 * arcs_))
                T = double(5 * arcs_);
            break;
        case ShuffleType::FINAL_HEURISTICS:
            if (ok) {
                if ((K + 10.0) * T > 5.0 * double(arcs_))
                    K /= 1.03;
                else
                    T *= 2;
            } else {
                K *= 1.35;
                delete[] Kbuff;
                Kbuff = new int[int(K) + 1];
            }
            break;
        case ShuffleType::OPTIMAL_HEURISTICS:
            if (ok)
                T = double(optimal_window());
            break;
        case ShuffleType::BRUTE_FORCE_HEURISTICS:
            K *= 2;
            delete[] Kbuff;
            Kbuff = new int[int(K) + 1];
            break;
        default:
            throw std::logic_error(
                    "Error in degree_viger_generator::shuffle(): "
                    "Unknown heuristics type");
            return 0;
        }
    }

    delete[] Kbuff;
    delete[] visited;

    if (maxtimes <= all_swaps) {
        std::cerr << "WARNING: Cannot shuffle graph, maybe there is only a "
                     "single one?"
                  << std::endl;
    }

    // Status report
    if (verbose) {
        std::cout << "*** Shuffle Monitor ***" << std::endl;
        std::cout << " - Average cost : " << cost / double(nb_swaps)
                  << " / validated edge swap" << std::endl;
        std::cout << " - Connectivity tests : " << successes + failures << " ("
                  << successes << " successes, " << failures << " failures)"
                  << std::endl;
        std::cout << " - Average window : "
                  << int(avg_T / double(successes + failures)) << std::endl;
        if (type == ShuffleType::FINAL_HEURISTICS ||
            type == ShuffleType::BRUTE_FORCE_HEURISTICS) {
            std::cout << " - Average isolation test width : "
                      << avg_K / double(successes + failures) << std::endl;
        }
    }
    return nb_swaps;
}

int degree_viger_generator::optimal_window(const bool verbose) {
    int Tmax;
    int optimal_T = 1;
    double min_cost = 1e+99;
    int *back = backup();
    // on cherche une borne sup pour Tmax
    int been_greater = 0;
    for (Tmax = 1; Tmax <= 5 * arcs_; Tmax *= 2) {
        double c = average_cost(Tmax, back, min_cost);
        if (c > 1.5 * min_cost)
            break;
        if (c > 1.2 * min_cost && ++been_greater >= 3)
            break;
        if (c < min_cost) {
            min_cost = c;
            optimal_T = Tmax;
        }
        if (verbose) {
            std::cout << "Tmax = " << Tmax << " [" << min_cost << "]"
                      << std::endl;
        }
    }
    // on cree Tmin
    int Tmin = int(0.5 * double(arcs_) / (min_cost - 1.0));
    if (verbose) {
        std::cout << "Optimal T is in [ " << Tmin << ", " << Tmax << "]"
                  << std::endl;
    }
    // on cherche autour
    double span = 2.0;
    int try_again = 4;
    while (span > 1.05 && optimal_T <= 5 * arcs_) {
        if (verbose) {
            std::cout << "Best T [cost]: " << optimal_T << " [" << min_cost
                      << "]" << std::endl;
        }
        int T_low = int(double(optimal_T) / span);
        int T_high = int(double(optimal_T) * span);
        double c_low = average_cost(T_low, back, min_cost);
        double c_high = average_cost(T_high, back, min_cost);
        if (c_low < min_cost && c_high < min_cost) {
            if (try_again--)
                continue;
            if (verbose) {
                std::cout << "Warning: when looking for optimal T,"
                          << std::endl;
                std::cout << "Low:" << T_low << " [" << c_low << "]  ";
                std::cout << "Middle:" << optimal_T << " [" << min_cost
                          << "]  ";
                std::cout << "High:" << T_high << " [" << c_high << "]";
                std::cout << std::endl;
            }
            delete[] back;
            return optimal_T;
        }
        if (c_low < min_cost) {
            optimal_T = T_low;
            min_cost = c_low;
        } else if (c_high < min_cost) {
            optimal_T = T_high;
            min_cost = c_high;
        };
        span = pow(span, 0.618);
    }
    delete[] back;
    return optimal_T;
}

bool bernoulli_param_is_lower(int success, int trials, double param) {
    constexpr double _TRUST_BERNOULLI_LOWER = 0.01;
    if (double(success) >= double(trials) * param)
        return false;
    double comb = 1.0;
    double fact = 1.0;
    for (int i = 0; i < success; i++) {
        comb *= double(trials - i);
        fact *= double(i + 1);
    }
    comb /= fact;
    comb *= pow(param, double(success)) *
            exp(double(trials - success) * log1p(-param));
    double sum = comb;
    while (success && sum < _TRUST_BERNOULLI_LOWER) {
        comb *= double(success) * (1.0 - param) /
                (double(trials - success) * param);
        sum += comb;
        success--;
    }
    // fprintf(stderr,"bernoulli test : %d/%d success against p=%f ->
    // %s\n",success, trials, param, (sum < _TRUST_BERNOULLI_LOWER) ? "lower" :
    // "can't say");
    return (sum < _TRUST_BERNOULLI_LOWER);
}

double
degree_viger_generator::average_cost(int T, int *backup, double min_cost) {
    constexpr int _MIN_SUCCESS_FOR_BERNOULLI_TRUST = 100;
    if (T < 1)
        return 1e+99;
    int successes = 0;
    int trials = 0;
    while (successes < _MIN_SUCCESS_FOR_BERNOULLI_TRUST &&
           !bernoulli_param_is_lower(successes, trials, 1.0 / min_cost)) {
        if (try_shuffle(T, 0, backup))
            successes++;
        trials++;
    }
    if (successes >= _MIN_SUCCESS_FOR_BERNOULLI_TRUST)
        return double(trials) / double(successes) *
               (1.0 + double(arcs_ / 2) / double(T));
    else
        return 2.0 * min_cost;
}

void degree_viger_generator::restore(int *b) {
    using namespace generator;
    // init();
    for (int i = 0; i < arcs_; i++)
        links_[i] = HASH_NONE;
    int i;
    auto dd = deg_;
    for (i = 0; i < num_vertices_; i++)
        deg_[i] = 0;
    for (i = 0; i < num_vertices_ - 1; i++) {
        while (deg_[i] < dd[i]) {
            add_edge(i, *b, dd);
            b++;
        }
    }
}

inline bool degree_viger_generator::add_edge(int a,
                                             int b,
                                             const std::vector<int> &realdeg) {
    using namespace generator;
    int deg_a = realdeg[a];
    const auto size = arcs_;
    if (deg_a == deg_[a])
        return false;
    // Check that edge was not already inserted
    assert(fast_search(neigh_[a],
                       int((a == num_vertices_ - 1 ? links_ + size : neigh_[a + 1]) -
                           neigh_[a]),
                       b) == NULL);
    assert(fast_search(neigh_[b],
                       int((b == num_vertices_ - 1 ? links_ + size : neigh_[b + 1]) -
                           neigh_[b]),
                       a) == NULL);
    assert(deg_[a] < deg_a);
    int deg_b = realdeg[b];
    if (IS_HASH(deg_a))
        *H_add(neigh_[a], HASH_EXPAND(deg_a), b) = b;
    else
        neigh_[a][deg_[a]] = b;
    if (IS_HASH(deg_b))
        *H_add(neigh_[b], HASH_EXPAND(deg_b), a) = a;
    else
        neigh_[b][deg_[b]] = a;
    deg_[a]++;
    deg_[b]++;
    // Check that edge was actually inserted
    assert(fast_search(neigh_[a],
                       int((a == num_vertices_ - 1 ? links_ + size : neigh_[a + 1]) -
                           neigh_[a]),
                       b) != NULL);
    assert(fast_search(neigh_[b],
                       int((b == num_vertices_ - 1 ? links_ + size : neigh_[b + 1]) -
                           neigh_[b]),
                       a) != NULL);
    return true;
}

bool degree_viger_generator::try_shuffle(int T, int K, int *backup_graph) {
    // init all
    int *Kbuff = NULL;
    bool *visited = NULL;
    if (K > 2) {
        Kbuff = new int[K];
        visited = new bool[num_vertices_];
        for (int i = 0; i < num_vertices_; i++)
            visited[i] = false;
    }
    int *back = backup_graph;
    if (back == NULL)
        back = backup();
    // perform T edge swap attempts
    while (T--)
        random_edge_swap(K, Kbuff, visited);
    // clean
    if (visited != NULL)
        delete[] visited;
    if (Kbuff != NULL)
        delete[] Kbuff;
    // check & restore
    bool yo = is_connected();
    restore(back);
    if (backup_graph == NULL)
        delete[] back;
    return yo;
}

int *degree_viger_generator::backup() {
    using namespace generator;
    int *b = new int[arcs_ / 2];
    int *c = b;
    int *p = links_;
    for (int i = 0; i < num_vertices_; i++)
        for (int d = HASH_SIZE(deg_[i]); d--; p++)
            if (*p != HASH_NONE && *p > i)
                *(c++) = *p;
    assert(c == b + (arcs_ / 2));
    return b;
}

int degree_viger_generator::random_edge_swap(int K, int *Kbuff, bool *visited) {
    // Pick two random vertices a and c
    int f1 = pick_random_vertex();
    int f2 = pick_random_vertex();
    // Check that f1 != f2
    if (f1 == f2)
        return 0;
    // Get two random edges (f1,*f1t1) and (f2,*f2t2)
    int *f1t1 = random_neighbour(f1);
    int t1 = *f1t1;
    int *f2t2 = random_neighbour(f2);
    int t2 = *f2t2;
    // Check simplicity
    if (t1 == t2 || f1 == t2 || f2 == t1)
        return 0;
    if (is_edge(f1, t2) || is_edge(f2, t1))
        return 0;
    // Swap
    using namespace generator;
    int *f1t2 = H_rpl(neigh_[f1], deg_[f1], f1t1, t2);
    int *f2t1 = H_rpl(neigh_[f2], deg_[f2], f2t2, t1);
    int *t1f2 = H_rpl(neigh_[t1], deg_[t1], f1, f2);
    int *t2f1 = H_rpl(neigh_[t2], deg_[t2], f2, f1);
    // isolation test
    if (K <= 2)
        return 1;
    if (!isolated(f1, K, Kbuff, visited) && !isolated(f2, K, Kbuff, visited))
        return 1;
    // undo swap
    H_rpl(neigh_[f1], deg_[f1], f1t2, t1);
    H_rpl(neigh_[f2], deg_[f2], f2t1, t2);
    H_rpl(neigh_[t1], deg_[t1], t1f2, f1);
    H_rpl(neigh_[t2], deg_[t2], t2f1, f2);
    return 0;
}

int degree_viger_generator::pick_random_vertex() const {
    using namespace generator;
    std::uniform_int_distribution<int> uid(0, arcs_ - 1);
    int v;
    do
        v = links_[uid(RNG::engine())];
    while (v == HASH_NONE);
    return v;
}
int *degree_viger_generator::random_neighbour(const int v) const {
    return generator::H_random(neigh_[v], deg_[v]);
}

bool degree_viger_generator::is_edge(int a, int b) const {
    using namespace generator;
    assert(H_is(neigh_[a], deg_[a], b) ==
           (fast_search(neigh_[a], HASH_SIZE(deg_[a]), b) != NULL));
    assert(H_is(neigh_[b], deg_[b], a) ==
           (fast_search(neigh_[b], HASH_SIZE(deg_[b]), a) != NULL));
    assert(H_is(neigh_[a], deg_[a], b) == H_is(neigh_[b], deg_[b], a));
    if (deg_[a] < deg_[b])
        return H_is(neigh_[a], deg_[a], b);
    else
        return H_is(neigh_[b], deg_[b], a);
}

bool degree_viger_generator::is_connected() const {
    bool *visited = new bool[num_vertices_];
    int *buff = new int[num_vertices_];
    int comp_size = depth_search(visited, buff);
    delete[] visited;
    delete[] buff;
    return (comp_size == num_vertices_);
}

int degree_viger_generator::depth_search(bool *visited,
                                         int *buff,
                                         int v0) const {
    using namespace generator;
    for (int i = 0; i < num_vertices_; i++)
        visited[i] = false;
    int *to_visit = buff;
    int nb_visited = 1;
    visited[v0] = true;
    *(to_visit++) = v0;
    while (to_visit != buff && nb_visited < num_vertices_) {
        int v = *(--to_visit);
        int *ww = neigh_[v];
        int w;
        for (int k = HASH_SIZE(deg_[v]); k--; ww++) {
            if (HASH_NONE != (w = *ww) && !visited[w]) {
                visited[w] = true;
                nb_visited++;
                *(to_visit++) = w;
            }
        }
    }
    return nb_visited;
}

bool degree_viger_generator::isolated(int v,
                                      int K,
                                      int *Kbuff,
                                      bool *visited) const {
    using namespace generator;
    if (K < 2)
        return false;
    int *seen = Kbuff;
    int *known = Kbuff;
    int *max = Kbuff + K;
    *(known++) = v;
    visited[v] = true;
    bool is_isolated = true;

    while (known != seen) {
        v = *(seen++);
        int *ww = neigh_[v];
        int w;
        for (int d = HASH_SIZE(deg_[v]); d--; ww++)
            if ((w = *ww) != HASH_NONE && !visited[w]) {
                if (known == max) {
                    is_isolated = false;
                    goto end_isolated;
                }
                visited[w] = true;
                *(known++) = w;
            }
    }
end_isolated:
    // Undo the changes to visited[]...
    while (known != Kbuff)
        visited[*(--known)] = false;
    return is_isolated;
}

} // namespace SG
