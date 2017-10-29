// exec
#include <cstdio>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>

#include <cstring>
#include <unistd.h>
#include <signal.h>
#include <linux/fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <dirent.h>

// dependency stuff
#include <string>
#include <vector>
#include <set>
#include <map>

#define READ_END 0
#define WRITE_END 1

namespace SH {
    bool process_running(int pid) {
        return (kill(pid,0) == 0);
    }
    std::string exec(std::vector<std::string> cmd, const std::string& input) {
        std::string result;
        char buffer[2048];
        pid_t pid = 0;
        int pipefd[2][2];
        
        if (pipe2(pipefd[0], O_CLOEXEC) == -1) {
            throw std::runtime_error("exec/pipe (1) failed");
        }
        
        if (pipe2(pipefd[1], O_CLOEXEC) == -1) {
            throw std::runtime_error("exec/pipe (2) failed");
        }
        
        pid = fork();
        if (pid == 0) {
            
            const char* arr[cmd.size()+1];
            for (unsigned int i=0; i<cmd.size(); i++) {
                arr[i] = cmd[i].c_str();
            }
            arr[cmd.size()] = NULL;
            // Child
            dup2(pipefd[0][ READ_END], STDIN_FILENO);
            dup2(pipefd[1][WRITE_END], STDOUT_FILENO);
            dup2(pipefd[1][WRITE_END], STDERR_FILENO);
            // fucking awful code, I hate C
            execvp(arr[0], (char * const*) arr);
            std::cerr << "execl failed " << errno << std::endl;
            // Nothing below this line should be executed by child process. If so, 
            // it means that the execl function wasn't successfull, so lets exit:
            _exit(1);
        }
        
        
        close(pipefd[0][READ_END]);
        if (input.size() > 0) {
            if (write(pipefd[0][WRITE_END], input.c_str(), input.size()) == -1) {
                std::cout << "(exec warning, write failed)" << std::endl;
            }
        }
    
        close(pipefd[0][WRITE_END]);
        close(pipefd[1][WRITE_END]);
        
        try { 
            int i;
            while ((i = read(pipefd[1][READ_END], buffer, 2048)) > 0) {
                buffer[i] = 0;
                result += buffer;
            }
        } catch(...) {
            // zombie 
            throw;
        }
        
        return result;
    }

    void exec_i(std::string cmd) {
        int status = system(cmd.c_str());
        if (status != 0) throw std::runtime_error("exec_i/system failed");
    }

    void mkdir(std::string str) {
        DIR* dir = ::opendir(str.c_str());
        if (!dir) {
            if (ENOENT == errno) {
                int status = ::mkdir(str.c_str(), S_IRUSR | S_IRGRP | S_IWUSR | S_IWGRP | S_IXUSR | S_IXGRP | S_IROTH);
                if (status != 0) throw std::runtime_error("mkdir/mkdir failed");
            } else {
                throw std::runtime_error("mkdir/opendir failed");
            }
        }
    }

    void rm(std::string str) {
        remove(str.c_str()); // ignore errors, we don't really care if it didn't exist, or so.
    }

};


class Task {
public:
    virtual std::string getName() = 0;
    virtual std::vector<std::string> dependencies() = 0;
    virtual bool task() = 0;
};

class TaskBuild : public Task {
private:
    std::string binary;
    std::vector<std::string> sources;
    std::vector<std::string> optimization;
    std::vector<std::string> static_bin;
public:
    TaskBuild(std::string binary, std::vector<std::string> sources, int opt, bool static_bin) : binary(binary) {
        this->sources = sources;
        switch (opt) {
            case 1:
                optimization = {"-O1"};
                break;
            case 2:
                optimization = {"-O2"};
                break;
            case 3:
                optimization = {"-O3"};
                break;
            default:
                if (opt != 0) {
                    std::cout << "Invalid optimization mode; assuming 0/debug" << std::endl;
                }
                optimization = {"-O0","-g"};
                break;
        }
        this->static_bin = static_bin ? std::vector<std::string>{"-static"} : std::vector<std::string>{};
    }
    std::string getName() { return "build"; }
    std::vector<std::string> dependencies() {
        return { "fonts" };
    }
    bool task() {
        try {
            SH::mkdir("build/");
            std::vector<std::string> args = {"g++","-std=c++11","-lm","-o","build/" + binary,"-Iinclude/","-Ifont/"};
            args.insert(args.end(), sources.begin(), sources.end());
            args.insert(args.end(), optimization.begin(), optimization.end());
            args.insert(args.end(), static_bin.begin(), static_bin.end());
            std::cout << SH::exec(args, "") << std::endl;
        } catch (std::runtime_error& err) {
            return false;
        }
        return true;
    }
};

