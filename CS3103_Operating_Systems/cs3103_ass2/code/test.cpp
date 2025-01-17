#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <iostream>
#include "Queue.h"
using namespace std;

#define Token int       // use a structure or simply integer?
// thread safe queue?
// argument and mutex initialization
struct Thread_arg {
    Queue<Token>* buffer;
    int max_token;
    double flow_interval;
    Thread_arg(Queue<Token>* b, int m, double f):buffer(b), max_token(m), flow_interval(f) {}
};

// shared resources
pthread_mutex_t my_mutex = PTHREAD_MUTEX_INITIALIZER;
int generated_token, fetched_token, dropped_token;

// get random number
int getRand(int a, int b) {
    srand((unsigned)time(NULL));
    return rand() % (b - a + 1) + a;
}

void* Flow(void *threadarg) {
    Thread_arg* my_data;
    my_data = (Thread_arg*)threadarg;
    const int max_token = my_data->max_token;
    const double flow_interval = my_data->flow_interval;
    Queue<Token>* buffer = my_data -> buffer;

    int seq_num = 0;
    while(fetched_token + dropped_token < max_token) {
        usleep((unsigned int)(flow_interval * 1e6));

        pthread_mutex_lock(&my_mutex);
        int added_token = getRand(1, 10);
        generated_token += added_token;
        // use a for loop to simulate
        for(int i = 0; i < added_token; i++) {
            if(dropped_token + fetched_token >= max_token)break;
            if(!buffer->full()) {
                buffer->push(seq_num);
            } else {
                dropped_token++;
            }
            seq_num++;
        }
        printf("%3d          %3d                    %3d\n", added_token, seq_num - 1, buffer->size());
        pthread_mutex_unlock(&my_mutex);
    }
    pthread_exit(NULL);
}

void *Server(void *threadarg) {
    Thread_arg* my_data;
    my_data = (Thread_arg*)threadarg;
    int max_token = my_data->max_token;
    Queue<Token>* buffer = my_data -> buffer;
    while(fetched_token + dropped_token < max_token) {
        usleep(2000000);

        pthread_mutex_lock(&my_mutex);
        int deleted_token = getRand(1, 20);

        int cnt;
        for(cnt = 0; cnt < deleted_token && !buffer->empty(); cnt++) {
            if(dropped_token + fetched_token >= max_token)break;
            buffer->pop();
            fetched_token++;
        }
        printf("                                    %3d             %3d          %3d\n"
               , buffer->size(), cnt, fetched_token);
        pthread_mutex_unlock(&my_mutex);

    }
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    if(argc < 3) {
        printf("Usage: $ ./Filename 500 2\n");
    } else {
        // initialization and create the argument needed by the threads
        fetched_token = dropped_token = generated_token = 0;
        int max_token = atoi(argv[1]);
        double flow_interval = atof(argv[2]);
        Thread_arg arg(new Queue<Token>(), max_token, flow_interval);
        printf("Flow         Queue                                  Server\n");
        printf("Token added  Latest sequence number Current Length  Token served Total Token fetched\n");
        //srand((unsigned)time(NULL));
        // create 2 threads to do the simulation
        int record;
        pthread_t flow_thread, server_thread;
        record = pthread_create(&flow_thread, NULL, Flow, (void*)&arg);
        if (record) {
            cout << "Error when creating thread!" << endl;
            exit(-1);
        }
        record = pthread_create(&server_thread, NULL, Server, (void*)&arg);
        if (record) {
            cout << "Error when creating thread!" << endl;
            exit(-1);
        }

        // wait for 2 thread to finish
        record = pthread_join(flow_thread, NULL);
        if (record) {
            cout << "Error when joining thread!" << endl;
            exit(-1);
        }
        record = pthread_join(server_thread, NULL);
        if (record) {
            cout << "Error when joining thread!" << endl;
            exit(-1);
        }

        printf("The total number of tokens that have been generated by the flow is %d.\n", generated_token);
        printf("The total number of tokens that have been fetched by the server is %d.\n", fetched_token);
        printf("The total number of tokens that have been dropped by the queue is %d.\n", dropped_token);

    }
    return 0;
}
