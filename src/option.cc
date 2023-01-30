#include "option.hh"

#include <cstdio>
#include <cstring>
#include <string_view>

#include "util/str_cvt.hh"

namespace rt {

void Option::DebugPrint() const
{
  // Log the command line arguments for debugging
  printf("path = %s\n", path);
  printf("sample_per_pixel = %d\n", sample_per_pixel);
  printf("thread_num = %d\n", thread_num);
  printf("image_height = %d\n", image_height);
  printf("gamma = %d\n", gamma);
  printf("scene = %d\n", scene_id);
}

#define PROGRAM_USAGE                                                          \
  "Usage: %s [image path] "                                                    \
  "[--sample_per_pixel/-spp integer] "                                         \
  "[--threads/-t integer] "                                                    \
  "[--gamma/-g integer] "                                                      \
  "[--height/-h integer] "                                                     \
  "[--scene/-s integer]\n",                                                    \
      argv[0]

inline bool check_option(std::string_view opt, char const *lopt,
                         char const *sopt) noexcept
{
  return opt == lopt || opt == sopt;
}

bool parse_options(int argc, char *argv[], Option *option)
{
  if (2 > argc) {
    fprintf(stderr, PROGRAM_USAGE);
    return false;
  }

  if (!strcmp(argv[1], "--help") || !strcmp(argv[1], "-h")) {
    printf(PROGRAM_USAGE);
    return false;
  }

  option->path = argv[1];

  argc -= 2;
  argv += 2;

  while (argc > 0) {
    auto eq_pos = strchr(*argv, '=');
    if (!eq_pos) {
      fprintf(stderr, "This is not a valid option format: <option> = value\n");
      return false;
    }

    std::string_view opt(*argv, eq_pos - *argv);
    char const *arg = eq_pos + 1;

    if (opt == "-spp" || opt == "--sample-per-pixel") {
      auto ret = util::str2int(arg);
      if (!ret) {
        fprintf(stderr, "The argument of --sample-per-pixel/-spp is invalid\n");
        return false;
      }
      option->sample_per_pixel = *ret;
    } else if (opt == "--threads" || opt == "-t") {
      auto ret = util::str2int(arg);
      if (!ret) {
        fprintf(stderr, "The argument of --threads/-t is invalid\n");
        return false;
      }
      option->thread_num = *ret;
    } else if (opt == "--height" || opt == "-h") {
      auto ret = util::str2int(arg);
      if (!ret) {
        fprintf(stderr, "The argument of --height/-h is invalid\n");
        return false;
      }
      option->image_height = *ret;
    } else if (opt == "--gamma" || opt == "-g") {
      auto ret = util::str2int(arg);
      if (!ret) {
        fprintf(stderr, "The argument of --gamma/-g is invalid\n");
        return false;
      }
      option->gamma = *ret;

    } else if (check_option(opt, "--scene", "-s")) {
      auto ret = util::str2int(arg);
      if (!ret) {
        fprintf(stderr, "The argument of --scene/-s is invalid\n");
        return false;
      }
      option->scene_id = *ret;
    } else {
      fprintf(stderr, "Unknown option: %s\n", *argv);
      return false;
    }

    argc--;
    argv++;
  }

  return true;
}

} // namespace rt
