# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "D:/Project DS Final/build/_deps/asio-src"
  "D:/Project DS Final/build/_deps/asio-build"
  "D:/Project DS Final/build/_deps/asio-subbuild/asio-populate-prefix"
  "D:/Project DS Final/build/_deps/asio-subbuild/asio-populate-prefix/tmp"
  "D:/Project DS Final/build/_deps/asio-subbuild/asio-populate-prefix/src/asio-populate-stamp"
  "D:/Project DS Final/build/_deps/asio-subbuild/asio-populate-prefix/src"
  "D:/Project DS Final/build/_deps/asio-subbuild/asio-populate-prefix/src/asio-populate-stamp"
)

set(configSubDirs Debug)
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "D:/Project DS Final/build/_deps/asio-subbuild/asio-populate-prefix/src/asio-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "D:/Project DS Final/build/_deps/asio-subbuild/asio-populate-prefix/src/asio-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
