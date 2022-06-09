#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "oslabs.h"

//Function used to initialize NULLPCB
struct PCB NULLPCB() {
    struct PCB nullpcb;
    nullpcb.process_id = 0;
    nullpcb.arrival_timestamp = 0;
    nullpcb.total_bursttime = 0;
    nullpcb.execution_starttime = 0;
    nullpcb.execution_endtime = 0;
    nullpcb.remaining_bursttime = 0;
    nullpcb.process_priority = 0;
    return nullpcb;
};

//Signatures of the functions
struct PCB handle_process_arrival_pp(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, struct PCB current_process,
                                     struct PCB new_process, int timestamp);

struct PCB handle_process_completion_pp(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, int timestamp);

struct PCB handle_process_arrival_srtp(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, struct PCB current_process, 
                                       struct PCB new_process, int time_stamp);

struct PCB handle_process_completion_srtp(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, int timestamp);

struct PCB handle_process_arrival_rr(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, struct PCB current_process,
                                     struct PCB new_process, int timestamp, int time_quantum);

struct PCB handle_process_completion_rr(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, int time_stamp, int time_quantum);

struct PCB* add_item(struct PCB* ready_queue_dyn, struct PCB a, int *queue_cnt);

//Definitions of the above signatures
struct PCB handle_process_arrival_pp(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, struct PCB current_process,
                                     struct PCB new_process, int timestamp)
{
    struct PCB nullpcb = NULLPCB(); //initializes nullpcb
    struct PCB* ready_queue_dyn; //initialize dynamic ready_queue
    int init_queue_cnt = *queue_cnt; //initial queue_cnt
    bool needs_free = false; //bool used for free() logic below
    //If current_process == nullpcb, then new_process is updated and returned
    if(current_process.process_id == nullpcb.process_id && current_process.arrival_timestamp == nullpcb.arrival_timestamp 
        && current_process.total_bursttime == nullpcb.total_bursttime && current_process.execution_starttime == nullpcb.execution_starttime 
        && current_process.execution_endtime == nullpcb.execution_endtime && current_process.remaining_bursttime == nullpcb.remaining_bursttime 
        && current_process.process_priority == nullpcb.process_priority) {
        new_process.execution_starttime = timestamp;
        new_process.execution_endtime = timestamp + new_process.total_bursttime;
        new_process.remaining_bursttime = new_process.total_bursttime;
        return new_process;
    }
    else {
        //if new_process is of a lower or equal priority it is updated and added to the ready_queue and current_process is returned
        if(new_process.process_priority >= current_process.process_priority) {
            new_process.execution_starttime = 0;
            new_process.execution_endtime = 0;
            new_process.remaining_bursttime = new_process.total_bursttime;
            ready_queue_dyn = add_item(ready_queue, new_process, queue_cnt);
            //If update requires more space in array, then dynamically resize using realloc() and copy current queue into ready_queue
            if(init_queue_cnt > *queue_cnt) {
                ready_queue_dyn = (struct PCB*)realloc(ready_queue, sizeof(struct PCB)*(*queue_cnt));
                needs_free = true;
            }
            for(int i = 0; i < *queue_cnt; i++) {
                ready_queue[i] = ready_queue_dyn[i];
            }
            //If realloc() was used, then free() is used
            if(needs_free){
                free(ready_queue_dyn);
            }
            return current_process;
        }
        //if new_process is higher priority it is updated and current_process is updated and added to the ready_queue, new_process is returned
        else {
            new_process.execution_starttime = timestamp;
            new_process.execution_endtime = timestamp + new_process.total_bursttime;
            new_process.remaining_bursttime = new_process.total_bursttime;
            current_process.execution_endtime = 0;
            current_process.remaining_bursttime = current_process.total_bursttime - current_process.arrival_timestamp;
            ready_queue_dyn = add_item(ready_queue, current_process, queue_cnt);
            //If update requires more space in array, then dynamically resize using realloc() and copy current queue into ready_queue
            if(init_queue_cnt > *queue_cnt) {
                ready_queue_dyn = (struct PCB*)realloc(ready_queue, sizeof(struct PCB)*(*queue_cnt));
                needs_free = true;
            }
            for(int i = 0; i < *queue_cnt; i++) {
                ready_queue[i] = ready_queue_dyn[i];
            }
           //If realloc() was used, then free() is used
            if(needs_free){
                free(ready_queue_dyn);
            }
            return new_process;
        }
    }
}