class TaskFonts : public Task {
private:
    std::vector<char> chars;
public:
    void add(char start, char end) {
        for (char i = start; i <= end; i++) {
            chars.push_back(i);
        }
    }
    TaskFonts() {
        add('0', '9');
        add('A', 'Z');
        add('a', 'z');
        chars.push_back('@');
        chars.push_back('>');
        chars.push_back('<');
        chars.push_back('$');
        chars.push_back('"');
        chars.push_back('\'');
        chars.push_back('~');
    }
    std::string getName() { return "fonts"; }
    std::vector<std::string> dependencies() {
        return {};
    }
    bool task() {
        SH::mkdir("font");
        SH::rm("font/font.h");
        std::ofstream f("font/font.h");
        for (auto c : chars) {
            std::string s(1, c);
            std::string xbm = SH::exec({"convert","-font","./dejavu.ttf","-resize","8x16!", "-pointsize","14","label:"+s, "xbm:-"}, "");
            std::string tail = SH::exec({"tail", "--lines=3"}, xbm);
            if (s == "'") s = "\\\\'"; // hacky hack
            std::string sed = SH::exec({"sed","s|^static char \\(.*\\)_bits\\[\\]|bits['" + s +"']|"}, tail);
            f << sed << std::endl;
            
        }
        return true;
    }
};


class TaskManager {
private:
    std::map<std::string, std::shared_ptr<Task>> deps;
    bool sanity_internal(const std::string& start, std::map<std::string,std::string> visited, const std::string& requester) {
        bool failed = false;
        if (deps[start] == NULL) {
            /* dependency not found */
            std::cout << "Task not found: " << start << " (required by " << requester << ")" << std::endl;
            return true;
        }
        visited[start] = requester;
        for (auto dep : deps[start]->dependencies()) {
            if (visited[dep] != "") {
                std::cout << "Loop found" << std::endl;
                std::cout << "Task " << dep << " requested by " << requester << std::endl;
                
                std::string curr = dep;
                while (visited[dep] != "") {
                    std::cout << "Task " << visited[dep] << " requested by " << dep << std::endl;
                }
                std::cout << "---------" << std::endl;
                failed = true;
            } else {
                failed |= sanity_internal(dep, visited, start);
                // if we are here
                visited[dep] = "";
            }
        }
        return failed;
    }
    bool sanity(const std::string& start) {
        std::map<std::string,std::string> visited;
        return !sanity_internal(start, visited, "<<user>>");
    }
    /* run_internal assumes there are no loops, and all tasks exist */
    bool run_internal(std::string task, bool ignore_deps, std::set<std::string> finished) {
        if (finished.find(task) != finished.end()) return true; // task has been completed already
        
        bool deps_ok = true;
        if (!ignore_deps) {
            std::vector<std::string> deps_v = deps[task]->dependencies();
            for (auto dep : deps_v) {
                std::cout << "[NEEDED] " << task << " requires " << dep << std::endl;
                deps_ok &= run_internal(dep, false, finished);
            }
        } else {
            std::cout << "[WARNING] Assuming dependencies have been met already" << std::endl;
        }
        if (!deps_ok) {
            std::cout << "Some dependencies have failed, cannot continue" << std::endl;
            return false;
        }
        
        std::cout << "[RES] Task '" << task << "' | ";
        bool task_ret = deps[task]->task();
        if (task_ret) {
            std::cout << "OK     :)" << std::endl;
        } else {
            std::cout << "failed :(" << std::endl;
        }
        return task_ret;
    }
public:
    TaskManager() {
        std::cout.setf(std::ios::unitbuf);
    };
    void list_tasks() {
        std::cout << "Available tasks: " << std::endl;
        for (auto task : deps) {
            std::cout << "\t" << task.second->getName() << std::endl;
        }
    }
    void add(std::shared_ptr<Task> task) {
        if (task == NULL) {
            std::cout << "NULL Task added (BROKEN CODE)" << std::endl;
            return;
        } 
        if (deps[task->getName()] != NULL) {
            std::cout << "A dependency with the name " << task->getName() << " already exists" << std::endl;
            return;
        }
        
        deps[task->getName()] = task;
    }
    
    void run(std::string str, bool ignore_deps) {
        if (!sanity(str)) {
            return;
        }
        std::set<std::string> finished;
        run_internal(str, ignore_deps,finished);
    }
};


int main(int argc, char** argv) {
    TaskManager man;
    
    std::vector<std::string> sources = {
        "main.cxx",
        "src/Color.cxx",
        "src/DoubleBuffering.cxx",
        "src/FontManager.cxx",
        "src/Pos.cxx",
        "src/RawFB.cxx",
        "src/Toolkit.cxx",
        "src/Exceptions.cxx"
    };
    
    man.add(std::shared_ptr<Task>(new TaskFonts()));
    man.add(std::shared_ptr<Task>(new TaskBuild("fb", sources, 0, false)));

    std::string task = "build";
    bool ignore = false;
    if (argc > 1) {
        if(strcmp(argv[1],"ignore_deps") == 0) {
            ignore = true;
            if (argc > 2) task = argv[2];
        } else {
            task = argv[1];
        }
    }
    man.run(task, ignore);
}
