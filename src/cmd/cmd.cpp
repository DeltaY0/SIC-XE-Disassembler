#include "cmd.h"

ylib::CommandInfo ylib::parse_cli(std::vector<std::string> arguments, std::vector<Command> commands)
{
    std::vector<std::string> args = arguments;
    args.erase(std::find(args.begin(), args.end(), args[0]));
    Command calledCmd;

    // first => check the commands and choose the correct one
    for(Command cmd : commands)
    {
        if(args[0] == cmd.name)
        {
            // if command is found: remove it from the vector.
            calledCmd = cmd;
            args.erase(std::find(args.begin(), args.end(), args[0]));
            break;
        }
    }

    std::map<std::string, std::string> foundArgs;
    std::map<std::string, std::string> foundAvailableArgs;
    std::vector<std::string> cmdIn;

    // parse the arguments first.
    std::vector<std::string> usedArgs;
    for(usize i = 0; i < args.size(); i++)
    {
        // ex: Proj1 --config-file ./YMake.toml -C
        if(args[i][0] == '-')
        {
            // found an arg.
            bool found = false;
            for(CommandArgument &arg : calledCmd.args)
            {
                // looking for it in the options for the command.
                if(args[i] == arg.shortOpt || args[i] == arg.longOpt)
                {
                    // found it!
                    found = true;
                    if(arg.valType == ValueType::BOOL || arg.valType == ValueType::NONE)
                    {
                        // no need to check the other args.
                        foundAvailableArgs[arg.name] = "NULL";
                        usedArgs.push_back(args[i]);
                    }
                    else if((i + 1) < args.size() && args[i + 1][0] != '-')
                    {
                        // need to check the next val.
                        foundAvailableArgs[arg.name] = args[i + 1];
                        usedArgs.push_back(args[i]);
                        usedArgs.push_back(args[i + 1]);
                        i++;
                    }
                    else
                    {
                        LLOG(RED_TEXT("[YSICXE ERROR]: "), "command \'", calledCmd.name, "\' is incorrect.\n");
                        calledCmd.output_cmd_info();
                        throw ylib::Error("incorrect arguments for a command.");
                    }
                }
            }

            if(!found)
            {
                LLOG(RED_TEXT("[YSICXE ERROR]: "), "command \'", calledCmd.name, "\' is incorrect.\n");
                calledCmd.output_cmd_info();
                throw ylib::Error("incorrect arguments for a command.");
            }
        }
    }

    // parse the std in.
    for(usize i = 0; i < args.size(); i++)
    {
        bool used = false;
        for(auto arg : usedArgs)
        {
            if(args[i] == arg)
            {
                used = true;
            }
        }

        if(!used)
        {
            cmdIn.push_back(args[i]);
        }
    }

    LTRACE(true, BLUE_TEXT("COMMAND INPUT: \n"));
    for(auto entry : cmdIn)
    {
        LTRACE(true, "\t", entry, "\n");
    }

    LTRACE(true, BLUE_TEXT("COMMAND ARGS: \n"));
    for(auto &[k, v] : foundAvailableArgs)
    {
        LTRACE(true, "\tkey: ", k, "\tval: ", v, "\n");
    }

    CommandInfo info = {
        .cmd       = calledCmd,
        .cmdIn     = cmdIn,
        .arguments = foundAvailableArgs,
    };

    return info;
}

void ylib::output_project_info()
{
    LLOG(BLUE_TEXT("YSICXE v", VERSION_MAJOR, ".", VERSION_MINOR, ".", VERSION_PATCH, "\n"));
    LLOG("A SIC/XE Disassembler and Linker/Loader built in C++\n");
    LLOG("To get started, run \'ysicxe help\' for usage info\n");
}

void ylib::output_help_info(std::vector<Command> commands)
{
    LLOG(BLUE_TEXT("YSICXE v", VERSION_MAJOR, ".", VERSION_MINOR, ".", VERSION_PATCH, "\n"));

    // LLOG("Create a YMake.toml using \'ymake default\' to get started.\n");
    LLOG("\tCheck docs at https://github.com/DeltaY0/SIC-XE-Disassembler for more information.\n\n");

    LLOG(GREEN_TEXT("USAGE: \n"));

    for(Command cmd : commands)
    {
        LLOG("ysicxe ", CYAN_TEXT(cmd.name), "\t", cmd.desc, "\n");

        if(cmd.args.size() > 0)
            LLOG(PURPLE_TEXT("\targuments: \n"));
        for(CommandArgument arg : cmd.args)
        {
            LLOG("\t", CYAN_TEXT(arg.name), ": ", arg.shortOpt, ", ", arg.longOpt, "\t", arg.desc, "\n");
        }

        LLOG("\n");
    }
}