struct PCB handle_process_completion_pp(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, int timestamp) {
    struct PCB nullpcb = NULLPCB(); //initializes nullpcb
    //If ready_queue is empty, return nullpcb
    if(*queue_cnt == 0) {
        return nullpcb;
    }
    else {
        int priority = ready_queue[0].process_priority;
        int pos = 0;
        struct PCB return_PCB;
        //Iterate through the array and determine the highest priority process, save process into return_PCB
        for(int i = 1; i < *queue_cnt; i++) {
            if (priority > ready_queue[i].process_priority) {
                priority = ready_queue[i].process_priority;
                return_PCB = ready_queue[i];
                pos = i;
            }
        }
        //Remove return_PCB process from ready_queue and update queue_cnt
        for(int i = pos; i < *queue_cnt; i++)
        {
            ready_queue[i] = ready_queue[i + 1];
        }
        (*queue_cnt)--;
        //Update the return_PCB and return it
        return_PCB.execution_starttime = timestamp;
        return_PCB.execution_endtime = timestamp + return_PCB.remaining_bursttime;
        return return_PCB;
    }
}

struct PCB handle_process_arrival_srtp(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, struct PCB current_process, 
                                       struct PCB new_process, int time_stamp) {
    struct PCB nullpcb = NULLPCB();
    struct PCB* ready_queue_dyn; //initialize dynamic ready_queue
    int init_queue_cnt = *queue_cnt; //initial queue_cnt
    bool needs_free = false; //bool used for free() logic below
    //If current_process == nullpcb, then new_process is updated and returned
    if(current_process.process_id == nullpcb.process_id && current_process.arrival_timestamp == nullpcb.arrival_timestamp 
        && current_process.total_bursttime == nullpcb.total_bursttime && current_process.execution_starttime == nullpcb.execution_starttime 
        && current_process.execution_endtime == nullpcb.execution_endtime && current_process.remaining_bursttime == nullpcb.remaining_bursttime 
        && current_process.process_priority == nullpcb.process_priority) {
        new_process.execution_starttime = time_stamp;
        new_process.execution_endtime = time_stamp + new_process.total_bursttime;
        new_process.remaining_bursttime = new_process.total_bursttime;
        return new_process;
    }
    else {
        //If remaining_bursttime of new process is greater than or equal to current_process, then update new_process, add it to the ready_queue,
        //and return the current process
        if(new_process.remaining_bursttime >= current_process.remaining_bursttime) {
            new_process.execution_starttime = 0;
            new_process.execution_endtime = 0;
            new_process.remaining_bursttime = new_process.total_bursttime;
            ready_queue_dyn = add_item(ready_queue, new_process, queue_cnt);
            //If update requires more space in array, then dynamically resize using realloc() and copy current queue into ready_queue
            if(init_queue_cnt > *queue_cnt) {
                ready_queue_dyn = (struct PCB*)realloc(ready_queue, sizeof(struct PCB)*(*queue_cnt));
                needs_free = true;
            }
            for(int i = 0; i < *queue_cnt; i++) {
                ready_queue[i] = ready_queue_dyn[i];
            }
            //If realloc() was used, then free() is used
            if(needs_free){
                free(ready_queue_dyn);
            }
            return current_process;
        }
        else {
            //Else, update new_process and current_process, add current_process to ready_queue, and return new_process
            new_process.execution_starttime = time_stamp;
            new_process.execution_endtime = time_stamp + new_process.total_bursttime;
            new_process.remaining_bursttime = new_process.total_bursttime;
            current_process.execution_starttime = 0;
            current_process.execution_endtime = 0;
            current_process.remaining_bursttime = current_process.total_bursttime - 1;
            ready_queue_dyn = add_item(ready_queue, current_process, queue_cnt);
            //If update requires more space in array, then dynamically resize using realloc() and copy current queue into ready_queue
            if(init_queue_cnt > *queue_cnt) {
                ready_queue_dyn = (struct PCB*)realloc(ready_queue, sizeof(struct PCB)*(*queue_cnt));
                needs_free = true;
            }
            for(int i = 0; i < *queue_cnt; i++) {
                ready_queue[i] = ready_queue_dyn[i];
            }
           //If realloc() was used, then free() is used
            if(needs_free){
                free(ready_queue_dyn);
            }
            return new_process;
        }
    }
}

struct PCB handle_process_completion_srtp(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, int timestamp) {
    struct PCB nullpcb = NULLPCB();//Initilizes the nullpacb
    //If ready_queue is empty, return nullpcb
    if(*queue_cnt == 0) {
        return nullpcb;
    }
    else {
        int shortest_burst = ready_queue[0].remaining_bursttime;
        int pos = 0;
        struct PCB return_PCB;
        //Iterate through the array and determine the shortest bursttime process, save process into return_PCB
        for(int i = 0; i < *queue_cnt; i++) {
            if (shortest_burst >= ready_queue[i].remaining_bursttime && ready_queue[i].remaining_bursttime > 0) {
                shortest_burst = ready_queue[i].remaining_bursttime;
                return_PCB = ready_queue[i];
                pos = i;
            }
        }
        //Remove return_PCB process from ready_queue and update queue_cnt
        for(int i = pos; i < *queue_cnt; i++)
        {
            ready_queue[i] = ready_queue[i + 1];
        }
        (*queue_cnt)--;
        //Update the return_PCB and return it
        return_PCB.execution_starttime = timestamp;
        return_PCB.execution_endtime = timestamp + return_PCB.remaining_bursttime;
        return return_PCB;
    }
}

