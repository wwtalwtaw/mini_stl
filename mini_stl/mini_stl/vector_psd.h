//
//  vector_psd.cpp
//  mini_stl
//
//  Created by pengsida on 16/8/30.
//  Copyright © 2016年 pengsida. All rights reserved.
//

#include "allocator_psd.h"
#include "reverse_iterator_psd.h"
#include "algorithm_psd.h"

// vector完成
// 留有一个疑问，怎么像标准vector库一样实现以下代码
// mini_stl::vector<int> my_test;
// my_test.insert(my_test.begin(), 5, 5);
// cout << *(++my_test.begin()) << endl;
// 我在提出下一个疑问的时候发现，上述问题貌似是因为函数返回值类型是指针时不可修改导致的
// 详细情况见下一个疑问

// 还有个疑问
// 实现back函数的时候，为什么不能写 return *(--end());
// 而在list实现back函数的时候，却可以这么写
// 是因为函数返回值类型是指针时不可修改吗
// 我写了测试程序试了一下，貌似就是这样的
//#include <iostream>
//#include <list>
//
//using namespace std;
//
//class teee
//{
//private:
//    int* data;
//    
//public:
//    explicit teee(int* da):data(da){}
//    teee(const teee& rhs):data(rhs.data){}
//    
//    int operator*() const{return *data;}
//    
//    teee operator++()
//    {
//        ++data;
//        return *this;
//    }
//};
//
//teee test(int* a)
//{
//    return teee(a);
//}
//
//int* testt(int* a)
//{
//    return a;
//}
//
//int main()
//{
//    int te[10] = {1,2,3,4,5,6,7,8,9,10};
//    teee tee(te);
//    cout << *(++test(te)) << endl;
//    cout << *(++testt(te)) << endl; // Expression is not assignable
//    return 0;
//}

namespace mini_stl
{
    template<typename T, typename Alloc = alloc>
    class vector
    {
    public:
        typedef T value_type;
        typedef T& reference;
        typedef const T& const_reference;
        typedef T* pointer;
        typedef const T* const_pointer;
        typedef T* iterator;
        typedef const T* const_iterator;
        typedef reverse_iterator<const_iterator> const_reverse_iterator;
        typedef reverse_iterator<iterator> reverse_iterator;
        typedef ptrdiff_t difference_type;
        typedef size_t size_type;
        
    private:
        typedef vector<T, Alloc> self;
        pointer start;
        pointer finish;
        pointer end_of_storage;
        typedef allocator<value_type, Alloc> data_allocator;
        
    private:
        void initialize(size_type count, const value_type& value);
        
    public:
        // constructor
        vector():start(0),finish(0),end_of_storage(0){}
        explicit vector(size_type count, const value_type& value);
        explicit vector(size_type count);
        explicit vector(const self& rhs);
        
        self& operator=(const self& rhs);
        
        // destructor
        ~vector();
        
        // Iterator
        iterator begin();
        iterator end();
        reverse_iterator rbegin();
        reverse_iterator rend();
        const_iterator cbegin() const;
        const_iterator cend() const;
        const_reverse_iterator crbegin() const;
        const_reverse_iterator crend() const;
        
        // Capacity
        size_type size() const;
        size_type max_size() const;
        void resize(size_type new_sz, const value_type& value = value_type());
        size_type capacity() const;
        bool empty() const;
        void reserve(size_type n);
        
        // Element access
        reference operator[](size_type index);  // 后面没带const只是为了函数重载
        const_reference operator[](size_type index) const;
        reference at(size_type index);
        const_reference at(size_type index) const;
        reference front();
        const_reference front() const;
        reference back();
        const_reference back() const;
        
        // Modifiers
        void push_back(const value_type& v);
        void pop_back();
        iterator insert(iterator position, const value_type& v);
        iterator insert(iterator position, size_type n, const value_type& v);
        iterator insert(iterator position, const_iterator first, const_iterator last);
        iterator erase(iterator position);
        iterator erase(iterator first, iterator last);
        void swap(self& rhs);
        void clear();
    };
    
