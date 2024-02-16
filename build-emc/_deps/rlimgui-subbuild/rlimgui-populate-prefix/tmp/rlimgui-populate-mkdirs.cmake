# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Users/Brett/Nextcloud/sources/repos/cpp/lpr/lpr-web/lpr-solver-tool/build-emc/_deps/rlimgui-src"
  "C:/Users/Brett/Nextcloud/sources/repos/cpp/lpr/lpr-web/lpr-solver-tool/build-emc/_deps/rlimgui-build"
  "C:/Users/Brett/Nextcloud/sources/repos/cpp/lpr/lpr-web/lpr-solver-tool/build-emc/_deps/rlimgui-subbuild/rlimgui-populate-prefix"
  "C:/Users/Brett/Nextcloud/sources/repos/cpp/lpr/lpr-web/lpr-solver-tool/build-emc/_deps/rlimgui-subbuild/rlimgui-populate-prefix/tmp"
  "C:/Users/Brett/Nextcloud/sources/repos/cpp/lpr/lpr-web/lpr-solver-tool/build-emc/_deps/rlimgui-subbuild/rlimgui-populate-prefix/src/rlimgui-populate-stamp"
  "C:/Users/Brett/Nextcloud/sources/repos/cpp/lpr/lpr-web/lpr-solver-tool/build-emc/_deps/rlimgui-subbuild/rlimgui-populate-prefix/src"
  "C:/Users/Brett/Nextcloud/sources/repos/cpp/lpr/lpr-web/lpr-solver-tool/build-emc/_deps/rlimgui-subbuild/rlimgui-populate-prefix/src/rlimgui-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Users/Brett/Nextcloud/sources/repos/cpp/lpr/lpr-web/lpr-solver-tool/build-emc/_deps/rlimgui-subbuild/rlimgui-populate-prefix/src/rlimgui-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/Users/Brett/Nextcloud/sources/repos/cpp/lpr/lpr-web/lpr-solver-tool/build-emc/_deps/rlimgui-subbuild/rlimgui-populate-prefix/src/rlimgui-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
