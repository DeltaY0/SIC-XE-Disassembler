#pragma once

#include "../core/defines.h"
#include "../core/logger.h"
#include "../core/error.h"

#include <algorithm>
#include <functional>
#include <map>

namespace ylib {

enum class ValueType
{
    NONE,
    BOOL,
    STRING,
    INT,
    FLOAT
};

struct CommandArgument
{
    std::string name;
    std::string desc;

    //          -t        --type
    std::string shortOpt, longOpt;
    ValueType valType;

    CommandArgument(std::string name, std::string desc) : name{name}, desc{desc} { valType = ValueType::STRING; }
    CommandArgument(std::string name, std::string desc, std::string shortOpt, std::string longOpt)
        : name{name}, desc{desc}, shortOpt{shortOpt}, longOpt{longOpt}
    {
        valType = ValueType::STRING;
    }
    CommandArgument(std::string name, std::string desc, std::string shortOpt, std::string longOpt, ValueType valType)
        : name{name}, desc{desc}, shortOpt{shortOpt}, longOpt{longOpt}, valType{valType}
    {
    }
};

struct Command
{
    std::string name;
    std::string desc;

    std::vector<std::string> commandIn;

    std::vector<CommandArgument> args;

    std::function<void(std::vector<std::string> &, std::map<std::string, std::string> &)> funcToCall;

    Command() {}
    Command(std::string name, std::string desc) : name{name}, desc{desc} {}

    Command(std::string name, std::string desc, std::vector<CommandArgument> args) : name{name}, desc{desc}, args{args}
    {
    }

    Command(std::string name, std::string desc, std::vector<CommandArgument> args,
            std::function<void(std::vector<std::string> &, std::map<std::string, std::string> &)> funcToCall)
        : name{name}, desc{desc}, args{args}, funcToCall{funcToCall}
    {
    }

    Command(std::string name, std::string desc,
            std::function<void(std::vector<std::string> &, std::map<std::string, std::string> &)> funcToCall)
        : name{name}, desc{desc}, funcToCall{funcToCall}
    {
    }

    public:
    void call_function(std::vector<std::string> &cmdIn, std::map<std::string, std::string> &arguments)
    {
        if(funcToCall != nullptr)
            funcToCall(cmdIn, arguments);
        else
        {
            LLOG(RED_TEXT("[YMAKE ERROR]: "), "tried calling a command's function that doesn't exist, command: ", name,
                 "\n");
            throw ylib::Error("couldn't call the function for a command.");
        }

        return;
    }

    void output_cmd_info()
    {
        LLOG(YELLOW_TEXT("[YMAKE]: "), "USAGE: \n");
        LLOG("ymake ", CYAN_TEXT(name), "\t", desc, "\n");

        if(args.size() > 0)
            LLOG(PURPLE_TEXT("\targuments: \n"));
        for(CommandArgument arg : args)
        {
            LLOG("\t", CYAN_TEXT(arg.name), ": ", arg.shortOpt, ", ", arg.longOpt, "\t", arg.desc, "\n");
        }

        LLOG("\n");
    }
};

struct CommandInfo
{
    Command cmd;
    std::vector<std::string> cmdIn;
    std::map<std::string, std::string> arguments;

    public:
    void call_function() { cmd.call_function(cmdIn, arguments); }
};

CommandInfo parse_cli(std::vector<std::string> arguments, std::vector<Command> commands);

// command specific.

// info
void output_project_info();
void output_help_info(std::vector<Command> commands);

// tools
void CreateDefaultConfig(std::vector<std::string> &input, std::map<std::string, std::string> &args);
void CreateNewProject(std::vector<std::string> &input, std::map<std::string, std::string> &args);
void OutputProjectsInfo(std::vector<std::string> &input, std::map<std::string, std::string> &args);
void SetupProjectInfo(std::vector<std::string> &input, std::map<std::string, std::string> &args);
void BuildProjects(std::vector<std::string> &input, std::map<std::string, std::string> &args);
void CleanAllCache(std::vector<std::string> &input, std::map<std::string, std::string> &args);

} // namespace Y