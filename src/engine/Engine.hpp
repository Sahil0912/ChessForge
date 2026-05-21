#pragma once

#include "../game/Board.hpp"
#include "../game/Move.hpp"
#include "Search.hpp"
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <utility>
#include <vector>

class IChessEngine {
public:
  virtual ~IChessEngine() = default;
  virtual Move GetBestMove(Board &board, int timeLimitMs) = 0;
  virtual int GetLastSearchDepth() const { return 0; }
  virtual int GetNodesSearched() const { return 0; }
};

// stockfish via IPC
class StockfishEngine : public IChessEngine {
private:
  int pipeIn[2], pipeOut[2];
  pid_t pid;

public:
  StockfishEngine();
  ~StockfishEngine() override;
  void Start(const char *path = "/usr/bin/stockfish");
  void WriteInteract(std::string command);
  std::string GetBestMoveUci();
  Move GetBestMove(Board &board, int timeLimitMs) override;
  std::pair<int, std::string> EvaluatePosition(const std::vector<std::string>& uciMoves, int depth = 10);
};

// custom engine (in-process search)
class ForgeEngine : public IChessEngine {
private:
  Search search;

public:
  Move GetBestMove(Board &board, int timeLimitMs) override;
  int GetLastSearchDepth() const override;
  int GetNodesSearched() const override;
  int GetLastBestScore() const { return search.GetLastBestScore(); }
};