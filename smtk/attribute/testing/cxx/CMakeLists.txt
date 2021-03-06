add_executable(attributeReaderWriterTest attributeReaderWriterTest.cxx)
target_link_libraries(attributeReaderWriterTest smtkCore)

set(attributeTests
  basicAttributeTest
  basicAttributeDefinitionTest
  basicAttributeDerivationTest
  basicAttributeXMLWriterTest
  childrenItemsTest
  discreteStringsWithDefaultTest
  expressionTest
  expressionTest2
  extensibleAttributeCopyTest
  fileValidationTest
  attributeAssociationTest
  attributeAutoNamingTest
  attributeReferencingTest
  categoryTest
)
set(basicAttributeXMLWriterTest_ARGS
  "${CMAKE_BINARY_DIR}/Testing/Temporary/basicAttributeXMLWriterTest.xml"
  "${CMAKE_BINARY_DIR}/Testing/Temporary/basicAttributeXMLWriterTest1.xml")
set(childrenItemsTest_ARGS "dummy.sbi")
foreach(tst ${attributeTests})
  add_executable(${tst} ${tst}.cxx)
  target_link_libraries(${tst} smtkCore ${Boost_LIBRARIES})
  add_test(NAME ${tst} COMMAND ${tst} ${${tst}_ARGS})
endforeach()

################################################################################
# Tests that require SMTK_DATA_DIR
################################################################################
if (SMTK_DATA_DIR)
  set(attributeDataTests
    attributeReaderTest
    extensibleAttributeTest
  )
  set(attributeReaderTest_ARGS
    "${SMTK_DATA_DIR}/attribute/attribute_collection/HydraTemplateV1.sbt"
  )
  set(extensibleAttributeTest_ARGS
    "${SMTK_DATA_DIR}/attribute/attribute_collection/ExtensibleTemplateTest.xml"
    "${CMAKE_BINARY_DIR}/Testing/Temporary/ExtensibleTemplateTestResult.xml"
  )
  foreach(tst ${attributeDataTests})
    add_executable(${tst} ${tst}.cxx)
    target_link_libraries(${tst} smtkCore ${Boost_LIBRARIES})
    add_test(NAME ${tst} COMMAND ${tst} ${${tst}_ARGS})
  endforeach()
endif()

################################################################################
# New tests using smtk test harness
################################################################################

set(unit_tests
  UnitTestDateTimeItem.cxx
)

smtk_unit_tests(
  Label "Attribute"
  SOURCES ${unit_tests}
  LIBRARIES smtkCore ${Boost_LIBRARIES}
)