    //////////////////////////////////////
    // constructor
    
    template<typename T, typename Alloc>
    void vector<T,Alloc>::initialize(size_type count, const value_type& value)
    {
        start = data_allocator::allocate(count);
        end_of_storage = start + count;
        finish = uninitialized_fill_n(start, count, value);
    }
    
    template<typename T, typename Alloc>
    vector<T,Alloc>::vector(size_type count, const value_type& value)
    {
        initialize(count, value);
    }
    
    template<typename T, typename Alloc>
    vector<T,Alloc>::vector(size_type count)
    {
        initialize(count, value_type());
    }
    
    template<typename T, typename Alloc>
    vector<T,Alloc>::vector(const self& rhs)
    {
        start = data_allocator::allocate(rhs.size());
        finish = uninitialized_copy(rhs.begin(), rhs.end(), start);
    }
    
    template<typename T, typename Alloc>
    typename vector<T,Alloc>::self& vector<T,Alloc>::operator=(const self& rhs)
    {
        if(&rhs != this)
        {
            size_type rhs_size = rhs.size();
            if(rhs_size > capacity())
            {
                destroy(start, finish);
                data_allocator::deallocate(start, end_of_storage - start);
                start = data_allocator::allocate(rhs_size);
                uninitialized_copy(rhs.begin(), rhs.end(), start);
                end_of_storage = start + rhs_size;
            }
            else if(rhs_size > size())
                copy(rhs.begin(), rhs.end(), start);
            else
            {
                iterator pos = copy(rhs.begin(), rhs.end(), begin());
                destroy(pos, end());
            }
            finish = start + rhs_size;
        }
        return *this;
    }
    
    ///////////////////////////////////////
    // destructor
    
    template<typename T, typename Alloc>
    vector<T,Alloc>::~vector()
    {
        destroy(begin(), end());
        data_allocator::deallocate(begin(), end_of_storage - start);
    }
    
    ///////////////////////////////////////
    // Iterator
    
    template<typename T, typename Alloc>
    typename vector<T,Alloc>::iterator vector<T,Alloc>::begin()
    {
        return start;
    }
    
    template<typename T, typename Alloc>
    typename vector<T,Alloc>::iterator vector<T,Alloc>::end()
    {
        return finish;
    }
    
    template<typename T, typename Alloc>
    typename vector<T,Alloc>::reverse_iterator vector<T,Alloc>::rbegin()
    {
        return reverse_iterator(end());
    }
    
    template<typename T, typename Alloc>
    typename vector<T,Alloc>::reverse_iterator vector<T,Alloc>::rend()
    {
        return reverse_iterator(begin());
    }
    
    template<typename T, typename Alloc>
    typename vector<T,Alloc>::const_iterator vector<T,Alloc>::cbegin() const
    {
        return start;
    }
    
    template<typename T, typename Alloc>
    typename vector<T,Alloc>::const_iterator vector<T,Alloc>::cend() const
    {
        return finish;
    }
    
    template<typename T, typename Alloc>
    typename vector<T,Alloc>::const_reverse_iterator vector<T,Alloc>::crbegin() const
    {
        return const_reverse_iterator(cbegin());
    }
    
    template<typename T, typename Alloc>
    typename vector<T,Alloc>::const_reverse_iterator vector<T,Alloc>::crend() const
    {
        return const_reverse_iterator(cend());
    }
    
    template<typename T, typename Alloc>
    typename vector<T,Alloc>::size_type vector<T,Alloc>::size() const
    {
        return size_type(finish - start);
    }
    
    template<typename T, typename Alloc>
    typename vector<T,Alloc>::size_type vector<T,Alloc>::max_size() const
    {
        return size_type(-1) / sizeof(value_type);
    }
    
