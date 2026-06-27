set(TESTBASETARGETNAME testgenericbase)
add_library(${TESTBASETARGETNAME} OBJECT "tests/base/base.c")
target_include_directories(${TESTBASETARGETNAME} PUBLIC "tests/base/")
target_link_libraries(${TESTBASETARGETNAME} PUBLIC ${PROJECT_NAME})

macro(test name sources)
    add_executable(${name} ${sources})
    target_link_libraries(${name} PRIVATE ${TESTBASETARGETNAME})
endmacro()

# =============================================================================

test(test0 "tests/0.c")
test(test1 "tests/1.c")
test(test2 "tests/2.c")