file(GLOB_RECURSE RT_SOURCES
  img/*.cc
  gm/*.cc
  util/*.cc
  rt/*.cc 
  material/*.cc
  shape/*.cc
  texture/*.cc
  accelerate/*.cc
  sample/*.cc
  option.cc)

GenLib(ray_tracer ${RT_SOURCES})

set(RT_LIBS ray_tracer)
set(RT_APP_SRC main.cc main_scene.cc)
GenApplication(rt RT_APP_SRC RT_LIBS)