    template<typename T, typename Alloc>
    void vector<T,Alloc>::resize(size_type new_sz, const value_type& value)
    {
        if(new_sz > capacity())
            reserve(new_sz);
        if(new_sz > size())
            uninitialized_fill_n(finish, new_sz - size(), value);
        else
            destroy(start + new_sz, finish);
        finish = start + new_sz;
    }
    
    template<typename T, typename Alloc>
    typename vector<T,Alloc>::size_type vector<T,Alloc>::capacity() const
    {
        return end_of_storage - start;
    }
    
    template<typename T, typename Alloc>
    bool vector<T,Alloc>::empty() const
    {
        return finish == start;
    }
    
    // 要求vector有n的空间大小
    template<typename T, typename Alloc>
    void vector<T,Alloc>::reserve(size_type n)
    {
        n = (0 == n ? 1 : n);
        if(capacity() < n)
        {
            iterator new_start = data_allocator::allocate(capacity()*2 > n ? capacity()*2 : n);
            iterator new_finish = uninitialized_copy(begin(), end(), new_start);
            destroy(begin(), end());
            data_allocator::deallocate(start, capacity());
            start = new_start;
            finish = new_finish;
            end_of_storage = start + n;
        }
    }
    
    template<typename T, typename Alloc>
    typename vector<T,Alloc>::reference vector<T,Alloc>::operator[](size_type index)
    {
        return *(begin() + index);
    }
    
    template<typename T, typename Alloc>
    typename vector<T,Alloc>::const_reference vector<T,Alloc>::operator[](size_type index) const
    {
        return *(begin() + index);
    }
    
    template<typename T, typename Alloc>
    typename vector<T,Alloc>::reference vector<T,Alloc>::at(size_type index)
    {
        return operator[](index);
    }
    
    template<typename T, typename Alloc>
    typename vector<T,Alloc>::const_reference vector<T,Alloc>::at(size_type index) const
    {
        return operator[](index);
    }
    
    template<typename T, typename Alloc>
    typename vector<T,Alloc>::reference vector<T,Alloc>::front()
    {
        return *begin();
    }
    
    template<typename T, typename Alloc>
    typename vector<T,Alloc>::const_reference vector<T,Alloc>::front() const
    {
        return *begin();
    }
    
    template<typename T, typename Alloc>
    typename vector<T,Alloc>::reference vector<T,Alloc>::back()
    {
        return *(end() - 1);
    }
    
    template<typename T, typename Alloc>
    typename vector<T,Alloc>::const_reference vector<T,Alloc>::back() const
    {
        return *(end() - 1);
    }
    
    template<typename T, typename Alloc>
    void vector<T,Alloc>::push_back(const value_type& v)
    {
        if(end_of_storage == finish)
            reserve(capacity() * 2);
        construct(finish, v);
        finish++;
    }
    
    template<typename T, typename Alloc>
    void vector<T,Alloc>::pop_back()
    {
        --finish;
        destroy(finish);
    }
    
    // insert会返回插入位置的迭代器
    template<typename T, typename Alloc>
    typename vector<T,Alloc>::iterator vector<T,Alloc>::insert(iterator position, const value_type& v)
    {
        if(position == finish)
            push_back(v);
        else
        {
            size_type pos = position - begin();
            if(finish == end_of_storage)
                reserve(capacity() * 2);
            position = begin() + pos;
            copy_backward(position, end(), end()+1);
            construct(position, v);
            finish++;
        }
        return position;
    }
    
