#ifndef RT_OPTION_HH__
#define RT_OPTION_HH__

namespace rt {

struct Option {
  char const *path = nullptr;
  int sample_per_pixel = 100;
  int thread_num = 8;
  int gamma = 2;
  int image_height = 400;
  int scene_id = -1;
  void DebugPrint() const;
};

bool parse_options(int argc, char *argv[], Option *option);

} // namespace rt

#endif
