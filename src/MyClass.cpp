/*
 * MyClass.cpp
 *
 *  Created on: Dec 9, 2023
 *      Author: lochnerr
 */

#include "MyClass.h"
#include "MyClassImpl.h"

MyClass::MyClass() : m_pImpl(new MyClassImpl())
{}

MyClass::~MyClass() = default;

MyClass::MyClass(MyClass &&) noexcept = default;

MyClass& MyClass::operator=(MyClass &&) noexcept = default;

MyClass::MyClass(const MyClass& rhs)
    : m_pImpl(new MyClassImpl(*rhs.m_pImpl))
{}

MyClass& MyClass::operator=(const MyClass& rhs) {
    if (this != &rhs)
        m_pImpl.reset(new MyClassImpl(*rhs.m_pImpl));

    return *this;
}

void MyClass::DoSth()
{
    Pimpl()->DoSth();
}

void MyClass::DoConst() const
{
    Pimpl()->DoConst();
}
