# ---- Variables ----

# We use variables separate from what CTest uses, because those have
# customization issues

set(
    COVERAGE_TRACE_COMMAND
    gcovr ${PROJECT_BINARY_DIR}
    -r ${CMAKE_SOURCE_DIR}
    --html-details "${PROJECT_BINARY_DIR}/index.html"
    --exclude-unreachable-branches
    --exclude-throw-branches
    --merge-lines
    --fail-under-line 95
)

# ---- Coverage target ----

add_custom_target(
    coverage
    COMMAND ${COVERAGE_TRACE_COMMAND}
    COMMENT "Generating coverage report"
    VERBATIM
)
