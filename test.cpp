#include <stdio.h>
#include <vector>

class BaseClass
{
private:
public:
    BaseClass(){};

    virtual void justInBase();
    virtual void doThing();
    virtual void PassToGrandChild();
};

void BaseClass::doThing()
{
    printf("Base class version did thing\n");
}

void BaseClass::justInBase()
{
    printf("This is just in the base class\n");
}

void BaseClass::PassToGrandChild()
{
    printf("This will be passed from base to grandchild\n");
}

class ChildClass : public BaseClass
{
private:
public:
    ChildClass(){};

    void doThing();
};

void ChildClass::doThing()
{
    printf("Child class version did thing\n");
}

class GrandchildClass : public ChildClass
{
private:
public:
    int extraAttribute;

    GrandchildClass()
    {
        extraAttribute = 9999;
    };

    void doThing();
    void PassToGrandChild();
};

void GrandchildClass::PassToGrandChild()
{
    printf("This has been passed all the way to the grandchild\n");
}

void GrandchildClass::doThing()
{
    printf("Grandchild class version did thing\n");
}

void iterateOverVectorOfPointers(std::vector<BaseClass *> *vec)
{
    for (auto &&i : *vec)
    {
        i->doThing();
    }
}

void iterateOverVectorOfPointersWithDynamicCasting(std::vector<BaseClass *> *vec)
{
    for (auto &&i : *vec)
    {
        auto p = dynamic_cast<GrandchildClass *>(i);
        if (p != nullptr)
        {
            printf("Filtered out the grandchild %d\n", p->extraAttribute);
        }
    }
}

int main(int argc, char const *argv[])
{
    BaseClass *test = new GrandchildClass();
    BaseClass *test2 = new BaseClass();
    BaseClass *test3 = new ChildClass();

    BaseClass test4 = GrandchildClass();
    test4.PassToGrandChild();

    test->doThing();
    test->justInBase();
    test->PassToGrandChild();
    test2->doThing();
    test3->doThing();

    printf("=============================================\n");

    std::vector<BaseClass *> testvec;

    for (size_t i = 0; i < 10; i++)
    {
        BaseClass *x;
        x = new ChildClass();
        testvec.push_back(x);
    }

    testvec.push_back(test);

    iterateOverVectorOfPointers(&testvec);
    printf("=============================================\n");
    iterateOverVectorOfPointersWithDynamicCasting(&testvec);
    return 0;
}
