#include <stdio.h>

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
    GrandchildClass(){};

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

    return 0;
}
