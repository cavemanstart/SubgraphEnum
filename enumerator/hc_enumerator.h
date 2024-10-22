//
// Created by Stone on 2023/12/11.
//

#ifndef XTRAGRAPHCOMPUTING_HC_ENUMERATOR_H
#define XTRAGRAPHCOMPUTING_HC_ENUMERATOR_H

#include <vector>
#include <set>
#include "util/sparsepp/spp.h"
#include "util/graph/directed_graph.h"
extern bool g_exit;
class HcEnumerator {
public:
    enum query_method {
        SimpleEnum,
        PathEnum,
        SortedEnum,
    };

    DirectedGraph * graph_;
    uint32_t num_vertices_;
    uint32_t src_;
    uint32_t dst_;
    uint8_t len_constrain_;
    query_method method_type_;
    uint32_t join_count_;

    //basic structure initialized after initializing graph
    bool * sign_; //default value false
    bool * visited_; //default value false

    //basic structure initialized after initializing k and gamma
    std::pair<uint8_t, uint8_t>* distance_;//default value k
    uint32_t * stack_; //dynamic store the path

    //basic structure dynamic allocate memory
    spp::sparse_hash_map<uint32_t, uint32_t> s_hash_;
    uint32_t * helper_offset_;
    uint32_t * csr_adj_;
    spp::sparse_hash_map<uint32_t, uint32_t> rev_s_hash_;
    uint32_t * rev_helper_offset_;
    uint32_t *rev_csr_adj_;

    //subgraph struct
    std::vector<uint32_t> subgraph_vertices;
    std::vector<std::set<uint32_t>> pre_subgraph;
    std::vector<std::vector<uint32_t>> subgraph;
    std::vector<std::vector<uint32_t>> sorted_subgraph;
    std::set<std::pair<uint32_t,uint32_t>> st;
    std::unordered_map<uint32_t,std::pair<uint32_t,uint32_t>> subgraph_degree;

    //statistic metadata
    uint64_t result_count_;
    uint64_t accessed_edges_;
    uint64_t concat_path_count_;
    uint64_t index_memory_cost_;
    uint64_t partial_path_memory_cost_;
    uint64_t total_path_memory_cost_;
    uint64_t subgraph_vertices_count_;
    uint64_t subgraph_edges_count_;
    uint64_t path_edges_counts_;

    //times
    uint64_t query_time_;
    uint64_t preprocess_time_;
    uint64_t bfs_time_;
    uint64_t rev_bfs_time_;
    uint64_t find_subgraph_edges_time_;
    uint64_t find_subgraph_vertices_time_;
    uint64_t pre_estimate_time_;
    uint64_t build_index_time_;
    uint64_t eliminate_edges_time_;
    uint64_t find_cutLine_time_;
    uint64_t left_dfs_time_;
    uint64_t right_dfs_time_;
    uint64_t concat_path_time_;
    uint64_t join_time_;
    uint64_t reduce_graph_time_;
    uint64_t get_pre_subgraph_time_;
    //statistic counter array
    std::vector<uint64_t> result_count_arr;
    std::vector<uint64_t> accessed_edges_arr;
    std::vector<uint64_t> left_path_count_arr;
    std::vector<uint64_t> right_path_count_arr;
    std::vector<uint64_t> concat_path_count_arr;
    std::vector<uint64_t> subgraph_vertices_count_arr;
    std::vector<uint64_t> subgraph_edges_count_arr;
    std::vector<uint64_t> path_edges_count_arr;
    std::vector<uint64_t> index_memory_cost_arr;
    std::vector<uint64_t> partial_path_memory_cost_arr;
    std::vector<uint64_t> total_path_memory_cost_arr;
    std::vector<uint64_t> bfs_time_arr;
    std::vector<uint64_t> rev_bfs_time_arr;
    std::vector<uint64_t> find_subgraph_vertices_time_arr;
    std::vector<uint64_t> find_subgraph_edges_time_arr;
    std::vector<uint64_t> pre_estimate_time_arr;
    std::vector<uint64_t> eliminate_edges_time_arr;
    std::vector<uint64_t> build_index_time_arr;
    std::vector<uint64_t> preprocess_time_arr;
    std::vector<uint64_t> query_time_arr;
    std::vector<uint64_t> find_cutLine_time_arr;
    std::vector<uint64_t> left_dfs_time_arr;
    std::vector<uint64_t> right_dfs_time_arr;
    std::vector<uint64_t> concat_path_time_arr;
    std::vector<uint64_t> join_time_arr;
    std::vector<uint64_t> reduce_graph_time_arr;
    std::vector<uint64_t> get_pre_subgraph_time_arr;

    //estimated data
    uint8_t cut_position_;
    uint64_t estimated_left_path_count_;
    uint64_t estimated_right_path_count_;
    uint64_t estimated_result_count_;

    //join metadata
    uint32_t *left_relation_;//store left path
    uint64_t left_path_count_; // nums of left path
    uint32_t *left_cursor_; //helper
    uint32_t *left_partial_begin_;
    uint32_t *left_partial_end_;
    uint32_t left_part_length_;
    uint32_t *right_relation_; //store right path
    uint64_t right_path_count_;
    uint32_t *right_cursor_;
    uint32_t right_part_length_;
    uint32_t *right_partial_begin_;
    uint32_t *right_partial_end_;
public:
    HcEnumerator();
    void init(DirectedGraph * graph);
    void init(query_method method_type, uint8_t len_constrain);
    void execute(uint32_t src, uint32_t dst);
    void get_pre_subgraph();
    void get_subgraph();
    void build_index();
    void build_index_opt();
    void sort_adj();
    void dfs_by_index(uint32_t src, uint8_t k);
    void simple_dfs(uint32_t src, uint8_t k);
    void sorted_dfs(uint32_t src, uint8_t k);
    void index_dfs_for_edge(uint32_t src, uint8_t k);
    void reset_for_next_batch_query();
    void reset_for_next_single_query();
    void clear();
    void update_counter();
    void reset_counter();
    void outputPathGraph();
    void outputPreSubgraph();
    void outputSubgraph();
};
#endif //XTRAGRAPHCOMPUTING_HC_ENUMERATOR_H
