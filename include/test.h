#ifndef TEST_H
#define TEST_H

#include "../include/test.h"
#include "../include/DynamicArray.h"
#include "../include/LinkedList.h"
#include "../include/ArraySequence.h"
#include "../include/ListSequence.h"
#include "../include/Exceptions.h"
#include "../include/ZipUnzip.h"
#include "../include/MapUtils.h"
#include <QString>
#include <functional>
#include <sstream>

class TestRunnerGUI {
public:
    QString runAllTests();

private:
    std::stringstream output;
    int passed = 0;
    int failed = 0;

    void runTest(const std::string& name, std::function<bool()> test);
    void printSummary();

    void testDynamicArrayConstructors();
    void testDynamicArrayBasicOperations();
    void testDynamicArrayInsertErase();
    void testDynamicArrayExceptions();
    
    void testLinkedListConstructors();
    void testLinkedListBasicOperations();
    void testLinkedListInsertAt();
    void testLinkedListExceptions();
    
    void testArraySequenceConstructors();
    void testArraySequenceBasicOperations();
    void testArraySequenceFunctional();
    void testArraySequenceConcatSlice();
    void testArraySequenceIterators();
    
    void testListSequenceConstructors();
    void testListSequenceBasicOperations();
    void testListSequenceFunctional();
    void testListSequenceConcatSlice();
    void testListSequenceIterators();
    
    void testSequencePolymorphism();
    
    void testZipUnzipInt();
    void testZipUnzipDouble();
    void testZipUnzipChar();
    void testZipUnzipString();
    
    void testMapUtils();
};

#endif 