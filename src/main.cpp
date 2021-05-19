// Gaxtapper: Automated GSF ripper for GAX Sound Engine.

#include <charconv>
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
  args::ValueFlag<std::string> entrypoint_arg(
      parser, "entrypoint", "Entrypoint address where the driver is inserted", {"entrypoint"});
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

  const std::filesystem::path basename{
      basename_arg ? args::get(basename_arg)
                   : std::filesystem::path{cartridge.full_game_code()}};
  const std::filesystem::path outdir{args::get(outdir_arg)};
  const std::string gsfby{"Gaxtapper"};

  agbptr_t entrypoint = agbnullptr;
  std::string entrypoint_str{entrypoint_arg.Get()};
  if (!entrypoint_str.empty()) {
    std::string_view s{entrypoint_str};
    if (s.substr(0, 2) == "0X" || s.substr(0, 2) == "0x") s.remove_prefix(2);
    if (auto [ptr, ec] =
            std::from_chars(s.data(), s.data() + s.size(), entrypoint, 16);
        ec != std::errc{}) {
      throw std::invalid_argument(
          "The entrypoint address must be specified as a hexadecimal string.");
    }

    if (!is_romptr(entrypoint)) {
      throw std::invalid_argument(
          "The entrypoint address must point to a ROM section (0x8000000-0x9FFFFFF).");
    }
  }

  Gaxtapper::ConvertToGsfSet(cartridge, basename, entrypoint, outdir, gsfby);
}

void InspectCommand(args::Subparser& parser) {
  args::PositionalList<std::filesystem::path> paths(
      parser, "romfiles", "The ROM files to be processed");
  args::Flag single_line_arg(parser, "foo", "The foo flag", {'S', "single-line"});

  parser.Parse();

  for (auto&& path : paths) {
    if (!exists(path)) {
      std::cerr << path.string() << ": File does not exist" << std::endl;
      continue;
    }

    Cartridge cartridge = Cartridge::LoadFromFile(path);
    if (!single_line_arg.Get()) {
      std::cout << "# " << path.stem().string() << " ("
                << cartridge.full_game_code() << ")" << std::endl
                << std::endl;
      Gaxtapper::Inspect(cartridge);
      std::cout << std::endl;
    }
    else
      Gaxtapper::InspectSimple(cartridge, path.stem().string());
  }
}

int main(int argc, const char** argv) {
  args::ArgumentParser parser(
      "An automated GSF ripper for GAX Sound Engine by Shin'en Multimedia.");
  args::Group commands(parser, "commands");
  args::Command extract(commands, "extract", "Extract songs as gsflib and minigsfs", &ExtractCommand);
  args::Command inspect(commands, "inspect", "Display the driver code/data information in ROM", &InspectCommand);
  args::GlobalOptions globals(parser, arguments);

  try {
    try {
      if (argc < 2) throw args::Help(help.Name());

      parser.ParseCLI(argc, argv);
    } catch (args::Help&) {
      std::cout << kAppName << " " << kAppVersion << std::endl << std::endl;
      std::cout << kAppCredits << std::endl << std::endl;
      std::cout << parser;
      return EXIT_SUCCESS;
    }
  } catch (args::Error& e) {
    std::cerr << e.what() << std::endl << parser;
    return EXIT_FAILURE;
  } catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