    template<typename T, typename Alloc>
    typename vector<T,Alloc>::iterator vector<T,Alloc>::insert(iterator position, size_type num_insert, const value_type& v)
    {
        if(num_insert != 0)
        {
            if(size_type(end_of_storage - finish) >= num_insert)
            {
                const size_type elem_after_pos = finish - position;
                if(elem_after_pos < num_insert)
                {
                    uninitialized_copy(position, end(), position + num_insert);
                    finish += num_insert;
                    fill(position, position + elem_after_pos, v);
                    uninitialized_fill(position + elem_after_pos, position + num_insert, v);
                }
                else
                {
                    uninitialized_copy(finish - num_insert, finish, finish);
                    copy_backward(position, finish - num_insert, finish);
                    finish += num_insert;
                    fill(position, position + num_insert, v);
                }
            }
            else
            {
                const size_type old_size = size();
                const size_type new_capacity = old_size > num_insert ? old_size * 2 : old_size + num_insert;
                const size_type pos = position - begin();
                pointer new_start = data_allocator::allocate(new_capacity);
                pointer new_finish = uninitialized_copy(begin(), position, new_start);
                new_finish = uninitialized_fill_n(new_finish, num_insert, v);
                new_finish = uninitialized_copy(position, end(), new_finish);
                destroy(start, finish);
                data_allocator::deallocate(start, end_of_storage - start);
                start = new_start;
                finish = new_finish;
                end_of_storage = start + new_capacity;
                position = begin() + pos;
            }
        }
        return position;
    }
    
    // 在position处插入[first, last)的元素
    template<typename T, typename Alloc>
    typename vector<T,Alloc>::iterator vector<T,Alloc>::insert(iterator position, const_iterator first, const_iterator last)
    {
        if(first != last)
        {
            size_type num_insert = 0;
            distance(first, last, num_insert);
            if(size_type(end_of_storage - finish) >= num_insert)
            {
                const size_type elem_after_pos = finish - position;
                if(elem_after_pos < num_insert)
                {
                    uninitialized_copy(position, end(), position + num_insert);
                    finish += num_insert;
                    copy(first, first + elem_after_pos, position);
                    uninitialized_copy(first + elem_after_pos, last, end());
                }
                else
                {
                    iterator old_finish = finish;
                    uninitialized_copy(finish - num_insert, finish, finish);
                    copy_backward(position, finish - num_insert, finish);
                    finish += num_insert;
                    copy(first, last, position);
                }
            }
            else
            {
//                size_type pos = position - begin();
//                reserve(capacity() + num_insert);
//                position = begin() + pos;
//                return insert(position, first, last);
                // 上面的代码虽然比较取巧，为了考虑效率，决定用下面的方式实现
                const size_type old_size = size();
                const size_type pos = position - begin();
                const size_type new_capacity = old_size > num_insert ? old_size * 2 : old_size + num_insert;
                pointer new_start = data_allocator::allocate(new_capacity);
                pointer new_finish = uninitialized_copy(begin(), position, new_start);
                new_finish = uninitialized_copy(first, last, new_finish);
                new_finish = uninitialized_copy(position, end(), new_finish);
                destroy(start, finish);
                data_allocator::deallocate(start, end_of_storage - start);
                start = new_start;
                finish = new_finish;
                end_of_storage = new_start + new_capacity;
                position = begin() + pos;
            }
        }
        return position;
    }
    
    template<typename T, typename Alloc>
    typename vector<T,Alloc>::iterator vector<T,Alloc>::erase(iterator position)
    {
        if(position + 1 != end())
            copy(position + 1, end(), position);
        pop_back();
        return position;
    }
    
    template<typename T, typename Alloc>
    typename vector<T,Alloc>::iterator vector<T,Alloc>::erase(iterator first, iterator last)
    {
        if(last != first)
        {
            if(first == start && last == finish)
            {
                clear();
                return end();
            }
            else
            {
                const size_type len = last - first;
                iterator position = copy(last, end(), first);
                destroy(position, end());
                finish = finish - len;
            }
        }
        return first;
    }
    
    template<typename T, typename Alloc>
    void vector<T,Alloc>::swap(self& rhs)
    {
        swap(start, rhs.start);
        swap(finish, rhs.finish);
        swap(end_of_storage, rhs.end_of_storage);
    }
    
    // Removes all elements from the container
    template<typename T, typename Alloc>
    void vector<T,Alloc>::clear()
    {
        destroy(start, finish);
        finish = start;
    }
}
