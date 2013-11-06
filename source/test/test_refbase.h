#ifndef __TEST_REFBASE_H_
#define __TEST_REFBASE_H_
#include "util/refbase.h"

int gstate = 0;
class TestRefBase : public RefBase{
    virtual ~TestRefBase(){
        //gstate = 1;
    }
};

TINYTEST_SUIT(RefBase);

TINYTEST_CASE(RefBase, constructor)
{
    TestRefBase *p = new TestRefBase();
    sp<TestRefBase> ref = p;
    TINYTEST_EQUAL(ref.get(), p);
    
    TEST_END();
}

TINYTEST_CASE(RefBase, assign)
{
    TestRefBase *p = new TestRefBase();
    sp<TestRefBase> ref = p;
    sp<TestRefBase> ref2 = ref;
    TINYTEST_EQUAL(ref2.get(), p);
    
    TEST_END();
}

TINYTEST_CASE(RefBase, clear)
{
    TestRefBase *p = new TestRefBase();
    sp<TestRefBase> ref = p;
    sp<TestRefBase> ref2 = ref;
    TINYTEST_EQUAL(ref2.get(), p);
    ref2.clear();
    TINYTEST_EQUAL(ref2.get(), NULL);
    
    TEST_END();
}

TINYTEST_CASE(RefBase, destructor)
{
    TestRefBase *p = new TestRefBase();
    sp<TestRefBase> ref = p;
    {
        sp<TestRefBase> ref3 = ref;
        ref.clear();
        gstate = 0;
        ref3.clear();
        TINYTEST_EQUAL(gstate, 1);
    }
    TEST_END();
}

#endif