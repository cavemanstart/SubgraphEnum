//
// Created by Stone on 2023/12/09.
//
#include <chrono>
#include <future>
#include <numeric>
#include <iostream>
#include <iomanip>
#include "algorithm"
#include "util/log/log.h"
#include "util/graph/directed_graph.h"
#include "util/io/io.h"
#include "hc_enumerator.h"
bool execute_within_time_limit(HcEnumerator* enumerator, uint32_t src, uint32_t dst,
                               uint64_t time_limit) {
    g_exit = false;
    std::future<void> future = std::async(std::launch::async, [enumerator, src, dst](){
        return enumerator->execute(src, dst);
    });

    std::future_status status;
    do {
        status = future.wait_for(std::chrono::seconds(time_limit));
        if (status == std::future_status::deferred) {
            log_error("Deferred.");
            exit(-1);
        } else if (status == std::future_status::timeout) {
            g_exit = true;
        }
    } while (status != std::future_status::ready);
    return g_exit;
}
std::string get_date_time(time_t timestamp = time(NULL))
{
    char buffer[20] = {0};
    struct tm *info = localtime(&timestamp);
    strftime(buffer, sizeof buffer, "%Y/%m/%d %H:%M:%S", info);
    return std::string(buffer);
}
uint64_t accumulate_counter(std::vector<uint64_t> & counter){
    return std::accumulate(counter.begin(),counter.end(), 0ull);
}
double average_time(uint64_t time, uint32_t num_queries){//ms
    return (double) time / num_queries / 1000000.0 ;
}
void outputEdgeRatio(std::vector<uint64_t>& arr1, std::vector<uint64_t>& arr2){
    std::string outfile = "./edgeRatio.txt";
    std::ofstream outs(outfile,std::ios::app);
    for (uint32_t i=0;i<arr1.size();i++){
        double ratio = (double)arr1[i]/ (double)arr2[i];
        outs<< arr1[i]<<" "<< arr2[i]<< " "<<  std::fixed<<std::setprecision(2)<<ratio<< "\n";
    }
    outs.close();
}
void excutor(HcEnumerator &enumerator, std::vector<std::pair<uint32_t, uint32_t>> & queries ,std::string & dir){
    uint32_t terminated_query_count = 0;
    uint32_t num_queries = queries.size();
    uint32_t per_query_time_limit = 1000;
    log_info("num of queries: %u", num_queries);
    for (uint32_t i = 0; i < num_queries; i++) {
        auto query = queries[i];
        if (execute_within_time_limit(&enumerator, query.first, query.second, per_query_time_limit)) {
            terminated_query_count += 1;
        }
        enumerator.update_counter();
        log_info("src %u, dst %u, num of results: %u",enumerator.src_, enumerator.dst_,enumerator.result_count_);
        enumerator.reset_for_next_single_query();
    }
    std::sort(enumerator.query_time_arr.begin(),enumerator.query_time_arr.end());
    double latency_query_time = (double ) enumerator.query_time_arr[(int)(num_queries*0.99)-1]/ 1000000.0;
    double average_query_time = average_time(accumulate_counter(enumerator.query_time_arr),num_queries) ;
    double average_preprocess_time = average_time(accumulate_counter(enumerator.preprocess_time_arr),num_queries) ;
    double average_enumerate_time = average_query_time - average_preprocess_time;
    double average_join_time = average_time(accumulate_counter(enumerator.join_time_arr),num_queries);
    double average_find_subgraph_vertices_time = average_time( accumulate_counter(enumerator.find_subgraph_vertices_time_arr),num_queries);
    double average_build_index_time = average_time(accumulate_counter(enumerator.build_index_time_arr),num_queries);
    double average_bfs_time = average_time( accumulate_counter(enumerator.bfs_time_arr),num_queries);
    double average_rev_bfs_time = average_time( accumulate_counter(enumerator.rev_bfs_time_arr),num_queries);
    double average_left_dfs_time = average_time( accumulate_counter(enumerator.left_dfs_time_arr),num_queries);
    double average_right_dfs_time = average_time( accumulate_counter(enumerator.right_dfs_time_arr),num_queries);
    double average_concat_path_time = average_time( accumulate_counter(enumerator.concat_path_time_arr),num_queries);

    uint64_t average_subgraph_vertices_count = accumulate_counter(enumerator.subgraph_vertices_count_arr)/num_queries;
    uint64_t average_subgraph_edges_count = accumulate_counter(enumerator.subgraph_edges_count_arr)/num_queries;
    uint64_t average_path_edges_count = accumulate_counter(enumerator.path_edges_count_arr)/num_queries;
    double subgraph_ratio = (double) average_path_edges_count/(double) average_subgraph_edges_count;
    uint64_t average_result_count = accumulate_counter(enumerator.result_count_arr)/num_queries;
    uint64_t average_concat_path_count = accumulate_counter(enumerator.concat_path_count_arr)/num_queries;
    uint64_t average_left_path_count = accumulate_counter(enumerator.left_path_count_arr)/num_queries;
    uint64_t average_right_path_count = accumulate_counter(enumerator.right_path_count_arr)/num_queries;
    uint64_t average_accessed_edges_count = accumulate_counter(enumerator.accessed_edges_arr)/num_queries;
    double average_index_memory_cost = (double )accumulate_counter(enumerator.index_memory_cost_arr)/num_queries/1024/1024;
    double average_partial_path_memory_cost =(double ) accumulate_counter(enumerator.partial_path_memory_cost_arr)/num_queries/1024/1024;
    double average_total_path_memory_cost = (double ) accumulate_counter(enumerator.total_path_memory_cost_arr)/num_queries/1024/1024;
    double average_throughput = (double )average_result_count / average_enumerate_time * 1000;
//    outputEdgeRatio(enumerator.path_edges_count_arr,enumerator.subgraph_edges_count_arr);
    enumerator.reset_counter();
    enumerator.reset_for_next_batch_query();
    log_info("dump into log file...");
    std::string outfile = dir+"_res_log.txt";
    std::ofstream outs(outfile,std::ios::app);
    outs<<"terminated query ratio: "<<std::fixed<<std::setprecision(2) << (double) terminated_query_count/num_queries <<" ms\n";
    outs<<"average query time: "<<std::fixed<<std::setprecision(2) << average_query_time <<" ms\n";
    outs<<"99% latency query time: "<< std::fixed<<std::setprecision(2) <<latency_query_time<<" ms\n";
    outs<<"average preprocess time: "<< std::fixed<<std::setprecision(2) <<average_preprocess_time <<" ms\n";
    outs<<"average enumerate time: "<< std::fixed<<std::setprecision(2) <<average_enumerate_time <<" ms\n";
    outs<<"average find actives time: "<< std::fixed<<std::setprecision(2) <<average_find_subgraph_vertices_time <<" ms\n";
    outs<<"average build index time: "<< std::fixed<<std::setprecision(2) <<average_build_index_time <<" ms\n";
    outs<<"average bfs time: "<< std::fixed<<std::setprecision(2) <<average_bfs_time <<" ms\n";
    outs<<"average reverse bfs time: "<< std::fixed<<std::setprecision(2) <<average_rev_bfs_time <<" ms\n";
    outs<<"average join time: "<< std::fixed<<std::setprecision(2) <<average_join_time <<" ms\n";
    outs<<"average left dfs time: "<< std::fixed<<std::setprecision(2) << average_left_dfs_time <<" ms\n";
    outs<<"average right dfs time: "<< std::fixed<<std::setprecision(2) << average_right_dfs_time <<" ms\n";
    outs<<"average concat path time: "<< std::fixed<<std::setprecision(2) << average_concat_path_time <<" ms\n";
    outs<<"average subgraph vertices count: "<< average_subgraph_vertices_count <<"\n";
    outs<<"average subgraph edges count: "<< average_subgraph_edges_count <<"\n";
    outs<<"average path edges count: "<< average_path_edges_count <<"\n";
    outs<<"subgraph ratio: "<< std::fixed<<std::setprecision(2) << subgraph_ratio <<"\n";
    outs<<"average result count: "<< average_result_count <<"\n";
    outs<<"average concat path count: "<< average_concat_path_count <<"\n";
    outs<<"average left path count: "<< average_left_path_count <<"\n";
    outs<<"average right path count: "<< average_right_path_count <<"\n";
    outs<<"average accessed edge count: "<< average_accessed_edges_count <<"\n";
    outs<<"average index memory cost: "<< std::fixed<<std::setprecision(2)<< average_index_memory_cost <<" mb\n";
    outs<<"average total path memory cost: "<< std::fixed<<std::setprecision(2) << average_total_path_memory_cost <<" mb\n";
    outs<<"average partial path memory cost: "<< std::fixed<<std::setprecision(2) << average_partial_path_memory_cost <<" mb\n";
    outs<<"average throughput: "<< average_throughput <<" per second\n\n";
    outs.close();
}
void output_queries(std::vector<std::pair<uint32_t, uint32_t>> &queries, std::string &dir){
    std::string outfile = dir+"_queries.txt";
    std::ofstream outs(outfile,std::ios::app);
    for (auto pi : queries){
        outs<<pi.first<<" "<<pi.second<<"\n";
    }
    outs.close();
}
int main(int argc, char *argv[]) {
    std::string input_graph_folder(argv[1]);
    std::string dir = input_graph_folder.substr(input_graph_folder.size()-2,2);
    std::string input_query_folder(argv[2]);
    std::string query_file = input_query_folder.substr(input_query_folder.size()-2,2);
    std::vector<HcEnumerator::query_method> methods = {
            HcEnumerator::query_method::PathEnum,
            HcEnumerator::query_method::SortedEnum,
            HcEnumerator::query_method::Join1,
            HcEnumerator::query_method::Join2,
    };
    std::vector<uint8_t> len_constrains = {3,4,5,6,7};
    //init graph
    DirectedGraph digraph;
    digraph.load_csr(input_graph_folder);
    HcEnumerator enumerator;
    enumerator.init(&digraph);
    input_query_folder += "/general_pairs.bin";
    std::vector<std::pair<uint32_t, uint32_t>> queries;
//    queries.emplace_back(693, 19122);
    IO::read(input_query_folder, queries);
//    output_queries(queries,dir);
    for(auto len_constrain: len_constrains){
        for(auto method : methods){
            std::cout << "method: " << method << '\t';
            std::cout << "length constraint: " << std::to_string(len_constrain) << '\t';
            enumerator.init(method,len_constrain);
            std::string outfile = dir+"_res_log.txt";
            std::ofstream outs(outfile,std::ios::app);
            outs<<get_date_time()<<"\n";
            outs<<"dataset: "<<dir<<"\t query file: "<<query_file<<"\t method: "<<method
                <<"\t num queries: " << queries.size() <<"\t len constrain: "<<std::to_string(len_constrain) <<"\n";
            outs.close();
            excutor(enumerator,queries,dir);
        }
    }
    enumerator.clear();
    return 0;
}
