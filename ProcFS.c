/*
 * Anghelina Ion Marian, Enache Alexandru, Voicu Bianca Oana
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

struct process_node {
    pid_t pid;
    pid_t parent_pid;
    int number_of_children;
    pid_t *children_pids;
    uid_t user_id;
    segsz_t stack_size;
};

void dfs(pid_t pid, pid_t parent_pid, char* path){
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

    //Create new path
    char* new_path = malloc(strlen(path) + 100);
    strcpy(new_path, path);
    strcat(new_path, "/proc");
    char* string_pid = malloc(100);
    int int_pid = p->pid;
    sprintf(string_pid, "%d", int_pid);
    strcat(new_path, string_pid);

    //Create folder
    int check;
    check = mkdir(new_path,0777);
    if (!check)
        printf("Directory created for process with pid %d\n", pid);
    else {
        printf("Unable to create directory\n");
        return;
    }

    //Put statistics
    char* statistics_path = malloc(strlen(new_path) + 100);
    strcpy(statistics_path, new_path);
    strcat(statistics_path, "/statistics.txt");
    int fd = open(statistics_path, O_CREAT | O_RDWR, 0777);
	if (fd < 0){
		printf("Error opening file err=%d\n", errno);
		return;
	}

    char* buffer = malloc(100000);
    sprintf(buffer, "path: %s\n", new_path);
    sprintf(buffer + strlen(buffer), "pid: %d\n", p->pid);
    sprintf(buffer + strlen(buffer), "ppid: %d\n", p->parent_pid);
    sprintf(buffer + strlen(buffer), "number_of_children: %d\n", p->number_of_children);
    sprintf(buffer + strlen(buffer), "user_id: %d\n", (int)p->user_id);
    sprintf(buffer + strlen(buffer), "stack_size: %d\n", p->stack_size);
	write(fd, buffer, strlen(buffer));
    free(buffer);
    free(statistics_path);

    printf("Statistics created for process with pid %d\n", pid);

    //Dfs forward
    int i;
    for (i = 0; i < number_of_children; i++) {
        dfs(p->children_pids[i], pid, new_path);
    }

    //Deallocate memory
    free(p->children_pids);
    free(p);
    free(new_path);
}

int main(int argc, char **argv) {
    pid_t starting_pid = atoi(argv[1]);

    int checker = syscall(331, starting_pid);

    if (checker < 0) {
        printf("The given pid does not correspond to any process.\n");
    } 
    else {
        printf("Starting DFS on the process tree...%d\n\n", checker);
        
        mkdir("/home/proc",0777);
        dfs(starting_pid, -1, "/home/proc");
    }
    return 0;
}