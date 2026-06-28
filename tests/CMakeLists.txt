# Developer-only unit tests for the Entitlement module. Not part of the shipped
# app target. Build with:  cmake -B build-tests -DENABLE_TESTS=ON
# Run with:                 ctest --test-dir build-tests -V

find_package(Qt6 REQUIRED COMPONENTS Core Test)

set(_entitlement_sources
    ${CMAKE_SOURCE_DIR}/src/Entitlement/EntitlementEras.cpp
    ${CMAKE_SOURCE_DIR}/src/Entitlement/Provenance.cpp
    ${CMAKE_SOURCE_DIR}/src/Entitlement/ProvenanceStore.cpp
)

function(theengs_add_unit_test name)
    qt_add_executable(${name} ${name}.cpp ${_entitlement_sources})
    target_include_directories(${name} PRIVATE ${CMAKE_SOURCE_DIR}/src)
    target_link_libraries(${name} PRIVATE Qt6::Core Qt6::Test)
    set_target_properties(${name} PROPERTIES
        AUTOMOC ON
        WIN32_EXECUTABLE OFF
        MACOSX_BUNDLE OFF
    )
    add_test(NAME ${name} COMMAND ${name})
endfunction()

theengs_add_unit_test(test_entitlement_eras)
theengs_add_unit_test(test_provenance)
theengs_add_unit_test(test_provenance_store)
