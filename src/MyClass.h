/*
 * MyClass.h
 *
 *  Created on: Dec 9, 2023
 *      Author: lochnerr
 */

#ifndef MYCLASS_H_
#define MYCLASS_H_

#include <memory>	// unique_ptr

class MyClassImpl;
class MyClass
{
public:
    explicit MyClass();
    ~MyClass();

    // movable:
    MyClass(MyClass && rhs) noexcept;
    MyClass& operator=(MyClass && rhs) noexcept;

    // and copyable
    MyClass(const MyClass& rhs);
    MyClass& operator=(const MyClass& rhs);

    void DoSth();
    void DoConst() const;

private:
    const MyClassImpl* Pimpl() const { return m_pImpl.get(); }
    MyClassImpl* Pimpl() { return m_pImpl.get(); }

    std::unique_ptr<MyClassImpl> m_pImpl;
};


#endif /* MYCLASS_H_ */
