# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(zephyr_codec2)

target_sources(app PRIVATE src/main.c
codec2/dump.c
codec2/lpc.c
codec2/nlp.c
codec2/postfilter.c
codec2/sine.c
codec2/codec2.c
codec2/codec2_fft.c
codec2/fifo.c
codec2/kiss_fft.c
codec2/kiss_fftr.c
codec2/interp.c
codec2/lsp.c
codec2/mbest.c
# codec2/newamp1.c
codec2/phase.c
codec2/quantise.c
codec2/pack.c
codec2/codebook.c
codec2/codebookd.c
codec2/codebookjvm.c
codec2/codebookge.c)

target_include_directories(app
  PRIVATE
  ${PROJECT_SOURCE_DIR}/codec2
)
# zephyr_get_include_directories_for_lang_as_string(       C includes)
# zephyr_get_system_include_directories_for_lang_as_string(C system_includes)
# zephyr_get_compile_definitions_for_lang_as_string(       C definitions)
# zephyr_get_compile_options_for_lang_as_string(           C options)

# set(external_project_cflags
#   "${includes} ${definitions} ${options} ${system_includes}"
# )

add_compile_definitions(ARM_MATH_CM33 CORTEX_M33)

# add_library(codec2 STATIC
#   codec2/dump.c
#   codec2/lpc.c
#   codec2/nlp.c
#   codec2/postfilter.c
#   codec2/sine.c
#   codec2/codec2.c
#   codec2/codec2_fft.c
#   codec2/fifo.c
#   codec2/kiss_fft.c
#   codec2/kiss_fftr.c
#   codec2/interp.c
#   codec2/lsp.c
#   codec2/mbest.c
#   codec2/newamp1.c
#   codec2/phase.c
#   codec2/quantise.c
#   codec2/pack.c
#   codec2/codebook.c
#   codec2/codebookd.c
#   codec2/codebookjvm.c
#   codec2/codebookge.c
# )

# target_include_directories(codec2
#   PUBLIC
#   ${PROJECT_SOURCE_DIR}/codec2
#   # PRIVATE
#   # ${includes}
#   # SYSTEM
#   # ${system_includes}
#   C:/ncs/v2.0.0/modules/hal/cmsis/CMSIS/Core/Include
#   C:/ncs/v2.0.0/modules/hal/cmsis/CMSIS/DSP/Include
#   C:/ncs/v2.0.0/modules/hal/nordic/nrfx/mdk
# )

# target_compile_definitions(codec2
#   PUBLIC
#   ${definitions}
# )

# target_compile_options(codec2
#   PUBLIC
#   ${options}
#   -O0
# )

# target_link_libraries(app 
#     PUBLIC
#     codec2
# )
