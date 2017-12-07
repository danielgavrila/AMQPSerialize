
function (compileArgs projName)

target_compile_options(${projName} PUBLIC -fconcepts)
set_property(TARGET ${projName} PROPERTY CXX_STANDARD 17)
set_target_properties(${projName} PROPERTIES
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS ON
)

endfunction()
