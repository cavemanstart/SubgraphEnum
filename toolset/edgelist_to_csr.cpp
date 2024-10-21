//
// Created by Stone on 2023/12/08.
//
#include "util/log/log.h"
#include "util/graph/directed_graph.h"

int main(int argc, char *argv[]) {
    std::string input_graph_folder(argv[1]);
    std::string output_graph_folder(argv[2]);
    char skip_character = '#';
    if (argc > 3) {
        skip_character = argv[3][0];
    }
    log_info("skip character is %c", skip_character);

    DirectedGraph digraph;
    digraph.load_graph(input_graph_folder, skip_character);
    digraph.print_metadata();
    digraph.store_csr(output_graph_folder);
    log_info("done.");
    return 0;
}
