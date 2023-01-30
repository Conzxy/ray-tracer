file(GLOB_RECURSE RT_SOURCES
  img/*.cc
  gm/*.cc
  util/*.cc
  rt/*.cc
  shape/*.cc
  material/*.cc
  texture/*.cc
  accelerate/*.cc
  sample/*.cc
  *.cc)

GenLib(ray_tracer ${RT_SOURCES})

set(RT_LIBS ray_tracer)
GenApplication(rt RT_SOURCES RT_LIBS)
