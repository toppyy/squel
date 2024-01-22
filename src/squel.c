#include "./include/utils.h"
#include "./include/parser.h"
#include "./include/parsetree.h"


void printTree(Node *node) {


    printf("Node with type %d and content %s.\n", node->type, node->content);

    if (node->child != NULL) {
        printf("Children: \n");
        printTree(node->child);
        printf("Children done \n");
    }

    if (node->next != NULL) {
        printTree(node->next);
    }
}

int main(int argc, char* argv[]) {

    if (argc == 1) {
        printf("Need that SQL!\n");
        exit(1);
    }

    if (strlen(argv[1]) >= MAXQUERYSIZE) {
        printf("Query length exceeds maximum :/\n");
        exit(1);
    }

    
    Node *nodes = NULL;
    nodes = (Node*) malloc(sizeof(Node) * NODEBUFFSIZE);
    Node **p_nodes = &nodes;
    size_t nodeCount = parse(argv[1], p_nodes);
    
    printf("Nodes %ld\n", nodeCount);
    
    
    for (size_t i = 0; i < nodeCount; i++) {
        printf("Node type %d, content %s\n",nodes[i].type, nodes[i].content);
    }
        
    free(nodes);

}