#include "Engine.hpp"

// ============================================================
// StockfishEngine (IPC)
// ============================================================

StockfishEngine::StockfishEngine(){
    pid = -1;
    pipeIn[0] = pipeIn[1] = pipeOut[0] = pipeOut[1] = -1;
}

StockfishEngine::~StockfishEngine(){
    if(pipeIn[1] >= 0) close(pipeIn[1]);
    if(pipeOut[0] >= 0) close(pipeOut[0]);
    if(pid > 0){
        int status;
        kill(pid, SIGTERM);
        waitpid(pid,&status,0);
    }
}

void StockfishEngine::Start(const char* path){
    if(pipe(pipeIn) < 0 || pipe(pipeOut) < 0){
        perror("Pipe failed\n");
        exit(1);
    }
    pid = fork();
    if(pid == 0){
        //child : stockfish
        close(pipeIn[1]);
        close(pipeOut[0]);
        dup2(pipeOut[1], STDOUT_FILENO);
        dup2(pipeIn[0], STDIN_FILENO);
        close(pipeIn[0]);
        close(pipeOut[1]);
        execlp(path, path, nullptr);
        std::cerr << "Stockfish not found" << std::endl;
        exit(1);
    }
    else{
        //parent: chess
        close(pipeIn[0]);
        close(pipeOut[1]);
    }
}

void StockfishEngine::WriteInteract(std::string command){
    command += '\n';
    if(pipeIn[1] != -1) write(pipeIn[1], command.c_str(), command.length());
    else {
        perror("pipeIn[1] is -1\n");
        exit(1);
    }
}

std::string StockfishEngine::GetBestMoveUci(){
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

Move StockfishEngine::GetBestMove(Board& board, int timeLimitMs){
    std::string command = "position startpos";
    if(!board.history.empty()){
        command += " moves";
        for(auto& m : board.history){
            command += " " + Board::MoveToUci(m);
        }
    }
    WriteInteract(command);
    WriteInteract("go movetime " + std::to_string(timeLimitMs));
    std::string bestMoveUci = GetBestMoveUci();
    if(!bestMoveUci.empty()){
        return Board::UciToMove(bestMoveUci, board);
    }
    return Move();
}

std::pair<int, std::string> StockfishEngine::EvaluatePosition(const std::vector<std::string>& uciMoves, int depth){
    std::string command = "position startpos";
    if(!uciMoves.empty()){
        command += " moves";
        for(const auto& m : uciMoves){
            command += " " + m;
        }
    }
    WriteInteract(command);
    WriteInteract("go depth " + std::to_string(depth));

    // Read output until bestmove, parse last score
    char buffer[4096];
    std::string result = "";
    std::string bestMoveStr = "";

    while(true){
        int bytes = read(pipeOut[0], buffer, sizeof(buffer) - 1);
        if(bytes <= 0) break;
        buffer[bytes] = '\0';
        result += buffer;
        if(result.find("bestmove") != std::string::npos) break;
    }

    // Parse the last "score cp X" or "score mate X" from info lines
    int score = 0;
    std::istringstream stream(result);
    std::string line;
    while(std::getline(stream, line)){
        size_t pos = line.find("score cp ");
        if(pos != std::string::npos){
            std::istringstream ss(line.substr(pos + 9));
            ss >> score;
        }
        pos = line.find("score mate ");
        if(pos != std::string::npos){
            int mateIn;
            std::istringstream ss(line.substr(pos + 11));
            ss >> mateIn;
            score = (mateIn > 0) ? 49000 - mateIn : -49000 - mateIn;
        }
        pos = line.find("bestmove ");
        if(pos != std::string::npos){
            std::istringstream ss(line.substr(pos + 9));
            ss >> bestMoveStr;
        }
    }

    return {score, bestMoveStr}; // from side-to-move perspective
}

// ============================================================
// ForgeEngine (custom in-process search)
// ============================================================

Move ForgeEngine::GetBestMove(Board& board, int timeLimitMs){
    return search.FindBestMove(board, timeLimitMs);
}

int ForgeEngine::GetLastSearchDepth() const{
    return search.GetDepthReached();
}

int ForgeEngine::GetNodesSearched() const{
    return search.GetNodesSearched();
}
