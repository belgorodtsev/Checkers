#include <iostream>
#include <iterator>
#include <fstream>
#include <filesystem>
#include <Game.hpp>

namespace fs = std::filesystem;

const auto ROOT_DIR = ".";
const auto DATA = "data";
const auto ANSWERS = "answers";
const auto RESULTS = "results";
const auto DEPTH = 4;

std::string readFile(const std::string& fileName) {
    std::ifstream f(fileName);
    std::stringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

bool compare(const fs::path& res, const fs::path& ans) {
    auto strRes = readFile(res.string());
    auto ansRes = readFile(ans.string());
    return strRes == ansRes;
}

bool test() {
    const fs::path testPath(ROOT_DIR);
    const fs::path dataPath = testPath / DATA;
    const fs::path answerPath = testPath / ANSWERS;

    fs::create_directory(testPath / RESULTS);
    const fs::path resultsPath = testPath / RESULTS;

    for (auto& file : fs::directory_iterator(dataPath)) {
        auto in = file.path().string();
        const fs::path outPath = resultsPath / (file.path().stem().string() + ".result");

        auto out = outPath.string();
        Game game(in, out);
        game.calculateBestMove(WHITE, DEPTH);
        game.makeBestMove();
        game.saveGame();

        const fs::path ansPath = answerPath / (file.path().stem().string() + ".ans");
        auto ans = ansPath.string();
        if (!compare(out, ans))
            return false;
    }
    return true;
}

int main(int argc, char* argv[]) {
    std::cout << std::boolalpha << test() << std::endl;
    return 0;
}