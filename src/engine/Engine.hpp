#pragma once

#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <vector>
#include <cstring>
#include <string>
#include <sstream>
#include <csignal>
#include <cstdlib>
#include <cstdio>

class Engine{
    private :
        int pipeIn[2], pipeOut[2]; 
        // in - user writes, engine reads
        // out - engine writes the best move, user reads and make the move
        pid_t pid;

    public : 
        Engine();
        ~Engine();
        void Start(const char* path = "stockfish");
        void WriteInteract(std::string command); //user gives stockfish commands
        std::string GetBestMove();//give us the parsed bestmove
};