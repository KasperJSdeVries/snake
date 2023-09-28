add_subdirectory(glad)

option(CGLM_SHARED "Shared build" ON)
option(CGLM_STATIC "Static build" OFF)
option(CGLM_USE_C99 "" OFF) # C11
option(CGLM_USE_TEST "Enable Tests" OFF)
add_subdirectory(cglm)