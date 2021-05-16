// Gaxtapper: Automated GSF ripper for GAX Sound Engine.

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include "args.hxx"
#include "gaxtapper/cartridge.hpp"
#include "gaxtapper/gaxtapper.hpp"

using namespace gaxtapper;
using namespace std::literals::string_view_literals;

const std::string_view kAppName = "Gaxtapper"sv;
const std::string_view kAppVersion = "0.1"sv;
const std::string_view kAppCredits =
    "Created by loveemu. Visit <http://github.com/loveemu/gaxtapper> for details."sv;

args::Group arguments("arguments");
args::HelpFlag help(arguments, "help", "Show this help message and exit",
                    {'h', "help"});

void ExtractCommand(args::Subparser& parser) {
  args::ValueFlag<std::filesystem::path> outdir_arg(
      parser, "directory",
      "The output directory (the default is the working directory)", {'d'});
  args::ValueFlag<std::filesystem::path> basename_arg(
      parser, "basename", "The output filename (without extension)", {'o'});
  args::Positional<std::filesystem::path> input_arg(
      parser, "romfile", "The ROM file to be processed",
      args::Options::Required);

  parser.Parse();

  const auto in_path = args::get(input_arg);
  if (!exists(in_path)) {
    std::ostringstream message;
    message << in_path.string() << ": File does not exist" << std::endl;
    throw std::runtime_error{message.str()};
  }

  Cartridge cartridge = Cartridge::LoadFromFile(in_path);

  const std::filesystem::path basename{basename_arg ? args::get(basename_arg)
                                                    : in_path.stem()};
  const std::filesystem::path outdir{args::get(outdir_arg)};
  const std::string gsfby{"Gaxtapper"};

  Gaxtapper::ConvertToGsfSet(cartridge, basename, outdir, gsfby);
}

void InspectCommand(args::Subparser& parser) {
  args::PositionalList<std::filesystem::path> paths(
      parser, "romfiles", "The ROM files to be processed");

  parser.Parse();

  for (auto&& path : paths) {
    if (!exists(path)) {
      std::cerr << path.string() << ": File does not exist" << std::endl;
      continue;
    }

    Cartridge cartridge = Cartridge::LoadFromFile(path);
    std::cout << "# " << path.stem().string() << " (" << cartridge.game_code() << ")" << std::endl << std::endl;
    Gaxtapper::Inspect(cartridge);
    std::cout << std::endl;
  }
}

int main(int argc, const char** argv) {
  args::ArgumentParser parser(
      "An automated GSF ripper for GAX Sound Engine by Shin'en Multimedia.");
  args::Group commands(parser, "commands");
  args::Command extract(commands, "extract", "TODO SAVE", &ExtractCommand);
  args::Command inspect(commands, "inspect", "TODO INSPECT", &InspectCommand);
  args::GlobalOptions globals(parser, arguments);

  try {
#if false
    args::Command commit(commands, "commit", "record changes to the repository",
                         &CommitCommand);
    args::GlobalOptions globals(p, arguments);


    args::HelpFlag help(parser, "help", "Show this help message and exit",
                        {'h', "help"});
    args::Flag inspect_arg(
        parser, "inspect",
        "Show the inspection result without saving files and quit",
        {"inspect"});
    args::ValueFlag<std::filesystem::path> outdir_arg(
        parser, "directory",
        "The output directory (the default is the working directory)", {'d'});
    args::ValueFlag<std::filesystem::path> basename_arg(
        parser, "basename", "The output filename (without extension)", {'o'});
    args::Positional<std::filesystem::path> input_arg(
        parser, "romfile", "The ROM file to be processed",
        args::Options::Required);
#endif

    try {
      if (argc < 2) throw args::Help(help.Name());

      parser.ParseCLI(argc, argv);
    } catch (args::Help&) {
      std::cout << kAppName << " " << kAppVersion << std::endl << std::endl;
      std::cout << kAppCredits << std::endl << std::endl;
      std::cout << parser;
      return EXIT_SUCCESS;
    }
#if false
    const auto in_path = args::get(input_arg);
    if (!std::filesystem::exists(in_path)) {
      std::cerr << in_path.string() << ": File does not exist" << std::endl;
      return EXIT_FAILURE;
    }

    Cartridge cartridge = Cartridge::LoadFromFile(in_path);

    if (inspect_arg) {
      Gaxtapper::Inspect(cartridge);
    } else {
      const std::filesystem::path basename{
          basename_arg ? args::get(basename_arg) : in_path.stem()};
      const std::filesystem::path outdir{args::get(outdir_arg)};
      std::string gsfby{"Gaxtapper"};

      Gaxtapper::ConvertToGsfSet(cartridge, basename, outdir, gsfby);
    }
#endif
  } catch (args::Error& e) {
    std::cerr << e.what() << std::endl << parser;
    return EXIT_FAILURE;
  } catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