struct PCB handle_process_arrival_rr(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, struct PCB current_process,
                                     struct PCB new_process, int timestamp, int time_quantum){
    struct PCB nullpcb = NULLPCB();
    struct PCB* ready_queue_dyn; //initialize dynamic ready_queue
    int init_queue_cnt = *queue_cnt; //initial queue_cnt
    bool needs_free = false; //bool used for free() logic below
    int burst_or_quantum = 0; //used to determine shortest between total bursttime and time quantum
    //If current_process == nullpcb, then new_process is updated and returned
    if(current_process.process_id == nullpcb.process_id && current_process.arrival_timestamp == nullpcb.arrival_timestamp 
        && current_process.total_bursttime == nullpcb.total_bursttime && current_process.execution_starttime == nullpcb.execution_starttime 
        && current_process.execution_endtime == nullpcb.execution_endtime && current_process.remaining_bursttime == nullpcb.remaining_bursttime 
        && current_process.process_priority == nullpcb.process_priority) {
        new_process.execution_starttime = timestamp;
        //Update execution_endtime to be shortest between time_quantum and total_bursttime
        if(time_quantum < new_process.total_bursttime) {
            burst_or_quantum = time_quantum;
        }
        else {
            burst_or_quantum = new_process.total_bursttime;
        }
        new_process.execution_endtime = timestamp + burst_or_quantum;
        new_process.remaining_bursttime = new_process.total_bursttime;
        return new_process;
    }
    else {
        //Update new_process and add it to the ready_queue, return current_process
        new_process.execution_starttime = 0;
        new_process.execution_endtime = 0;
        new_process.remaining_bursttime = new_process.total_bursttime;
        ready_queue_dyn = add_item(ready_queue, new_process, queue_cnt);
        //If update requires more space in array, then dynamically resize using realloc() and copy current queue into ready_queue
        if(init_queue_cnt > *queue_cnt) {
            ready_queue_dyn = (struct PCB*)realloc(ready_queue, sizeof(struct PCB)*(*queue_cnt));
            needs_free = true;
        }
        for(int i = 0; i < *queue_cnt; i++) {
            ready_queue[i] = ready_queue_dyn[i];
        }
        //If realloc() was used, then free() is used
        if(needs_free){
            free(ready_queue_dyn);
        }
        return current_process;
    }
}

struct PCB handle_process_completion_rr(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, int time_stamp, int time_quantum) {
    struct PCB nullpcb = NULLPCB();
    int burst_or_quantum = 0;
    //If ready_queue is empty, return nullpcb
    if(*queue_cnt == 0) {
        return nullpcb;
    }
    else {
        int earliest_arrival = ready_queue[0].arrival_timestamp;
        int pos = 0;
        struct PCB return_PCB;
        //Iterate through the array and determine the earliest arrival_time process, save process into return_PCB
        for(int i = 0; i < *queue_cnt; i++) {
            if (earliest_arrival >= ready_queue[i].arrival_timestamp) {
                earliest_arrival = ready_queue[i].arrival_timestamp;
                return_PCB = ready_queue[i];
                pos = i;
            }
        }
        //Remove return_PCB process from ready_queue and update queue_cnt
        for(int i = pos; i < *queue_cnt; i++)
        {
            ready_queue[i] = ready_queue[i + 1];
        }
        (*queue_cnt)--;
        //Update the return_PCB and return it
        return_PCB.execution_starttime = time_stamp;
        //Update execution_endtime to be shortest between time_quantum and remaining_bursttime, return return_PCB
        if(time_quantum < return_PCB.remaining_bursttime) {
            burst_or_quantum = time_quantum;
        }
        else {
            burst_or_quantum = return_PCB.remaining_bursttime;
        }
        return_PCB.execution_endtime = time_stamp + burst_or_quantum;
        return return_PCB;
    }
}

struct PCB* add_item(struct PCB* ready_queue, struct PCB a, int *queue_cnt)
{
    ready_queue[*queue_cnt].process_id = a.process_id;
    ready_queue[*queue_cnt].arrival_timestamp = a.arrival_timestamp;
    ready_queue[*queue_cnt].total_bursttime = a.total_bursttime;
    ready_queue[*queue_cnt].execution_starttime = a.execution_starttime;
    ready_queue[*queue_cnt].execution_endtime = a.execution_endtime;
    ready_queue[*queue_cnt].remaining_bursttime = a.remaining_bursttime;
    ready_queue[*queue_cnt].process_priority = a.process_priority;
    (*queue_cnt)++;
    return ready_queue;
}