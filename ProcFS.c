/*
 * Echipa paleta
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

struct process_node {
    pid_t pid;
    pid_t parent_pid;
    int number_of_children;
    pid_t *children_pids;
    uid_t user_id;
    segsz_t stack_size;
};

void dfs(pid_t pid, pid_t parent_pid){
    //Get the number of children
    int number_of_children = syscall(331, pid);
    if (number_of_children < 0){
        return;
    }

    //Allocate memory
    struct process_node *p = (struct process_node*) malloc(sizeof(struct process_node));
    p->children_pids = (pid_t*) malloc(sizeof(pid_t) * number_of_children);

    //Get stats
    p->pid = pid;
    p->parent_pid = parent_pid;
    p->number_of_children = number_of_children;
    int ok = syscall(332, pid, p);

    if (ok < 0){
        printf("There was an error while getting the stats of the process with pid %d!", pid);
        return;
    }

    printf("pid: %d\n", p->pid);
    printf("ppid: %d\n", p->parent_pid);
    printf("number_of_children: %d\n", p->number_of_children);
    printf("user_id: %d\n", (int)p->user_id);
    printf("stack_size: %d\n", p->stack_size);
    printf("\n");

    //Dfs forward
    int i;
    for (i = 0; i < number_of_children; i++) {
        dfs(p->children_pids[i], pid);
    }

    //Deallocate memory
    free(p->children_pids);
    free(p);
}

int main(int argc, char **argv) {
    pid_t starting_pid = atoi(argv[1]);

    int checker = syscall(331, starting_pid);

    if (checker < 0) {
        printf("The given pid does not correspond to any process.\n");
    } 
    else {
        printf("Starting DFS on the process tree...%d\n\n", checker);
        dfs(starting_pid, -1);
    }
    return 0;
}