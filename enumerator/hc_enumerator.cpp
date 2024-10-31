//
// Created by Stone on 2023/12/11.
//

#include <cassert>
#include <chrono>
#include <queue>
#include <iostream>
#include <fstream>
#include "hc_enumerator.h"
#include "util/log/log.h"
bool g_exit = false;
HcEnumerator::HcEnumerator() {
    graph_ = nullptr;
    sign_ = nullptr;
    visited_ = nullptr;
    distance_ = nullptr;
    helper_offset_ = nullptr;
    rev_helper_offset_ = nullptr;
    csr_adj_ = nullptr;
    rev_csr_adj_ = nullptr;
}
void HcEnumerator::execute(uint32_t src, uint32_t dst) {
    assert(len_constrain_>0);
    auto start = std::chrono::high_resolution_clock::now();
    src_ = src;
    dst_ = dst;
    if(method_type_ == query_method::SimpleEnum){
        get_pre_subgraph();
        simple_dfs(src_,0);
    }else if(method_type_ == query_method::PathEnum){
        get_pre_subgraph();
//        outputPreSubgraph();
        build_index();
        preprocess_time_ = find_subgraph_vertices_time_ + build_index_time_;
        dfs_by_index(src_,0);
//        outputPathGraph();
        path_edges_counts_ = st.size();
    }else if(method_type_ == query_method::SortedEnum){
        get_pre_subgraph();
//        outputPreSubgraph();
        get_subgraph();
//        outputSubgraph();
        sort_adj();
        preprocess_time_ = find_subgraph_vertices_time_ + get_pre_subgraph_time_+ reduce_graph_time_ + build_index_time_;
        sorted_dfs(src,0);
//        outputPathGraph();
        path_edges_counts_ = st.size();
    }else if(method_type_ == query_method::Join1) {
        get_pre_subgraph();
        build_index();
        preprocess_time_ =
                find_subgraph_vertices_time_ + get_pre_subgraph_time_ + reduce_graph_time_ + build_index_time_;
        join(true);
        path_edges_counts_ = st.size();
    }else if(method_type_ == query_method::Join2){
        get_pre_subgraph();
        get_subgraph();
        sort_adj();
        preprocess_time_ = find_subgraph_vertices_time_ + get_pre_subgraph_time_+ reduce_graph_time_ + build_index_time_;
        join(false);
        path_edges_counts_ = st.size();
    }
    total_path_memory_cost_ = result_count_ * (len_constrain_ + 1) * sizeof (uint32_t);
    auto end = std::chrono::high_resolution_clock::now();
    query_time_ = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
}
//initialize enumerator
void HcEnumerator::init(DirectedGraph * graph){
    graph_ = graph;
    num_vertices_ = graph -> num_vertices_;
    uint64_t size = sizeof(bool) * num_vertices_;
    sign_ = (bool*)malloc(size);
    memset(sign_, false, size);
    visited_ = (bool*)malloc(size);
    memset(visited_, false, size);
}
//initialize enumerator for a group query with same len_constrain
void HcEnumerator::init(query_method method_type, uint8_t len_constrain){
    method_type_ = method_type;
    len_constrain_ = len_constrain;
    uint64_t size = sizeof(uint32_t) * (len_constrain_ + 1);
    stack_ = (uint32_t*)malloc(size);
    memset(stack_, 0, size);

    size = sizeof(std::pair<uint8_t, uint8_t>) * num_vertices_;
    distance_ = (std::pair<uint8_t, uint8_t>*) malloc(size);
    memset((uint8_t*)distance_, len_constrain, size);

    result_count_ = 0;
    accessed_edges_ = 0;
    concat_path_count_ = 0;
    left_path_count_ = 0;
    right_path_count_ = 0;
    subgraph_vertices_count_ = 0;
    subgraph_edges_count_ = 0;
    path_edges_counts_ = 0;
    index_memory_cost_  = 0;
    partial_path_memory_cost_ = 0;
    total_path_memory_cost_ = 0;
    middle_ = 0;

    query_time_ = 0;
    preprocess_time_ = 0;
    bfs_time_ = 0;
    rev_bfs_time_ = 0;
    find_subgraph_vertices_time_ = 0;
    find_subgraph_edges_time_ = 0;
    pre_estimate_time_ = 0;
    build_index_time_ = 0;
    left_dfs_time_ = 0;
    right_dfs_time_ = 0;
    concat_path_time_ = 0;
    join_time_ = 0;
    reduce_graph_time_ = 0;
    get_pre_subgraph_time_ = 0;
}
void HcEnumerator::reset_for_next_batch_query() {//prepare for next batch query
    free(stack_);
    stack_ = nullptr;
    free(distance_);
    distance_ = nullptr;
}
void HcEnumerator::reset_for_next_single_query() {//prepare for next query pair

    /*====================fixed size reuse structure=======================*/
    uint64_t size = sizeof(uint32_t) * (len_constrain_ + 1);
    memset(stack_, 0, size);

    size = sizeof(bool) * num_vertices_;
    memset(sign_, false, size);
    memset(visited_, false, size);

    size = sizeof(std::pair<uint8_t, uint8_t>) * num_vertices_;
    memset((uint8_t*) distance_, len_constrain_, size);

    /*====================dynamic size reuse structure=======================*/

    free(helper_offset_);
    helper_offset_ = nullptr;

    free(rev_helper_offset_);
    rev_helper_offset_ = nullptr;

    free(csr_adj_);
    csr_adj_ = nullptr;

    free(rev_csr_adj_);
    rev_csr_adj_ = nullptr;

    s_hash_.clear();
    rev_s_hash_.clear();

    subgraph_vertices.clear();
    pre_subgraph.clear();
    subgraph.clear();
    sorted_subgraph.clear();
    st.clear();
    subgraph_degree.clear();
    left_paths.clear();
    right_paths.clear();
    paths.clear();
    middle_vertices.clear();
    middle_vertices_map.clear();

    result_count_ = 0;
    accessed_edges_ = 0;
    concat_path_count_ = 0;
    subgraph_vertices_count_ = 0;
    subgraph_edges_count_ = 0;
    path_edges_counts_ = 0;
    left_path_count_ = 0;
    right_path_count_ = 0;
    index_memory_cost_  = 0;
    partial_path_memory_cost_ = 0;
    total_path_memory_cost_ = 0;
    middle_ = 0;
}
void HcEnumerator::update_counter() {
    result_count_arr.emplace_back(result_count_);
    accessed_edges_arr.emplace_back(accessed_edges_);
    left_path_count_arr.emplace_back(left_path_count_);
    right_path_count_arr.emplace_back(right_path_count_);
    concat_path_count_arr.emplace_back(concat_path_count_);
    subgraph_vertices_count_arr.emplace_back(subgraph_vertices_count_);
    subgraph_edges_count_arr.emplace_back(subgraph_edges_count_);
    path_edges_count_arr.emplace_back(path_edges_counts_);
    index_memory_cost_arr.emplace_back(index_memory_cost_);
    partial_path_memory_cost_arr.emplace_back(partial_path_memory_cost_);
    total_path_memory_cost_arr.emplace_back(total_path_memory_cost_);
    bfs_time_arr.emplace_back(bfs_time_);
    rev_bfs_time_arr.emplace_back(rev_bfs_time_);
    find_subgraph_vertices_time_arr.emplace_back(find_subgraph_vertices_time_);
    find_subgraph_edges_time_arr.emplace_back(find_subgraph_edges_time_);
    pre_estimate_time_arr.emplace_back(pre_estimate_time_);
    build_index_time_arr.emplace_back(build_index_time_);
    preprocess_time_arr.emplace_back(preprocess_time_);
    query_time_arr.emplace_back(query_time_);
    left_dfs_time_arr.emplace_back(left_dfs_time_);
    right_dfs_time_arr.emplace_back(right_dfs_time_);
    concat_path_time_arr.emplace_back(concat_path_time_);
    join_time_arr.emplace_back(join_time_);
    reduce_graph_time_arr.emplace_back(reduce_graph_time_);
    get_pre_subgraph_time_arr.emplace_back(get_pre_subgraph_time_);
}
void HcEnumerator::reset_counter() {
    result_count_arr.clear();
    accessed_edges_arr.clear();
    left_path_count_arr.clear();
    right_path_count_arr.clear();
    concat_path_count_arr.clear();
    subgraph_vertices_count_arr.clear();
    subgraph_edges_count_arr.clear();
    path_edges_count_arr.clear();
    index_memory_cost_arr.clear();
    partial_path_memory_cost_arr.clear();
    total_path_memory_cost_arr.clear();

    bfs_time_arr.clear();
    rev_bfs_time_arr.clear();
    find_subgraph_vertices_time_arr.clear();
    find_subgraph_edges_time_arr.clear();
    pre_estimate_time_arr.clear();
    eliminate_edges_time_arr.clear();
    build_index_time_arr.clear();
    preprocess_time_arr.clear();
    query_time_arr.clear();
    left_dfs_time_arr.clear();
    right_dfs_time_arr.clear();
    concat_path_time_arr.clear();
    join_time_arr.clear();
    reduce_graph_time_arr.clear();
    get_pre_subgraph_time_arr.clear();
}
void HcEnumerator::clear() {
    free(sign_);
    sign_ = nullptr;
    free(visited_);
    visited_ = nullptr;
}
void HcEnumerator::get_pre_subgraph() {
    auto bfs_start = std::chrono::high_resolution_clock::now();
    uint8_t dis_s2, dis_t2;
    // bfs from src
    std::queue<uint32_t> q;
    q.push(src_);
    distance_[src_].first = 0;
    uint8_t len = 0;
    while (!q.empty()) {
        uint32_t batch_size = q.size();
        while (batch_size--){
            uint32_t v = q.front();
            q.pop();
            if(len + 1 < len_constrain_){ //
                auto out_neighbors = graph_->out_neighbors(v);
                for(uint32_t i = 0; i < out_neighbors.second; ++i) {
                    uint32_t vv = out_neighbors.first[i];
                    if(vv==src_ || vv == dst_)//exclude src and dst
                        continue;
                    dis_s2= distance_[vv].first;
                    if (dis_s2 == len_constrain_) {//never accessed
                        distance_[vv].first = len + 1;
                        q.push(vv);
                    }
                }
            }
        }
        len ++;
    }
    len = 0;
    auto rev_bfs_start = std::chrono::high_resolution_clock::now();
    bfs_time_= std::chrono::duration_cast<std::chrono::nanoseconds>(rev_bfs_start - bfs_start).count();
    q.push(dst_);
    distance_[dst_].second = 0;
    subgraph_vertices.push_back(src_);
    subgraph_vertices_count_++;//include src
    while(!q.empty()){
        uint32_t batch_size = q.size();
        while (batch_size--){
            uint32_t v = q.front();
            q.pop();
            if(len < len_constrain_ - 1){ //
                auto in_neighbors = graph_->in_neighbors(v);
                for(uint32_t i = 0; i < in_neighbors.second; ++i) {
                    uint32_t vv = in_neighbors.first[i];
                    if(vv==src_ || vv == dst_)//exclude src and dst
                        continue;
                    dis_s2 = distance_[vv].first;
                    if(dis_s2 + len < len_constrain_){//prune
                        if (distance_[vv].second == len_constrain_) {//never accessed
                            distance_[vv].second = len + 1;
                            q.push(vv);
                            sign_[vv] = true;
                            subgraph_vertices_count_++;
                            subgraph_vertices.push_back(vv);
                        }
                    }
                }
            }
        }
        len++;
    }
    auto rev_bfs_end = std::chrono::high_resolution_clock::now();
    rev_bfs_time_= std::chrono::duration_cast<std::chrono::nanoseconds>(rev_bfs_end - rev_bfs_start).count();
    find_subgraph_vertices_time_ = bfs_time_+rev_bfs_time_;
    subgraph_vertices_count_++;//include dst
    sign_[dst_]= true;
    pre_subgraph.resize(num_vertices_);
    for(auto & v: subgraph_vertices){
        auto nbs = graph_->out_neighbors(v);
        for(uint32_t i=0;i<nbs.second;i++){
            uint32_t vv = nbs.first[i];
            if(sign_[vv] && distance_[v].first+distance_[vv].second<len_constrain_){
                pre_subgraph[v].insert(vv);
                subgraph_degree[v].first++;//出度
                subgraph_degree[vv].second++;//入度
                subgraph_edges_count_++;
            }
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    get_pre_subgraph_time_ = std::chrono::duration_cast<std::chrono::nanoseconds>(end - rev_bfs_end).count();
}
void HcEnumerator::get_subgraph() {
    auto start = std::chrono::high_resolution_clock::now();
    subgraph.resize(num_vertices_);
    uint32_t idx = 0;
    for(auto & u : subgraph_vertices){
        if(u!=src_ && subgraph_degree[u].first==1 && subgraph_degree[u].second==1){
            uint64_t v = *pre_subgraph[u].begin();
            if(pre_subgraph[v].count(u)){
                sign_[u] = false;
                subgraph_vertices_count_--;
                continue;
            }
        }
        subgraph_vertices[idx++] = u;
    }
    subgraph_vertices.resize(idx);
    subgraph_edges_count_ = 0;
    for(auto & u : subgraph_vertices){
        for(auto & v: pre_subgraph[u]){
            if(sign_[v]){
                subgraph_edges_count_++;
                subgraph[u].push_back(v);
            }
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    reduce_graph_time_ = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
}
void HcEnumerator::outputPreSubgraph(){
    std::string outfile = "./preSubGraph.txt";
    std::ofstream outs(outfile,std::ios::app);
    outs<<"\n";
    for(auto & u : subgraph_vertices){
        outs<<u<<" ";
        for(auto & v: pre_subgraph[u]){
            outs<<v<<" ";
        }
        outs<<"\n";
    }
    outs.close();
}
void HcEnumerator::outputSubgraph(){
    std::string outfile = "./subGraph.txt";
    std::ofstream outs(outfile,std::ios::app);
    outs<<"\n";
    for(int i=0;i<num_vertices_;i++){
        if(!subgraph[i].empty()){
            outs<<i<<" ";
            for(auto &v: subgraph[i]){
                outs<<v<<" ";
            }
            outs<<"\n";
        }
    }
    outs.close();
}
void HcEnumerator::outputPathGraph(){
    std::string outfile = "./pathGraph.txt";
    std::ofstream outs(outfile,std::ios::app);
    outs<<"\n";
    for (auto it = st.cbegin(); it != st.cend(); it++){
        outs<< (*it).first<<" "<< (*it).second<<"\n";
    }
    outs.close();
}
void HcEnumerator::build_index() {
    auto build_index_start = std::chrono::high_resolution_clock::now();
    std::vector<uint32_t> tmp_flat_adj;
    uint32_t subgraph_vertices_count = subgraph_vertices.size();
    tmp_flat_adj.reserve(subgraph_vertices_count * 10);

    helper_offset_ = (uint32_t*) malloc (sizeof(uint32_t) * len_constrain_ * (subgraph_vertices_count+1));
    std::vector<std::vector<uint32_t>> temp_adj(len_constrain_);
    uint32_t offset = 0;
    for(uint32_t i=0; i < subgraph_vertices_count; ++i){
        offset = i*len_constrain_;
        uint32_t v = subgraph_vertices[i];
        s_hash_[v] = offset;
        auto out_nb = graph_->out_neighbors(v);
        for(uint32_t j=0; j<out_nb.second;++j){
            uint32_t vv = out_nb.first[j];
            if(sign_[vv]){//if vv belongs to actives or dst
                temp_adj[distance_[vv].second].emplace_back(vv);
            }
        }
        uint32_t local_degree = 0;
        for(uint8_t j=0; j<len_constrain_;++j){
            helper_offset_[offset+j] = tmp_flat_adj.size();
            tmp_flat_adj.insert(tmp_flat_adj.end(),temp_adj[j].begin(),temp_adj[j].end());
            local_degree += temp_adj[j].size();
            temp_adj[j].clear();
        }
    }
    offset += len_constrain_;
    helper_offset_[offset] = tmp_flat_adj.size();
    csr_adj_ = (uint32_t *)malloc(sizeof(uint32_t) * tmp_flat_adj.size());
    memcpy(csr_adj_,tmp_flat_adj.data(),sizeof(uint32_t) * tmp_flat_adj.size());
    auto build_index_end = std::chrono::high_resolution_clock::now();
    build_index_time_ = std::chrono::duration_cast<std::chrono::nanoseconds>(build_index_end-build_index_start).count();
    //calculate index memory cost
    //csr
    index_memory_cost_ += tmp_flat_adj.size() * 2 * sizeof(uint32_t);
    //help_offset
    index_memory_cost_ += sizeof(uint32_t) * len_constrain_ * (subgraph_vertices_count_+1) * 2;
    //hash_set
    index_memory_cost_ += sizeof(uint32_t) * subgraph_vertices_count_ * 2;
    tmp_flat_adj.clear();
}
void HcEnumerator::build_index_opt() {
    auto build_index_start = std::chrono::high_resolution_clock::now();
    std::vector<uint32_t> tmp_flat_adj;
    uint32_t subgraph_vertices_count = subgraph_vertices.size();
    tmp_flat_adj.reserve(subgraph_vertices_count * 10);

    helper_offset_ = (uint32_t*) malloc (sizeof(uint32_t) * len_constrain_ * (subgraph_vertices_count+1));
    std::vector<std::vector<uint32_t>> temp_adj(len_constrain_);
    uint32_t offset = 0;
    for(uint32_t i=0; i < subgraph_vertices_count; ++i){
        offset = i*len_constrain_;
        uint32_t v = subgraph_vertices[i];
        s_hash_[v] = offset;
        for(auto vv: subgraph[v]){
            temp_adj[distance_[vv].second].push_back(vv);
        }
        uint32_t local_degree = 0;
        for(uint8_t j=0; j<len_constrain_;++j){
            helper_offset_[offset+j] = tmp_flat_adj.size();
            tmp_flat_adj.insert(tmp_flat_adj.end(),temp_adj[j].begin(),temp_adj[j].end());
            local_degree += temp_adj[j].size();
            temp_adj[j].clear();
        }
    }
    offset += len_constrain_;
    helper_offset_[offset] = tmp_flat_adj.size();
    //calculate index memory cost
    //csr
    csr_adj_ = (uint32_t *)malloc(sizeof(uint32_t) * tmp_flat_adj.size());
    memcpy(csr_adj_,tmp_flat_adj.data(),sizeof(uint32_t) * tmp_flat_adj.size());
    auto build_index_end = std::chrono::high_resolution_clock::now();
    build_index_time_ = std::chrono::duration_cast<std::chrono::nanoseconds>(build_index_end-build_index_start).count();
    //calculate index memory cost
    //csr
    index_memory_cost_ += tmp_flat_adj.size() * 2 * sizeof(uint32_t);
    //help_offset
    index_memory_cost_ += sizeof(uint32_t) * len_constrain_ * (subgraph_vertices_count_+1) * 2;
    //hash_set
    index_memory_cost_ += sizeof(uint32_t) * subgraph_vertices_count_ * 2;
    tmp_flat_adj.clear();
}
void HcEnumerator::sort_adj() {
    auto build_index_start = std::chrono::high_resolution_clock::now();
    std::vector<std::vector<uint32_t>> temp_adj(len_constrain_);
    sorted_subgraph.resize(num_vertices_);
    for(auto & v : subgraph_vertices){
        for(auto & vv: subgraph[v]){
            temp_adj[distance_[vv].second].emplace_back(vv);
        }
        std::vector<uint32_t> tmp;
        for(uint8_t j=0; j<len_constrain_;++j){
            tmp.insert(tmp.end(),temp_adj[j].begin(),temp_adj[j].end());
            temp_adj[j].clear();
        }
        sorted_subgraph[v] = tmp;
    }
    auto build_index_end = std::chrono::high_resolution_clock::now();
    build_index_time_ = std::chrono::duration_cast<std::chrono::nanoseconds>(build_index_end-build_index_start).count();
}
void HcEnumerator::simple_dfs(uint32_t u, uint8_t k) {
    stack_[k] = u;
    visited_[u] = true;
    auto pr = graph_->out_neighbors(u);
    for(uint32_t i = 0; i<pr.second; ++i) {
        if(g_exit) goto EXIT;
        uint32_t vv = pr.first[i];
        if(!visited_[vv] && sign_[vv] && distance_[u].first+distance_[vv].second < len_constrain_) {
            accessed_edges_ ++;
            if(vv == dst_) {
                stack_[k+1] = dst_;
                result_count_++;
            }else {
                if(k + 1 < len_constrain_)
                    simple_dfs(vv, k + 1);
            }
        }
    }
    EXIT:
    visited_[u] = false;
}
void HcEnumerator::dfs_by_index(uint32_t u, uint8_t k) {
    stack_[k] = u;
    visited_[u] = true;
    uint32_t pointer = s_hash_[u];
    uint32_t start  = helper_offset_[pointer];
    uint32_t end = helper_offset_[pointer + len_constrain_ - k];
    for(uint32_t i = start; i<end; ++i) {
        if(g_exit) goto EXIT;
        uint32_t v = csr_adj_[i];
        if(v == dst_){
            accessed_edges_ ++;
            stack_[k+1] = dst_;
            result_count_++;
//            for(uint8_t j=0;j<k+1;j++){
//                st.insert(std::make_pair(stack_[j],stack_[j+1]));
//            }
        }else if(k == len_constrain_-2 && !visited_[v]){
            accessed_edges_ ++;
            stack_[k+1] = v;
            stack_[k+2] = dst_;
            result_count_++;
//            for(uint8_t j=0;j<k+2;j++){
//                st.insert(std::make_pair(stack_[j],stack_[j+1]));
//            }
        }else if(!visited_[v]) {
            accessed_edges_ ++;
            dfs_by_index(v, k + 1);
        }
    }
    EXIT:
    visited_[u] = false;
}
void HcEnumerator::sorted_dfs(uint32_t u, uint8_t k) {
    stack_[k] = u;
    visited_[u] = true;
    for(auto & v : sorted_subgraph[u]){
        if(g_exit) goto EXIT;
        if( k+distance_[v].second >= len_constrain_) break;
        if(v == dst_){
            accessed_edges_ ++;
            stack_[k+1] = dst_;
            result_count_++;
//            for(uint8_t j=0;j<k+1;j++){
//                st.insert(std::make_pair(stack_[j],stack_[j+1]));
//            }
//            std::vector<uint32_t> tmp;
//            for(uint8_t i=0;i<k+1;i++){
//                tmp.push_back(stack_[i]);
//            }
//            paths.emplace_back(tmp);
        }else if(k == len_constrain_-2 && !visited_[v]){
            accessed_edges_ ++;
            stack_[k+1] = v;
            stack_[k+2] = dst_;
            result_count_++;
//            for(uint8_t j=0;j<k+2;j++){
//                st.insert(std::make_pair(stack_[j],stack_[j+1]));
//            }
//            std::vector<uint32_t> tmp;
//            for(uint8_t i=0;i<=len_constrain_;i++){
//                tmp.push_back(stack_[i]);
//            }
//            paths.emplace_back(tmp);
        }else if(!visited_[v]) {
            accessed_edges_ ++;
            sorted_dfs(v, k + 1);
        }
    }
    EXIT:
    visited_[u] = false;
}
void HcEnumerator::join(bool type) {
    auto start = std::chrono::high_resolution_clock::now();
    middle_ = len_constrain_/2;
//    left_paths.reserve(1024 * (uint32_t)pow(2,len_constrain_));
//    right_paths.reserve(1024 * (uint32_t)pow(2,len_constrain_));
    if(type){
        left_dfs_by_index(src_,0);
    }else{
        left_dfs(src_,0);
    }
    left_path_count_ = left_paths.size();
    auto left_dfs_end = std::chrono::high_resolution_clock::now();
    left_dfs_time_ = std::chrono::duration_cast<std::chrono::nanoseconds>(left_dfs_end-start).count();
    for(auto & u: middle_vertices){
        uint64_t l = right_paths.size();
        if(type){
            right_dfs_by_index(u,middle_);
        }else{
            right_dfs(u,middle_);
        }
        uint64_t r = right_paths.size();
        middle_vertices_map[u] = std::make_pair(l,r);
    }
    right_path_count_ = right_paths.size();
    auto right_dfs_end = std::chrono::high_resolution_clock::now();
    right_dfs_time_ = std::chrono::duration_cast<std::chrono::nanoseconds>(right_dfs_end-left_dfs_end).count();
    for(auto & l_path: left_paths){
        for(auto & v: l_path){
            visited_[v] = true;
        }
        uint32_t mid_vertex = l_path.back();
        for(uint64_t i=middle_vertices_map[mid_vertex].first;i<middle_vertices_map[mid_vertex].second;i++){
            bool sign = false;
            for(auto & v : right_paths[i]){
                if(visited_[v]){
                    sign = true;
                    break;
                }
            }
            if(!sign){
                concat_path_count_++;
                result_count_++;
//                std::vector<uint32_t> tmp;
//                tmp.insert(tmp.end(),l_path.begin(),l_path.end());
//                tmp.insert(tmp.end(),right_paths[i].begin(),right_paths[i].end());
//                paths.emplace_back(tmp);
            }
        }
        for(auto & v: l_path){
            visited_[v] = false;
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    concat_path_time_ = std::chrono::duration_cast<std::chrono::nanoseconds>(end-right_dfs_end).count();
    join_time_ = std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count();
    partial_path_memory_cost_ = left_path_count_*(middle_+1) + right_path_count_*(len_constrain_-middle_);
    partial_path_memory_cost_ *= sizeof (uint32_t);
}
void HcEnumerator::left_dfs(uint32_t u, uint8_t k) {
    stack_[k] = u;
    visited_[u] = true;
    for(auto & v : sorted_subgraph[u]){
        if(g_exit) goto EXIT;
        if( k+distance_[v].second >= len_constrain_) break;
        if(visited_[v]) continue;
        accessed_edges_ ++;
        if(v == dst_){
            stack_[k+1] = dst_;
            result_count_++;
        }else if(k == middle_-1){
            stack_[middle_] = v;
            middle_vertices.insert(v);
            std::vector<uint32_t> tmp(middle_+1);
            for(uint8_t i =0;i<=middle_;i++){
                tmp[i] = stack_[i];
            }
            left_paths.emplace_back(tmp);
        }else{
            left_dfs(v, k + 1);
        }
    }
    EXIT:
    visited_[u] = false;
}
void HcEnumerator::left_dfs_by_index(uint32_t u, uint8_t k) {
    stack_[k] = u;
    visited_[u] = true;
    uint32_t pointer = s_hash_[u];
    uint32_t start  = helper_offset_[pointer];
    uint32_t end = helper_offset_[pointer + len_constrain_ - k];
    for(uint32_t i = start; i<end; ++i) {
        if(g_exit) goto EXIT;
        uint32_t v = csr_adj_[i];
        if(visited_[v]) continue;
        accessed_edges_ ++;
        if(v == dst_){
            stack_[k+1] = dst_;
            result_count_++;
//            std::vector<uint32_t> tmp;
//            for(uint8_t j=0;j<=k+1;j++){
//                tmp.push_back(stack_[j]);
//            }
//            paths.emplace_back(tmp);
        }else if(k == middle_-1){
            stack_[middle_] = v;
            middle_vertices.insert(v);
            std::vector<uint32_t> tmp(middle_+1);
            for(uint8_t j =0;j<=middle_;j++){
                tmp[j] = stack_[j];
            }
            left_paths.emplace_back(tmp);
        }else{
            left_dfs_by_index(v, k + 1);
        }
    }
    EXIT:
    visited_[u] = false;
}
void HcEnumerator::right_dfs(uint32_t u, uint8_t k) {
    stack_[k] = u;
    visited_[u] = true;
    for(auto & v : sorted_subgraph[u]){
        if(g_exit) goto EXIT;
        if( k+distance_[v].second >= len_constrain_) break;
        if(v == dst_){
            accessed_edges_ ++;
            stack_[k+1] = dst_;
            std::vector<uint32_t> tmp(k+1-middle_);
            for(uint8_t i =middle_+1, j=0;i<=k+1;i++,j++){
                tmp[j] = stack_[i];
            }
            right_paths.emplace_back(tmp);
        }else if(k == len_constrain_-2 && !visited_[v]){
            accessed_edges_ ++;
            stack_[k+1] = v;
            stack_[k+2] = dst_;
            std::vector<uint32_t> tmp(len_constrain_-middle_);
            for(uint8_t i =middle_+1, j=0;i<=len_constrain_;i++, j++){
                tmp[j] = stack_[i];
            }
            right_paths.emplace_back(tmp);
        }else if(!visited_[v]) {
            accessed_edges_ ++;
            right_dfs(v, k + 1);
        }
    }
    EXIT:
    visited_[u] = false;
}
void HcEnumerator::right_dfs_by_index(uint32_t u, uint8_t k) {
    stack_[k] = u;
    visited_[u] = true;
    uint32_t pointer = s_hash_[u];
    uint32_t start  = helper_offset_[pointer];
    uint32_t end = helper_offset_[pointer + len_constrain_ - k];
    for(uint32_t i = start; i<end; ++i) {
        if(g_exit) goto EXIT;
        uint32_t v = csr_adj_[i];
        if(v == dst_){
            accessed_edges_ ++;
            stack_[k+1] = dst_;
            std::vector<uint32_t> tmp(k+1-middle_);
            for(uint8_t idx =0, j =middle_+1;j<=k+1; idx++,j++){
                tmp[idx] = stack_[j];
            }
            right_paths.emplace_back(tmp);
        }else if(k == len_constrain_-2 && !visited_[v]){
            accessed_edges_ ++;
            stack_[k+1] = v;
            stack_[k+2] = dst_;
            std::vector<uint32_t> tmp(len_constrain_-middle_);
            for(uint8_t idx=0, j =middle_+1;j<=len_constrain_;idx++,j++){
                tmp[idx] = stack_[j];
            }
            right_paths.emplace_back(tmp);
        }else if(!visited_[v]) {
            accessed_edges_ ++;
            right_dfs_by_index(v, k + 1);
        }
    }
    EXIT:
    visited_[u] = false;
}
void HcEnumerator::outputPaths(bool type){
    std::string outfile = "./";
    if(type){
        outfile += "original_path.txt";
    }else{
        outfile += "join_path.txt";
    }
    std::ofstream outs(outfile,std::ios::app);
    std::sort(paths.begin(),paths.end());
    for(auto & path : paths){
        for(auto & v: path){
            outs<<v<<" ";
        }
        outs<<"\n";
    }
    outs.close();
}