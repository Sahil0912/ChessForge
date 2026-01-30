#include "Engine.hpp"
    

Engine::Engine(){
    pid = -1;
    pipeIn[0] = pipeIn[1] = pipeOut[0] = pipeOut[1] = -1;
}

Engine::~Engine(){
    if(pipeIn[1] >= 0) close(pipeIn[1]);
    if(pipeOut[0] >= 0) close(pipeOut[0]);
    if(pid > 0){
        int status;
        kill(pid, SIGTERM);
        waitpid(pid,&status,0);
    }
    

}
void Engine::Start(const char* path){
    
    
    if(pipe(pipeIn) < 0 || pipe(pipeOut) < 0){
        perror("Pipe failed\n");
        exit(1);
    }
    pid = fork();
    if(pid == 0){
        //child : stockfish
        // reads from pipeIn[0];
        //writes in pipeOut[1];
        close(pipeIn[1]);
        close(pipeOut[0]);

        dup2(pipeOut[1], STDOUT_FILENO);
        dup2(pipeIn[0], STDIN_FILENO);

        //cleaning up the duplicates
        close(pipeIn[0]);
        close(pipeOut[1]);

        execlp(path, path, nullptr);

        std::cerr << "Stockfish not found" << std::endl;
        exit(1); 
    }
    else{
        //parent: chess
        // reads from pipeOut[0];
        //writes in pipeIn[1];
        close(pipeIn[0]);
        close(pipeOut[1]);
    }
}
void Engine::WriteInteract(std::string command){
    command += '\n';
    if(pipeIn[1] != -1) write(pipeIn[1], command.c_str(), command.length());
    else {
        perror("pipeIn[1] is -1\n");
        exit(1);
    }
}
std::string Engine::GetBestMove(){
    char buffer[1024];
    std::string result = "";
    while(true){
        int bytes = read(pipeOut[0], buffer, sizeof(buffer) - 1);
        if(bytes < 0){
            perror("No read!\n");
            exit(1);
        }
        else if(bytes == 0){
            perror("Stockfish closed\n");
            exit(1);
        }
        else{
            buffer[bytes] = '\0';
        }
        result += buffer;
        if(result.find("bestmove") != std::string::npos){
            size_t index = result.find("bestmove");
            std::string remString = result.substr(index);
            std::stringstream ss(remString);
            std::string temp, move;
            ss >> temp >> move;
            return move;
        }
    }
}
